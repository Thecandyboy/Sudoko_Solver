// dpll.cpp
#include "dpll.h"
#include <iostream>
#include <unordered_set>
#include <vector>
using namespace std;

typedef unordered_set<int> Clause;
typedef vector<Clause> CNF;

// Function to check if a clause contains a literal
bool containsLiteral(const Clause &clause, int literal) {
    return clause.find(literal) != clause.end();
}

// Function to remove a literal from a clause
void removeLiteralFromClause(Clause &clause, int literal) {
    clause.erase(literal);
}

// Simplify CNF by assigning a literal
CNF simplify(const CNF &cnf, int literal) {
    CNF newCnf;
    for (const auto &clause : cnf) {
        if (containsLiteral(clause, literal)) {
            // Clause is satisfied, so skip it.
            continue;
        }
        Clause newClause = clause;
        if (containsLiteral(clause, -literal)) {
            // Remove the negated literal from the clause.
            removeLiteralFromClause(newClause, -literal);
        }
        // Only keep clauses that are non-empty.
        if (!newClause.empty()) {
            newCnf.push_back(newClause);
        }
    }
    return newCnf;
}

// Recursive DPLL SAT solver
bool dpll(const CNF &cnf) {
    // Base case: empty CNF (all clauses satisfied).
    if (cnf.empty()) {
        return true;
    }

    // If any clause is empty, it means the formula is unsatisfiable.
    for (const auto &clause : cnf) {
        if (clause.empty()) {
            return false;
        }
    }

    // Choose a literal to assign (heuristic can be added here)
    int literal = *cnf[0].begin();

    // Try assigning the literal to true and simplify the formula
    if (dpll(simplify(cnf, literal))) {
        return true;
    }

    // If assigning literal to true doesn't work, try assigning it to false
    return dpll(simplify(cnf, -literal));
}
