//
// Created by Vito Bellini on 07/12/2018.
//

#define BOOST_TEST_MODULE Test
#include <boost/test/unit_test.hpp>
#include <iostream>

#include "similarity.h"
#include "../cnpy/cnpy.h"

using namespace boost::unit_test;

BOOST_AUTO_TEST_SUITE(sims)

    BOOST_AUTO_TEST_CASE(test_case1) {
        cnpy::NpyArray input = cnpy::npy_load("input.npy");
        cnpy::NpyArray output = cnpy::npy_load("output.npy");

        unsigned long x = input.shape[0];
        unsigned long y = input.shape[1];

        // Match word size
        BOOST_CHECK_EQUAL(input.word_size, sizeof(double));

        double** users = new double*[x];
        for (int i = 0; i < x; i++) {
            users[i] = new double[y];
        }

        double** sims = new double*[x];
        for (int i = 0; i < x; i++) {
            sims[i] = new double[x];
        }

        double** tsims = new double*[x];
        for (int i = 0; i < x; i++) {
            tsims[i] = new double[x];
        }

        double* A = input.data<double>();

        for(unsigned long i=0; i<x*y; i++) {
            unsigned long row = i/y;
            unsigned long col = i % y;
            users[row][col] = A[i];
        }

        double* B = output.data<double>();

        for(unsigned long i=0; i<x*x; i++) {
            unsigned long row = i/x;
            unsigned long col = i % x;
            tsims[row][col] = B[i];
        }

        for(unsigned long i=0; i<x; i++) {
            users_sims(sims, users, i, x, y);
        }

        for(unsigned int i=0; i<x; i++) {
            for(unsigned int j=0; j<x; j++) {
                BOOST_TEST(sims[i][j], tsims[i][j]);
            }
        }

        delete[] users;
        delete[] sims;
    }

BOOST_AUTO_TEST_SUITE_END()