//
// Created by Vito Bellini on 08/12/2018.
//

#include "similarity.h"

#include <cmath>

#include "../cnpy/cnpy.h"

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
    for(int j=0; j<=i; j++) {
        const double s = cosine_similarity(u, users[j], y);
        sims[i][j] = s;
        sims[j][i] = s;
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