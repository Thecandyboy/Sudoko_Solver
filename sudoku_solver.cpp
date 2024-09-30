// sudoku_solver.cpp
#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_set>  // Include unordered_set instead of set
#include "dpll.h"

using namespace std;

// No need to redefine CNF, it is already defined in dpll.h

// Function to encode Sudoku puzzle into CNF constraints
CNF encodeSudoku(int grid[9][9]) {
    CNF cnf;

    // Encode cell constraints (if a cell has a number, it must be that number)
    for (int r = 0; r < 9; ++r) {
        for (int c = 0; c < 9; ++c) {
            if (grid[r][c] != 0) {
                int var = r * 81 + c * 9 + (grid[r][c] - 1) + 1; // variable index
                cnf.push_back({var}); // This cell must be this number
            }
        }
    }

    // Encode row constraints (each number must appear exactly once in each row)
    for (int r = 0; r < 9; ++r) {
        for (int num = 1; num <= 9; ++num) {
            Clause clause;
            for (int c = 0; c < 9; ++c) {
                int var = r * 81 + c * 9 + (num - 1) + 1; // variable index
                clause.insert(var);
            }
            cnf.push_back(clause); // At least one of these must be true
        }

        // At most one of these can be true
        for (int num = 1; num <= 9; ++num) {
            for (int c1 = 0; c1 < 9; ++c1) {
                for (int c2 = c1 + 1; c2 < 9; ++c2) {
                    int var1 = r * 81 + c1 * 9 + (num - 1) + 1;
                    int var2 = r * 81 + c2 * 9 + (num - 1) + 1;
                    cnf.push_back({-var1, -var2}); // At most one can be true
                }
            }
        }
    }

    // Encode column constraints (similar to rows)
    for (int c = 0; c < 9; ++c) {
        for (int num = 1; num <= 9; ++num) {
            Clause clause;
            for (int r = 0; r < 9; ++r) {
                int var = r * 81 + c * 9 + (num - 1) + 1;
                clause.insert(var);
            }
            cnf.push_back(clause);
        }

        for (int num = 1; num <= 9; ++num) {
            for (int r1 = 0; r1 < 9; ++r1) {
                for (int r2 = r1 + 1; r2 < 9; ++r2) {
                    int var1 = r1 * 81 + c * 9 + (num - 1) + 1;
                    int var2 = r2 * 81 + c * 9 + (num - 1) + 1;
                    cnf.push_back({-var1, -var2});
                }
            }
        }
    }

    // Encode 3x3 subgrid constraints
    for (int boxRow = 0; boxRow < 3; ++boxRow) {
        for (int boxCol = 0; boxCol < 3; ++boxCol) {
            for (int num = 1; num <= 9; ++num) {
                Clause clause;
                for (int r = 0; r < 3; ++r) {
                    for (int c = 0; c < 3; ++c) {
                        int rowIndex = boxRow * 3 + r;
                        int colIndex = boxCol * 3 + c;
                        int var = rowIndex * 81 + colIndex * 9 + (num - 1) + 1;
                        clause.insert(var);
                    }
                }
                cnf.push_back(clause);
            }

            for (int num = 1; num <= 9; ++num) {
                for (int r1 = 0; r1 < 3; ++r1) {
                    for (int c1 = 0; c1 < 3; ++c1) {
                        for (int r2 = r1; r2 < 3; ++r2) {
                            for (int c2 = (r2 == r1) ? c1 + 1 : 0; c2 < 3; ++c2) {
                                int var1 = (boxRow * 3 + r1) * 81 + (boxCol * 3 + c1) * 9 + (num - 1) + 1;
                                int var2 = (boxRow * 3 + r2) * 81 + (boxCol * 3 + c2) * 9 + (num - 1) + 1;
                                cnf.push_back({-var1, -var2});
                            }
                        }
                    }
                }
            }
        }
    }

    return cnf;
}
// Function to decode the solution back into a Sudoku grid
void decodeSolution(const vector<int> &solution, int grid[9][9]) {
    for (int literal : solution) {
        if (literal > 0) {
            int var = literal - 1; // Get variable index
            int num = (var % 9) + 1; // Get the Sudoku number (1 to 9)
            int c = (var / 9) % 9; // Column index
            int r = var / 81; // Row index
            grid[r][c] = num; // Place number in the grid
        }
    }
}

// Function to read Sudoku grid from the file "testcases.txt"
bool readSudokuFromFile(int grid[9][9], ifstream &infile) {
    for (int r = 0; r < 9; ++r) {
        for (int c = 0; c < 9; ++c) {
            if (!(infile >> grid[r][c])) {
                return false;  // End of file or invalid format
            }
        }
    }
    return true;
}

// Function to print Sudoku grid to output file "output.txt"
void writeSudokuToFile(int grid[9][9], ofstream &outfile) {
    for (int r = 0; r < 9; ++r) {
        for (int c = 0; c < 9; ++c) {
            outfile << grid[r][c] << " ";
        }
        outfile << endl;
    }
    outfile << endl;  // Separate test cases with a blank line
}

int main() {
    int grid[9][9] = {0};  // Initialize Sudoku grid

    // Open the test case file and output file
    ifstream infile("testcases.txt");
    ofstream outfile("output.txt");

    if (!infile.is_open()) {
        cout << "Error opening testcases file." << endl;
        return 1;
    }
    if (!outfile.is_open()) {
        cout << "Error opening output file." << endl;
        return 1;
    }

    // Loop through all the test cases in the file
    while (readSudokuFromFile(grid, infile)) {
        // Encode the Sudoku puzzle into CNF
        CNF cnf = encodeSudoku(grid);

        // Solve using DPLL
        if (dpll(cnf)) {
            vector<int> solution;  // Placeholder for solution
            decodeSolution(solution, grid);  // Decode solution
            writeSudokuToFile(grid, outfile);  // Write solution to file
        } else {
            outfile << "No solution exists." << endl;
        }
    }

    infile.close();
    outfile.close();

    cout << "Sudoku solving completed. Check 'output.txt' for results." << endl;
    return 0;
}
