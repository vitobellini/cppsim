#include <iostream>
#include <cmath>
#include <string>
#include <future>

#include <boost/asio.hpp>
#include <boost/asio/thread_pool.hpp>
#include <boost/thread.hpp>

#include "cnpy.h"

double cosine_similarity(double *A, double *B, unsigned int size);
void users_sims(double** sims, double** users, double *u, unsigned long i, unsigned long x, unsigned long y);
void user_sims(double** sims, double** users, double *u, unsigned long i, unsigned long x, unsigned long y);

int main() {
    std::cout << "Reading numpy matrix..." << std::endl;

    cnpy::NpyArray m = cnpy::npy_load("A.npy");
    std::cout << "Matrix shape: " << m.shape[0] << ", " << m.shape[1] << std::endl;

    unsigned long x = m.shape[0];
    unsigned long y = m.shape[1];

    // Match word size
    assert(m.word_size == sizeof(double));

    double** users = new double*[x];
    for(int i=0; i<x; i++) {
        users[i] = new double[y];
    }

    std::cout << "Allocating matrix similarities" << std::endl;

    double** sims = new double*[x];
    for(int i=0; i<x; i++) {
        sims[i] = new double[x];
        //std::fill(sims[0], sims[x], 0);
    }

    double* A = m.data<double>();

    std::cout << "Obtaining raw data" << std::endl;

    for(unsigned long i=0; i<x*y; i++) {
        unsigned long row = i/y;
        unsigned long col = i % y;
        users[row][col] = A[i];
    }

    boost::asio::thread_pool pool(12);

    std::cout << "Computing similarities..." << std::endl;

    for(unsigned int i=0; i<x; i++) {
        boost::asio::post(pool, boost::bind(user_sims, boost::cref(sims), boost::cref(users), boost::cref(users[i]), i, x, y));
    }

    pool.join();

    //std::cout << "Writing sims.npy" << std::endl;
    //cnpy::npy_save("sims.npy", (const double*) &sims[0],{x,x},"w");

    return 0;
}

void user_sims(double** sims, double** users, double *u, unsigned long i, unsigned long x, unsigned long y) {

    double *s = new double[x];

    for(int j=0; j<x; j++) {
        s[j] = cosine_similarity(u, users[j], y);
    }

    std::stringstream fmt;
    fmt << "sims/" << i << ".npy";
    std::string filename = fmt.str();
    cnpy::npy_save(filename, (const double*) &s[0],{x},"w");

    delete[] s;
}

void users_sims(double** sims, double** users, double *u, unsigned long i, unsigned long x, unsigned long y) {

    for(int j=0; j<x; j++) {
        sims[i][j] = cosine_similarity(u, users[j], y);
    }

    std::cout << "User: " << i+1 << "/" << x << std::endl;
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