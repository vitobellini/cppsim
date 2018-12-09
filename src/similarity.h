//
// Created by Vito Bellini on 08/12/2018.
//

#ifndef CPPSIM_SIMILARITY_H
#define CPPSIM_SIMILARITY_H

#include <string>

double cosine_similarity(double *A, double *B, unsigned long size);
void users_sims(double** sims, double** users, unsigned long i, unsigned long x, unsigned long y);
void user_sims(std::string dir, double** users, double *u, unsigned long i, unsigned long x, unsigned long y);

#endif //CPPSIM_SIMILARITY_H
