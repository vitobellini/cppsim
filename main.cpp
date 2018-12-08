#include <iostream>
#include <string>
#include <future>
#include <iterator>

#include <boost/asio.hpp>
#include <boost/asio/thread_pool.hpp>
#include <boost/thread.hpp>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

#include "cnpy/cnpy.h"

#include "src/similarity.h"

namespace po = boost::program_options;
namespace fs = boost::filesystem;

int main(int argc, char* argv[]) {
    std::string ifile;
    std::string ofile;
    std::string dir_path;
    bool split_matrix = false;

    std::string users_path;
    bool index_user = false;
    std::vector<unsigned int> users_index;

    const unsigned int hw_max_concurrency = std::thread::hardware_concurrency();
    unsigned int hw_concurrency = hw_max_concurrency;

    po::options_description desc("Allowed options");
    desc.add_options()
            ("help,h", "print usage message")
            ("input,i", po::value(&ifile), "path for input matrix file")
            ("output,o", po::value(&ofile), "path for output matrix file")
            ("dir,d", po::value(&dir_path), "output directory for row vectors")
            ("split,s", po::bool_switch(&split_matrix), "split matrix into row vectors")
            ("users,u", po::value(&users_path), "path for users index file to compute similarities")
            ("threads,t", po::value(&hw_concurrency), "threads")
            ;

    po::variables_map vm;
    //po::store(po::parse_command_line(argc, argv, desc), vm);
    po::store(po::command_line_parser(argc, argv).options(desc).allow_unregistered().run(), vm);

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

    if (vm.count("threads")) {
        if (hw_concurrency > hw_max_concurrency) {
            hw_concurrency = hw_max_concurrency;
        }

        std::cout << "Threads: " << hw_max_concurrency << std::endl;
    }

    if (vm.count("users")) {
        index_user = true;

        std::ifstream inputFile(users_path);

        while (!inputFile.eof()) {
            unsigned int x;
            inputFile >> x;
            users_index.push_back(x);
        }

        if (!users_index.size()) {
            std::cerr << "User indexes file is empty!" << std::endl;
            exit(EXIT_FAILURE);
        }

        inputFile.close();
    }

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

    if (!index_user) {
        // Compute full matrix similarities
        for (unsigned int i = 0; i < x; i++) {
            if (split_matrix) {
                boost::asio::post(pool, boost::bind(user_sims, boost::cref(dir_path), boost::cref(users),
                                                    boost::cref(users[i]), i, x, y));
            } else {
                boost::asio::post(pool,
                                  boost::bind(users_sims, boost::cref(sims), boost::cref(users), boost::cref(users[i]),
                                              i, x, y));
            }
        }
    } else {
        // Compute similarities only for the provided indexes
        for (unsigned int i = 0; i<users_index.size(); i++) {
            unsigned int u = users_index.at(i);
            boost::asio::post(pool, boost::bind(user_sims, boost::cref(dir_path), boost::cref(users),
                                                boost::cref(users[u]), u, x, y));
        }
    }

    pool.join();

    if(!split_matrix) {
        std::cout << "Writing " << ofile << std::endl;
        cnpy::npy_save(ofile, (const double*) &sims[0],{x,x},"w");
        delete[] sims;
        delete[] users;
    } else {
        std::cout << "Done" << std::endl;
    }

    return 0;
}