#include <iostream>
#include <unordered_map>
#include <vector>
#include <set>
#include <algorithm>
#include "dpll.h"

using namespace std;

using Literal = int;
using Clause = std::vector<Literal>;
using CNFFormula = std::vector<Clause>;

// Function to check if a clause is satisfied
bool isClauseSatisfied(const Clause &clause, const std::unordered_map<Literal, bool> &assignments) {
    for (Literal literal : clause) {
        int var = abs(literal);
        if (assignments.count(var)) {
            if ((literal > 0 && assignments.at(var)) || (literal < 0 && !assignments.at(var))) {
                return true;  // Clause is satisfied
            }
        }
    }
    return false;  // Clause is not satisfied
}

// Function to check if the formula is satisfied
bool isFormulaSatisfied(const CNFFormula &formula, const std::unordered_map<Literal, bool> &assignments) {
    for (const Clause &clause : formula) {
        if (!isClauseSatisfied(clause, assignments)) {
            return false; // Found a clause that is not satisfied
        }
    }
    return true; // All clauses are satisfied
}

// Unit propagation: propagate unit clauses and assign values to the literals
bool unitPropagation(CNFFormula &formula, std::unordered_map<Literal, bool> &assignments) {
    bool changed = true;
    while (changed) {
        changed = false;
        for (auto it = formula.begin(); it != formula.end();) {
            const Clause &clause = *it;
            if (clause.size() == 1) {  // Found a unit clause
                Literal unit = clause[0];
                int var = abs(unit);
                bool value = (unit > 0);  // True if positive literal, false if negative
                if (assignments.count(var) == 0) {  // If var is not assigned
                    assignments[var] = value;  // Assign the unit clause
                    changed = true;

                    // Remove clauses containing the unit literal and modify others
                    CNFFormula newFormula;
                    for (const Clause &c : formula) {
                        // If clause is satisfied by the current assignment, skip it
                        if (isClauseSatisfied(c, assignments)) {
                            continue;
                        }
                        // Create a new clause
                        Clause newClause;
                        for (Literal literal : c) {
                            // If the literal is the unit literal, skip it (it’s satisfied)
                            if (literal == unit) {
                                continue;
                            }
                            // If the literal is the negation of the unit literal, we remove it
                            if (literal == -unit) {
                                continue;  // Don't add the negated unit literal
                            }
                            newClause.push_back(literal);  // Keep other literals
                        }
                        if (!newClause.empty()) {
                            newFormula.push_back(newClause);  // Only add non-empty clauses
                        }
                    }
                    formula = newFormula;  // Update the formula
                } else if (assignments[var] != value) {
                    return false;  // Conflict found
                }
                it = formula.begin();  // Reset iterator to start after a modification
            } else {
                ++it;  // Move to the next clause
            }
        }
    }
    return true;  // No conflict found
}

// Pure literal elimination: eliminate pure literals from the formula
bool pureLiteralElimination(CNFFormula &formula, std::unordered_map<Literal, bool> &assignments) {
    std::set<Literal> literals;
    for (const Clause &clause : formula) {
        for (Literal literal : clause) {
            literals.insert(literal);  // Set of all literals
        }
    }

    for (Literal literal : literals) {
        if (literals.count(-literal) == 0) {  // Pure literal found
            int var = abs(literal);
            bool value = (literal > 0);
            assignments[var] = value;

            // Remove clauses containing the pure literal
            CNFFormula newFormula;
            for (const Clause &clause : formula) {
                if (std::find(clause.begin(), clause.end(), literal) == clause.end()) {
                    newFormula.push_back(clause);  // Keep the clause if it doesn't contain the pure literal
                }
            }
            formula = newFormula;  // Update the formula
        }
    }
    return true;
}

// Select an unassigned variable for the next decision
Literal selectUnassignedVariable(const CNFFormula &formula, const std::unordered_map<Literal, bool> &assignments) {
    for (const Clause &clause : formula) {
        for (Literal literal : clause) {
            int var = abs(literal);
            if (assignments.count(var) == 0) {
                return literal;  // Return the literal directly
            }
        }
    }
    return 0;  // All variables are assigned
}

// DPLL algorithm: recursively search for a satisfying assignment
bool DPLL(CNFFormula formula, std::unordered_map<Literal, bool> &assignments) {
    // Step 1: Apply unit propagation
    if (!unitPropagation(formula, assignments)) {
        return false;  // Found a conflict (empty clause)
    }

    // Check for termination condition: empty formula is satisfiable
    if (formula.empty()) {
        return true;  // Formula is satisfied (trivially true)
    }

    // Check for unsatisfied clauses
    for (const Clause &clause : formula) {
        if (!isClauseSatisfied(clause, assignments)) {
            // At least one clause is unsatisfied
            break;
        }
    }

    // Step 2: Apply pure literal elimination
    pureLiteralElimination(formula, assignments);

    // Step 3: Select an unassigned variable
    Literal var = selectUnassignedVariable(formula, assignments);
    if (var == 0) {
        return false;  // No unassigned variables left, but formula is not satisfied
    }

    // Step 4: Recursively assign true and false to the variable
    std::vector<bool> values;  // Declare the vector
    values.push_back(true);     // Add true
    values.push_back(false);    // Add false

    for (bool value : values) {  // Iterate through the values
        assignments[abs(var)] = value;  // Assign the variable

        // Create a new formula after making the assignment
        CNFFormula newFormula;
        for (const Clause &clause : formula) {
            if (isClauseSatisfied(clause, assignments)) {
                continue;  // Clause satisfied, skip this clause
            }

            // Create a new clause without the negated literal
            Clause newClause;
            for (Literal literal : clause) {
                if (literal != (value ? -var : var)) {
                    newClause.push_back(literal);  // Keep other literals
                }
            }

            // Only add non-empty clauses
            if (!newClause.empty()) {
                newFormula.push_back(newClause);
            }
        }

        // Recursive call with updated formula and assignments
        if (DPLL(newFormula, assignments)) {
            return true;  // Found a satisfying assignment
        }

        // Backtrack: reset the variable assignment
        assignments.erase(abs(var));
    }
    return false;  // No satisfying assignment found
}

// Helper function to print the assignments
void printAssignments(const std::unordered_map<Literal, bool> &assignments) {
    for (const auto &[var, value] : assignments) {
        std::cout << "Variable " << var << " = " << (value ? "true" : "false") << "\n";
    }
}
