// dpll.h
#ifndef DPLL_H
#define DPLL_H

#include <vector>
#include <unordered_set>

typedef std::unordered_set<int> Clause;
typedef std::vector<Clause> CNF;

bool dpll(const CNF &cnf);

#endif
