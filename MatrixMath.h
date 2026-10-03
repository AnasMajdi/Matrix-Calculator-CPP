/**
 * =========================================================
 * @author      Anas Majdi 
 * @attention   Supervisor      : Dr. Ammar Issa
 * =========================================================
 */

#pragma once

#include <vector>
#include <string>
#include <stdexcept>

namespace LinearAlgebra {

    using Matrix = std::vector<std::vector<double>>;

    // ==========================================
    // 1. Basic Operations
    // ==========================================
    Matrix AddMatrices(const Matrix& A, const Matrix& B);
    Matrix SubtractMatrices(const Matrix& A, const Matrix& B);
    Matrix MultiplyMatrices(const Matrix& A, const Matrix& B);

    // ==========================================
    // 2. Matrix Properties
    // ==========================================
    Matrix Transpose(const Matrix& A);
    double Trace(const Matrix& A);
    double Determinant(const Matrix& A);
    Matrix Inverse(const Matrix& A);
    bool IsSymmetric(const Matrix& A); // Check if the matrix is symmetric

    // ==========================================
    // 3. Solving Linear Systems
    // ==========================================
    std::vector<double> CramerRule(const Matrix& A, const std::vector<double>& B);
    std::vector<double> GaussJordan(Matrix A, std::vector<double> B);

    // ==========================================
    // 4. Utility & Parsing Functions
    // ==========================================
    Matrix ParseMatrix(const std::string& text);
    std::vector<double> ParseVector(const std::string& text);
    std::string MatrixToString(const Matrix& m);
    std::string VectorToString(const std::vector<double>& v);

} // End of LinearAlgebra namespace