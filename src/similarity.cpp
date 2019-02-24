//
// Created by Vito Bellini on 08/12/2018.
//

#include "similarity.h"

#include <cmath>

#include "../cnpy/cnpy.h"

void user_sims(std::string dir, double* m, unsigned long i, unsigned long x, unsigned long y) {

    double *userA = new double[y];

    double *s = new double[x];

    double *u = new double[y];
    std::memcpy(u, m+(i*y), sizeof(double)*y);

    for(int j=0; j<x; j++) {
        std::memcpy(userA, m+(j*y), sizeof(double)*y);
        s[j] = cosine_similarity(u, userA, y);
    }

    std::stringstream fmt;
    fmt << dir << "/" << i << ".npy";
    std::string filename = fmt.str();
    cnpy::npy_save(filename, (const double*) &s[0],{x},"w");

    delete[] s;
    delete[] userA;
    delete[] u;
}

void users_sims(double** sims, double* m, unsigned long i, unsigned long x, unsigned long y) {
    double *userA = new double[y];
    double *userB = new double[y];

    std::memcpy(userA, m+(i*y), sizeof(double)*y);

    for(int j=0; j<=i; j++) {
        std::memcpy(userB, m+(j*y), sizeof(double)*y);

        const double s = cosine_similarity(userA, userB, y);
        sims[i][j] = s;
        sims[j][i] = s;
    }

    delete[] userA;
    delete[] userB;
}

double cosine_similarity(double *A, double *B, unsigned long size) {
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