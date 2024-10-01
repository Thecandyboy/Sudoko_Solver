#include <vector>
#include <unordered_map>

// Define the types used
using Literal = int;
using Clause = std::vector<Literal>;
using CNFFormula = std::vector<Clause>;

// Declare the functions
bool DPLL(CNFFormula formula, std::unordered_map<Literal, bool> &assignments);
void printAssignments(const std::unordered_map<Literal, bool> &assignments);
