#include <iostream>      // for cout, cin
#include <vector>        // for std::vector to store matrices
#include <fstream>       // for ifstream to read input file
#include <iomanip>       // for setw to align columns
#include <limits>        // for numeric_limits when clearing input
using namespace std;

// Define a simple Matrix type using vectors of vectors (integers).
using Matrix = vector<vector<int>>;

// Read N and then two N×N matrices from a file.
// Returns true on success; false on failure.
bool loadFromFile(const string& filename, Matrix& A, Matrix& B, int& N) {
    ifstream fin(filename);                         // open the file for reading
    if (!fin) {                                     // check if the file failed to open
        cerr << "Error: cannot open file: " << filename << "\n";  // print error message
        return false;                               // signal failure
    }
    if (!(fin >> N) || N <= 0) {                    // read N and check it’s a positive integer
        cerr << "Error: first value must be a positive integer N.\n"; // error if invalid
        return false;                               // signal failure
    }

    A.assign(N, vector<int>(N, 0));                 // size A to N×N and fill with 0
    B.assign(N, vector<int>(N, 0));                 // size B to N×N and fill with 0

    // Read matrix A values in row-major order
    for (int i = 0; i < N; ++i) {                   // loop over rows
        for (int j = 0; j < N; ++j) {               // loop over columns
            if (!(fin >> A[i][j])) {                // attempt to read next integer
                cerr << "Error: not enough numbers for matrix A.\n"; // error if missing
                return false;                       // signal failure
            }
        }
    }
    // Read matrix B values in row-major order
    for (int i = 0; i < N; ++i) {                   // loop over rows
        for (int j = 0; j < N; ++j) {               // loop over columns
            if (!(fin >> B[i][j])) {                // attempt to read next integer
                cerr << "Error: not enough numbers for matrix B.\n"; // error if missing
                return false;                       // signal failure
            }
        }
    }
    return true;                                    // file successfully loaded
}

// Print a matrix with a label and simple alignment.
void printMatrix(const Matrix& M, const string& label) {
    cout << label << " (" << M.size()               // print label and matrix size
         << "x" << (M.empty() ? 0 : M[0].size()) << "):\n";
    for (int i = 0; i < (int)M.size(); ++i) {       // for each row
        for (int j = 0; j < (int)M[i].size(); ++j) {// for each column
            cout << setw(4) << M[i][j];             // print with width 4 for neat columns
        }
        cout << "\n";                               // newline after each row
    }
}

// Add two N×N matrices element-wise and return the result.
Matrix add(const Matrix& A, const Matrix& B) {
    int N = (int)A.size();                          // matrix dimension
    Matrix C(N, vector<int>(N, 0));                 // result matrix sized N×N, init to 0
    for (int i = 0; i < N; ++i) {                   // rows
        for (int j = 0; j < N; ++j) {               // cols
            C[i][j] = A[i][j] + B[i][j];            // element-wise addition
        }
    }
    return C;                                       // return sum matrix
}

// Multiply two N×N matrices using the standard triple-loop algorithm.
Matrix multiply(const Matrix& A, const Matrix& B) {
    int N = (int)A.size();                          // matrix dimension
    Matrix C(N, vector<int>(N, 0));                 // result matrix sized N×N, init to 0
    for (int i = 0; i < N; ++i) {                   // row of A
        for (int k = 0; k < N; ++k) {               // column/row index (A’s column, B’s row)
            for (int j = 0; j < N; ++j) {           // column of B
                C[i][j] += A[i][k] * B[k][j];       // accumulate A[i][k] * B[k][j]
            }
        }
    }
    return C;                                       // return product matrix
}

// Compute main and secondary diagonal sums for an N×N matrix.
pair<int,int> diagonalSums(const Matrix& M) {
    int N = (int)M.size();                          // matrix dimension
    int mainSum = 0;                                // sum for main diagonal (i == j)
    int secondary = 0;                              // sum for secondary diagonal (i + j == N - 1)
    for (int i = 0; i < N; ++i) {                   // iterate over rows/cols
        mainSum += M[i][i];                         // add main diagonal element
        secondary += M[i][N - 1 - i];               // add secondary diagonal element
    }
    return {mainSum, secondary};                    // return both sums as a pair
}

// Swap two rows in a matrix if the indices are within bounds.
// Returns true if swapped; false if indices were invalid.
bool swapRows(Matrix& M, int r1, int r2) {
    int N = (int)M.size();                          // number of rows
    if (r1 < 0 || r2 < 0 || r1 >= N || r2 >= N)     // check bounds
        return false;                               // invalid indices → no swap
    swap(M[r1], M[r2]);                             // swap whole row vectors
    return true;                                    // success
}

// Swap two columns in a matrix if the indices are within bounds.
// Returns true if swapped; false if indices were invalid.
bool swapCols(Matrix& M, int c1, int c2) {
    int N = (int)M.size();                          // number of rows (and columns since N×N)
    if (c1 < 0 || c2 < 0 || c1 >= N || c2 >= N)     // check bounds
        return false;                               // invalid indices → no swap
    for (int i = 0; i < N; ++i) {                   // go down each row
        swap(M[i][c1], M[i][c2]);                   // swap the two column entries
    }
    return true;                                    // success
}

// Update a single cell (r, c) to value `val` if indices are valid.
// Returns true if updated; false if indices were invalid.
bool updateCell(Matrix& M, int r, int c, int val) {
    int N = (int)M.size();                          // dimension
    if (r < 0 || c < 0 || r >= N || c >= N)         // check bounds
        return false;                               // invalid indices → no update
    M[r][c] = val;                                  // set the new value
    return true;                                    // success
}

