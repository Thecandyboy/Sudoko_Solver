#include <vector>
#include <unordered_map>

// Define types for literals and CNF formula
using Literal = int;
using Clause = std::vector<Literal>;
using CNFFormula = std::vector<Clause>;

void encodeSudokuCNF(CNFFormula &formula, const std::vector<std::vector<int> > &puzzle);
