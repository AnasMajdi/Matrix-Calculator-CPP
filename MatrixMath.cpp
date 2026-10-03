/**
 * =========================================================
 * @author      Anas Majdi 
 * @attention   Supervisor      : Dr. Ammar Issa
 * =========================================================
 */

#include "MatrixMath.h"
#include <cmath>
#include <algorithm>  // For std::swap
#include <sstream>    // For std::stringstream used in parsing
#include <iomanip>    // For string formatting

namespace LinearAlgebra {

    // ==========================================
    // 1. Basic Operations
    // ==========================================
    
    // Add two matrices
    Matrix AddMatrices(const Matrix& A, const Matrix& B) {
        if (A.empty() || B.empty() || A.size() != B.size() || A[0].size() != B[0].size()) {
            throw std::invalid_argument("Error: Matrices must have the same dimensions to add.");
        }
        int rows = A.size();
        int cols = A[0].size();
        Matrix result(rows, std::vector<double>(cols, 0.0));
        
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                result[i][j] = A[i][j] + B[i][j];
            }
        }
        return result;
    }

    // Subtract two matrices
    Matrix SubtractMatrices(const Matrix& A, const Matrix& B) {
        if (A.empty() || B.empty() || A.size() != B.size() || A[0].size() != B[0].size()) {
            throw std::invalid_argument("Error: Matrices must have the same dimensions to subtract.");
        }
        int rows = A.size();
        int cols = A[0].size();
        Matrix result(rows, std::vector<double>(cols, 0.0));
        
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                result[i][j] = A[i][j] - B[i][j];
            }
        }
        return result;
    }

    // Multiply two matrices
    Matrix MultiplyMatrices(const Matrix& A, const Matrix& B) {
        if (A.empty() || B.empty() || A[0].size() != B.size()) {
            throw std::invalid_argument("Error: Number of columns in Matrix A must equal the number of rows in Matrix B.");
        }
        int rowsA = A.size();
        int colsA = A[0].size(); 
        int colsB = B[0].size();
        Matrix result(rowsA, std::vector<double>(colsB, 0.0));
        
        for (int i = 0; i < rowsA; ++i) {
            for (int j = 0; j < colsB; ++j) {
                for (int k = 0; k < colsA; ++k) {
                    result[i][j] += A[i][k] * B[k][j];
                }
            }
        }
        return result;
    }

    // ==========================================
    // 2. Matrix Properties
    // ==========================================

    // Get the transpose of a matrix
    Matrix Transpose(const Matrix& A) {
        if (A.empty()) return {};
        int rows = A.size();
        int cols = A[0].size();
        Matrix result(cols, std::vector<double>(rows, 0.0)); 
        
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                result[j][i] = A[i][j];
            }
        }
        return result;
    }

    // Calculate the trace of a square matrix
    double Trace(const Matrix& A) {
        if (A.empty() || A.size() != A[0].size()) {
            throw std::invalid_argument("Error: Trace can only be calculated for square matrices.");
        }
        double sum = 0.0;
        int n = A.size();
        for (int i = 0; i < n; ++i) {
            sum += A[i][i];
        }
        return sum;
    }

    // Helper function to get sub-matrix (used in Determinant & Inverse)
    Matrix GetSubMatrix(const Matrix& A, int excludeRow, int excludeCol) {
        int n = A.size();
        Matrix sub(n - 1, std::vector<double>(n - 1));
        int r = 0;
        for (int i = 0; i < n; ++i) {
            if (i == excludeRow) continue;
            int c = 0;
            for (int j = 0; j < n; ++j) {
                if (j == excludeCol) continue;
                sub[r][c] = A[i][j];
                c++;
            }
            r++;
        }
        return sub;
    }

    // Calculate the determinant of a square matrix
    double Determinant(const Matrix& A) {
        if (A.empty() || A.size() != A[0].size()) {
            throw std::invalid_argument("Error: Determinant can only be calculated for square matrices.");
        }
        int n = A.size();
        if (n == 1) return A[0][0];
        if (n == 2) {
            return (A[0][0] * A[1][1]) - (A[0][1] * A[1][0]);
        }
        
        double det = 0.0;
        int sign = 1;
        for (int j = 0; j < n; ++j) {
            Matrix sub = GetSubMatrix(A, 0, j); 
            det += sign * A[0][j] * Determinant(sub); 
            sign = -sign;
        }
        return det;
    }

    // Calculate the inverse of a square matrix
    Matrix Inverse(const Matrix& A) {
        if (A.empty() || A.size() != A[0].size()) {
            throw std::invalid_argument("Error: Inverse can only be calculated for square matrices.");
        }
        
        if (Determinant(A) == 0) {
            throw std::runtime_error("Error: Matrix is singular (Determinant is 0). Inverse does not exist.");
        }
        
        int n = A.size();
        Matrix result(n, std::vector<double>(n));
        
        if (n == 1) {
            result[0][0] = 1.0 / A[0][0];
            return result;
        }
        
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                Matrix sub = GetSubMatrix(A, i, j);
                double cofactorDet = Determinant(sub);
                int sign = ((i + j) % 2 == 0) ? 1 : -1;
                result[j][i] = (sign * cofactorDet) / Determinant(A);
            }
        }
        return result;
    }

    // Check if a matrix is symmetric
    bool IsSymmetric(const Matrix& A) {
        if (A.empty() || A.size() != A[0].size()) {
            return false;
        }
        int n = A.size();
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                if (A[i][j] != A[j][i]) {
                    return false;
                }
            }
        }
        return true;
    }

    // ==========================================
    // 3. Solving Linear Systems
    // ==========================================

    // Solve system of linear equations using Cramer's Rule
    std::vector<double> CramerRule(const Matrix& A, const std::vector<double>& B) {
        if (A.empty() || A.size() != A[0].size()) {
            throw std::invalid_argument("Error: Coefficient matrix must be square for Cramer's Rule.");
        }
        if (A.size() != B.size()) {
            throw std::invalid_argument("Error: Dimensions mismatch between matrix A and vector B.");
        }
        if (Determinant(A) == 0) {
            throw std::runtime_error("Error: Determinant is 0. System has no unique solution.");
        }
        
        int n = A.size();
        std::vector<double> solution(n);
        
        for (int i = 0; i < n; ++i) {
            Matrix Ai = A; 
            for (int j = 0; j < n; ++j) {
                Ai[j][i] = B[j]; 
            }
            solution[i] = Determinant(Ai) / Determinant(A);
        }
        return solution;
    }

    // Solve system of linear equations using Gauss-Jordan Elimination
    std::vector<double> GaussJordan(Matrix A, std::vector<double> B) {
        if (A.empty() || A.size() != A[0].size()) {
            throw std::invalid_argument("Error: Coefficient matrix must be square.");
        }
        if (A.size() != B.size()) {
            throw std::invalid_argument("Error: Dimensions mismatch.");
        }
        
        int n = A.size();
        for (int i = 0; i < n; ++i) {
            int maxRow = i;
            for (int k = i + 1; k < n; ++k) {
                if (std::abs(A[k][i]) > std::abs(A[maxRow][i])) {
                    maxRow = k;
                }
            }
            
            if (maxRow != i) {
                std::swap(A[i], A[maxRow]);
                std::swap(B[i], B[maxRow]);
            }
            
            if (A[i][i] == 0) {
                throw std::runtime_error("Error: Matrix is singular. System has no unique solution.");
            }
            
            double pivot = A[i][i];
            for (int j = i; j < n; ++j) {
                A[i][j] /= pivot;
            }
            B[i] /= pivot;
            
            for (int k = 0; k < n; ++k) {
                if (k != i) {
                    double factor = A[k][i];
                    for (int j = i; j < n; ++j) {
                        A[k][j] -= factor * A[i][j];
                    }
                    B[k] -= factor * B[i];
                }
            }
        }
        return B;
    }

    // ==========================================
    // 4. Utility & Parsing Functions
    // ==========================================

    // Convert multi-line string input into a Matrix object
    Matrix ParseMatrix(const std::string& text) {
        Matrix m; 
        std::stringstream ss(text); 
        std::string line;
        
        while (std::getline(ss, line)) { 
            if (line.empty()) continue;
            
            std::vector<double> row; 
            std::stringstream lineStream(line); 
            double val;
            
            while (lineStream >> val) {
                row.push_back(val);
            }
            
            if (!row.empty()) {
                m.push_back(row); 
            }
        }
        return m;
    }

    // Convert single-line string input into a vector
    std::vector<double> ParseVector(const std::string& text) {
        std::vector<double> v; 
        std::stringstream ss(text); 
        double val;
        
        while (ss >> val) {
            v.push_back(val);
        }
        return v;
    }

    // Convert a Matrix object to a formatted string for UI display
    std::string MatrixToString(const Matrix& m) {
        std::string res = "";
        for (const auto& row : m) {
            for (double val : row) {
                // Remove trailing zeros for integers (e.g., 5.000 -> 5)
                std::stringstream ss;
                ss << val; 
                res += ss.str() + "   ";
            }
            res += "\r\n";
        }
        return res;
    }

    // Convert a vector object to a formatted string for UI display
    std::string VectorToString(const std::vector<double>& v) {
        std::string res = "";
        for (double val : v) {
            std::stringstream ss;
            ss << val;
            res += ss.str() + "\r\n";
        }
        return res;
    }

} // End of LinearAlgebra namespace
