#include "Sudoku_solver.h"
#include <vector>
#include <iostream>

// Function to encode the Sudoku puzzle into CNF
void encodeSudokuCNF(CNFFormula &formula, const std::vector<std::vector<int> > &puzzle) {
    int N = 9; // Size of the Sudoku grid

    // Rule 1: Each cell contains at least one number
    for (int r = 1; r <= N; ++r) {
        for (int c = 1; c <= N; ++c) {
            Clause clause;
            for (int v = 1; v <= N; ++v) {
                clause.push_back(r * 100 + c * 10 + v); // Encodes x_{r,c,v}
            }
            formula.push_back(clause);
        }
    }

    // Rule 2: Each cell contains at most one number
    for (int r = 1; r <= N; ++r) {
        for (int c = 1; c <= N; ++c) {
            for (int v1 = 1; v1 <= N; ++v1) {
                for (int v2 = v1 + 1; v2 <= N; ++v2) {
                    Clause clause;
                    clause.push_back(-(r * 100 + c * 10 + v1));
                    clause.push_back(-(r * 100 + c * 10 + v2));
                    formula.push_back(clause);
                }
            }
        }
    }

    // Rule 3: Each number appears exactly once per row
    for (int v = 1; v <= N; ++v) {
        for (int r = 1; r <= N; ++r) {
            for (int c1 = 1; c1 <= N; ++c1) {
                for (int c2 = c1 + 1; c2 <= N; ++c2) {
                    Clause clause;
                    clause.push_back(-(r * 100 + c1 * 10 + v));
                    clause.push_back(-(r * 100 + c2 * 10 + v));
                    formula.push_back(clause);
                }
            }
        }
    }

    // Rule 4: Each number appears exactly once per column
    for (int v = 1; v <= N; ++v) {
        for (int c = 1; c <= N; ++c) {
            for (int r1 = 1; r1 <= N; ++r1) {
                for (int r2 = r1 + 1; r2 <= N; ++r2) {
                    Clause clause;
                    clause.push_back(-(r1 * 100 + c * 10 + v));
                    clause.push_back(-(r2 * 100 + c * 10 + v));
                    formula.push_back(clause);
                }
            }
        }
    }

    // Rule 5: Each number appears exactly once per 3x3 block
    for (int v = 1; v <= N; ++v) {
        for (int br = 0; br < 3; ++br) {
            for (int bc = 0; bc < 3; ++bc) {
                for (int r1 = 1; r1 <= 3; ++r1) {
                    for (int c1 = 1; c1 <= 3; ++c1) {
                        for (int r2 = r1; r2 <= 3; ++r2) {
                            for (int c2 = (r1 == r2 ? c1 + 1 : 1); c2 <= 3; ++c2) {
                                Clause clause;
                                clause.push_back(-( (3 * br + r1) * 100 + (3 * bc + c1) * 10 + v ));
                                clause.push_back(-( (3 * br + r2) * 100 + (3 * bc + c2) * 10 + v ));
                                formula.push_back(clause);
                            }
                        }
                    }
                }
            }
        }
    }

    // Rule 6: Initial clues from the puzzle
    for (int r = 0; r < N; ++r) {
        for (int c = 0; c < N; ++c) {
            if (puzzle[r][c] != 0) {
                Clause clause;
                clause.push_back((r + 1) * 100 + (c + 1) * 10 + puzzle[r][c]);
                formula.push_back(clause);
            }
        }
    }
}

// int main() {
//     CNFFormula formula;
//     std::vector<std::vector<int> > puzzle(9, std::vector<int>(9, 0)); // Example empty puzzle

//     try {
//         encodeSudokuCNF(formula, puzzle);
//         std::cout << "Sudoku encoded successfully." << std::endl;
//     } catch (const std::exception &e) {
//         std::cerr << "Exception: " << e.what() << std::endl;
//     }

//     return 0;
// }
