#include <iostream>
#include <cmath>
#include <string>
#include <future>

#include <boost/asio.hpp>
#include <boost/asio/thread_pool.hpp>
#include <boost/thread.hpp>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

#include "cnpy.h"

double cosine_similarity(double *A, double *B, unsigned int size);
void users_sims(double** sims, double** users, double *u, unsigned long i, unsigned long x, unsigned long y);
void user_sims(std::string dir, double** users, double *u, unsigned long i, unsigned long x, unsigned long y);

namespace po = boost::program_options;
namespace fs = boost::filesystem;

int main(int argc, char* argv[]) {

    std::string ifile;
    std::string ofile;
    std::string dir_path;
    bool split_matrix = false;

    po::options_description desc("Allowed options");
    desc.add_options()
            ("help,h", "print usage message")
            ("input,i", po::value(&ifile), "pathname for input matrix")
            ("output,o", po::value(&ofile), "pathname for output matrix")
            ("dir,d", po::value(&dir_path), "output directory for row vectors")
            ("split,s", po::bool_switch(&split_matrix), "split matrix into row vectors")
            ;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
        std::cout << desc << std::endl;
        exit(EXIT_SUCCESS);
    }

    if (!vm.count("input")) {
        std::cout << desc << std::endl;
        exit(EXIT_SUCCESS);
    }

    if(split_matrix && !vm.count("dir")) {
        std::cout << desc << std::endl;
        exit(EXIT_SUCCESS);
    }

    if(!split_matrix && !vm.count("output")) {
        std::cout << desc << std::endl;
        exit(EXIT_SUCCESS);
    }

    if (vm.count("dir")) {
        fs::path dir(dir_path);

        if(!fs::is_directory(dir) && !fs::create_directory(dir)) {
            std::cerr << "Unable to create directory: " << dir_path << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    unsigned hw_concurrency = std::thread::hardware_concurrency();

    std::cout << "Reading numpy matrix: " << ifile << std::endl;

    cnpy::NpyArray m = cnpy::npy_load(ifile);
    std::cout << "Matrix shape: " << m.shape[0] << ", " << m.shape[1] << std::endl;

    unsigned long x = m.shape[0];
    unsigned long y = m.shape[1];

    // Match word size
    assert(m.word_size == sizeof(double));

    double** users = new double*[x];
    for (int i = 0; i < x; i++) {
        users[i] = new double[y];
    }


    std::cout << "Allocating matrix similarities" << std::endl;

    double** sims = NULL;
    if(!split_matrix) {
        sims = new double*[x];
        for (int i = 0; i < x; i++) {
            sims[i] = new double[x];
        }
    }

    double* A = m.data<double>();

    std::cout << "Obtaining raw data" << std::endl;

    for(unsigned long i=0; i<x*y; i++) {
        unsigned long row = i/y;
        unsigned long col = i % y;
        users[row][col] = A[i];
    }

    boost::asio::thread_pool pool(hw_concurrency);

    std::cout << "Computing similarities..." << std::endl;

    for(unsigned int i=0; i<x; i++) {
        if(split_matrix) {
            boost::asio::post(pool, boost::bind(user_sims, boost::cref(dir_path), boost::cref(users), boost::cref(users[i]), i, x, y));
        } else {
            boost::asio::post(pool, boost::bind(users_sims, boost::cref(sims), boost::cref(users), boost::cref(users[i]), i, x, y));
        }
    }

    pool.join();

    if(!split_matrix) {
        std::cout << "Writing " << ofile << ".npy" << std::endl;
        cnpy::npy_save(ofile, (const double*) &sims[0],{x,x},"w");
        delete[] sims;
        delete[] users;
    } else {
        std::cout << "Done" << std::endl;
    }

    return 0;
}

void user_sims(std::string dir, double** users, double *u, unsigned long i, unsigned long x, unsigned long y) {

    double *s = new double[x];

    for(int j=0; j<x; j++) {
        s[j] = cosine_similarity(u, users[j], y);
    }

    std::stringstream fmt;
    fmt << dir << "/" << i << ".npy";
    std::string filename = fmt.str();
    cnpy::npy_save(filename, (const double*) &s[0],{x},"w");

    delete[] s;
}

void users_sims(double** sims, double** users, double *u, unsigned long i, unsigned long x, unsigned long y) {
    for(int j=0; j<x; j++) {
        sims[i][j] = cosine_similarity(u, users[j], y);
    }
}

double cosine_similarity(double *A, double *B, unsigned int size) {
    double mul = 0.0;
    double d_a = 0.0;
    double d_b = 0.0 ;

    for(unsigned int i = 0; i < size; ++i) {
        mul += *A * *B;
        d_a += *A * *A;
        d_b += *B * *B;
        A++;
        B++;
    }

    if (d_a == 0.0f || d_b == 0.0f) {
        return 0.0f;
    }

    return mul / (sqrt(d_a) * sqrt(d_b));
}