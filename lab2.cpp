#include <iostream>
#include <vector>
#include <cstdlib> 
#include <sstream>
#include <fstream>
#include <ctime>
#include <chrono>
#include <iomanip>
#include <omp.h>

std::vector<std::vector<int>> create_matrix_random(const int size) {
    std::vector<std::vector<int>> matrix(size, std::vector<int>(size));
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            matrix[i][j] = rand() % 100;
        }
    }
    return matrix;
}

std::vector<std::vector<int>> matrix_mult(const std::vector<std::vector<int>>& matrixA, const std::vector<std::vector<int>>& matrixB, const int size) {
    std::vector<std::vector<int>> result(size, std::vector<int>(size, 0));
    omp_set_num_threads(2);
#pragma omp parallel for num_threads(2)
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            for (int k = 0; k < size; ++k) {
                result[i][j] += matrixA[i][k] * matrixB[k][j];
            }
        }
    }
    return result;
}

void write_matrix_file(const std::vector<std::vector<int>>& matrix, const std::string filename) {
    std::ofstream output(filename);
    if (!output.is_open()) {
        std::cerr << "Failed to open file!" << std::endl;
        return;
    }
    for (const auto& row : matrix) {
        for (int value : row) {
            output << value << " ";
        }
        output << std::endl;
    }
    output.close();
    return;
}


int main() {
    srand(time(nullptr));
    std::vector<int> sizes = { 100, 250, 500, 750, 1000 };
    int iter = 5;
    std::ofstream time_file("avg_times.txt");
    for (int size : sizes) {
        auto total_duration = std::chrono::milliseconds(0);
        for (int k = 0; k < iter; ++k) {
            std::vector<std::vector<int>> matrixA = create_matrix_random(size);
            std::vector<std::vector<int>> matrixB = create_matrix_random(size);
            auto start = std::chrono::high_resolution_clock::now();
            std::vector<std::vector<int>> result_mult = matrix_mult(matrixA, matrixB, size);
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
            total_duration += duration;
            if (k == 0) {
                std::stringstream fstream_A;
                fstream_A << "matrix_A_" << size << ".txt";
                std::string file_A = fstream_A.str();
                std::stringstream fstream_B;
                fstream_B << "matrix_B_" << size << ".txt";
                std::string file_B = fstream_B.str();
                std::stringstream fstream_res;
                fstream_res << "res_AxB_" << size << ".txt";
                std::string file_res = fstream_res.str();
                write_matrix_file(result_mult, file_res);
                write_matrix_file(matrixA, file_A);
                write_matrix_file(matrixB, file_B);
            }
        }
        auto average_duration = total_duration / iter;
        time_file << size << " - " << average_duration.count() << std::endl;
    }
    time_file.close();
    return 0;
}