int main() {
    // NOTE: We intentionally do NOT disable iostream sync or untie cin/cout,
    // so prompts display immediately without needing manual flushes.

    cout << "Lab 9 - C++ Programming: Matrix Operations\n"; // introductory line
    cout << "Enter input filename: ";                        // prompt for file name

    string filename;                                        // stores the user-entered filename
    if (!(cin >> filename)) {                                // read filename; check input success
        cerr << "Error: failed to read filename.\n";         // print error if input fails
        return 1;                                            // exit with error code
    }

    Matrix A, B;                                             // two matrices we will load
    int N = 0;                                               // dimension of the matrices (N×N)
    if (!loadFromFile(filename, A, B, N)) {                  // attempt to load from file
        return 1;                                            // if load fails, exit (error already printed)
    }

    // 1) Print matrices
    printMatrix(A, "Matrix A");                              // show matrix A
    printMatrix(B, "Matrix B");                              // show matrix B

    // 2) A + B
    cout << "\nA + B:\n";                                    // header for addition
    Matrix Cadd = add(A, B);                                 // compute element-wise sum
    printMatrix(Cadd, "Sum");                                // print the sum matrix

    // 3) A * B
    cout << "\nA * B:\n";                                    // header for multiplication
    Matrix Cmul = multiply(A, B);                            // compute matrix product
    printMatrix(Cmul, "Product");                            // print the product matrix

    // 4) Diagonal sums for A and B
    auto sumsA = diagonalSums(A);                            // get (main, secondary) for A
    auto sumsB = diagonalSums(B);                            // get (main, secondary) for B
    cout << "\nDiagonal sums:\n";                            // header for diagonal sums section
    cout << "Matrix A -> main: " << sumsA.first              // print A’s main diag sum
         << ", secondary: " << sumsA.second << "\n";         // print A’s secondary diag sum
    cout << "Matrix B -> main: " << sumsB.first              // print B’s main diag sum
         << ", secondary: " << sumsB.second << "\n";         // print B’s secondary diag sum

    // 5) Swap rows once (choose matrix, input indices)
    cout << "\nRow swap (0-based). Choose matrix (0 for A, 1 for B): "; // ask which matrix
    int which = 0;                                           // will store 0 or 1
    cin >> which;                                            // read user selection
    Matrix* M = (which == 1 ? &B : &A);                      // pointer to chosen matrix
    char label = (which == 1 ? 'B' : 'A');                   // label char for printing

    // Make the prompt explicit about format and examples (0-based, space-separated).
    cout << "Enter two row indices to swap (0-based, separated by a space, e.g., '1 3'): ";
    int r1, r2;                                              // row indices to swap
    if (cin >> r1 >> r2) {                                   // read both indices
        if (swapRows(*M, r1, r2)) {                          // attempt to swap rows
            printMatrix(*M, string("Matrix ") + label + " after row swap"); // print result
        } else {
            cout << "Invalid row indices. No changes made.\n"; // bounds check failed
        }
    } else {                                                  // if user typed something invalid
        cout << "Invalid input; skipping row swap.\n";        // inform and recover input stream
        cin.clear();                                          // clear error flags
        cin.ignore(numeric_limits<streamsize>::max(), '\n');  // discard bad input
    }

    // 6) Swap columns once
    cout << "\nColumn swap (0-based). Choose matrix (0 for A, 1 for B): "; // choose matrix again
    which = 0;                                               // reset selection variable
    cin >> which;                                            // read user selection
    M = (which == 1 ? &B : &A);                              // pointer to chosen matrix
    label = (which == 1 ? 'B' : 'A');                        // update label

    cout << "Enter two column indices to swap (0-based, separated by a space, e.g., '0 2'): ";
    int c1, c2;                                              // column indices to swap
    if (cin >> c1 >> c2) {                                   // read both indices
        if (swapCols(*M, c1, c2)) {                          // attempt to swap columns
            printMatrix(*M, string("Matrix ") + label + " after column swap"); // print result
        } else {
            cout << "Invalid column indices. No changes made.\n"; // bounds check failed
        }
    } else {                                                  // if user typed something invalid
        cout << "Invalid input; skipping column swap.\n";     // inform and recover input stream
        cin.clear();                                          // clear error flags
        cin.ignore(numeric_limits<streamsize>::max(), '\n');  // discard bad input
    }

    // 7) Update one cell
    cout << "\nUpdate one element. Choose matrix (0 for A, 1 for B): "; // choose matrix one more time
    which = 0;                                               // reset selection variable
    cin >> which;                                            // read user selection
    M = (which == 1 ? &B : &A);                              // pointer to chosen matrix
    label = (which == 1 ? 'B' : 'A');                        // update label

    cout << "Enter the row, column, and new value (0-based, separated by spaces, e.g., '2 1 99'): ";
    int ur, uc, val;                                         // row, col, and new value
    if (cin >> ur >> uc >> val) {                            // read values
        if (updateCell(*M, ur, uc, val)) {                   // attempt the update
            printMatrix(*M, string("Matrix ") + label + " after update"); // print result
        } else {
            cout << "Invalid indices. No changes made.\n";   // bounds check failed
        }
    } else {                                                  // invalid input typed
        cout << "Invalid input; skipping update.\n";          // inform and recover input stream
        cin.clear();                                          // clear error flags
        cin.ignore(numeric_limits<streamsize>::max(), '\n');  // discard bad input
    }

    cout << "\nDone. Goodbye!\n";                            // friendly exit message
    return 0;                                                // normal termination
}
