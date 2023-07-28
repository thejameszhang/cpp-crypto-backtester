// operators.h
#include <iostream>
#include <vector>

typedef long double ld;

#ifndef OPERATORS_H
#define OPERATORS_H

// Sum and absolute sum to 1.
void weight_long(std::vector<ld>& holdings);

// Sum and absolute sum to -1.
void weight_short(std::vector<ld>& holdings);

// Sum to 0 and absolute sum to 1.
void weight(std::vector<ld>& holdings);

#endif // OPERATORS_H