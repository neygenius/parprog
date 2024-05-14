#include <iostream>
#include <vector>
#include <cstdlib> 
#include <sstream>
#include <fstream>
#include <ctime>
#include <chrono>
#include <iomanip>
#include "mpi.h"

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


int main(int argc, char* argv[]) {
    srand(time(nullptr));
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    std::vector<int> sizes = { 100, 250, 500, 750, 1000 };
    int iter = 5;
    std::ofstream time_file("avg_times.txt");
    for (int s : sizes) {
        auto total_duration = std::chrono::milliseconds(0);
        for (int k = 0; k < iter; ++k) {
            std::vector<std::vector<int>> matrixA, matrixB, result_mult;
            if (rank == 0) {
                matrixA = create_matrix_random(s);
                matrixB = create_matrix_random(s);
            }
            auto start = std::chrono::high_resolution_clock::now();
            MPI_Bcast(matrixA.data(), s * s, MPI_INT, 0, MPI_COMM_WORLD);
            MPI_Bcast(matrixB.data(), s * s, MPI_INT, 0, MPI_COMM_WORLD);
            result_mult = matrix_mult(matrixA, matrixB, s);
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
            total_duration += duration;
            if (k == 0 && rank == 0) {
                std::stringstream fstream_A;
                fstream_A << "matrix_A_" << s << ".txt";
                std::string file_A = fstream_A.str();
                std::stringstream fstream_B;
                fstream_B << "matrix_B_" << s << ".txt";
                std::string file_B = fstream_B.str();
                std::stringstream fstream_res;
                fstream_res << "res_AxB_" << s << ".txt";
                std::string file_res = fstream_res.str();
                write_matrix_file(result_mult, file_res);
                write_matrix_file(matrixA, file_A);
                write_matrix_file(matrixB, file_B);
            }
        }
        if (rank == 0) {
            auto average_duration = total_duration / iter;
            time_file << s << " - " << average_duration.count() << std::endl;
        }
    }
    time_file.close();
    MPI_Finalize();
    return 0;
}