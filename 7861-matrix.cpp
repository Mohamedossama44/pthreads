#include <iostream>
#include <vector>
#include <pthread.h>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <fstream>

using namespace std;
using namespace chrono;

//struct feha 2 input w 1 output
struct ThreadArgs {
    const vector<vector<int>>& matrix_a;
    const vector<vector<int>>& matrix_b;
    vector<vector<int>>& result_matrix;
    int row;
    int col;
};

//create matrix with all elements=0
vector<vector<int>> createMatrix(size_t rows, size_t cols) {
    return vector<vector<int>>(rows, vector<int>(cols, 0));
}

void* compute_element(void* args) {
    ThreadArgs* thread_args = static_cast<ThreadArgs*>(args);//cast 'args' to be like struct ThreadArgs

    int value = 0;

    for (size_t k = 0; k < thread_args->matrix_b.size(); ++k) {
        value += thread_args->matrix_a[thread_args->row][k] * thread_args->matrix_b[k][thread_args->col];
    //[r][k]*[k]*[c]
    }

    thread_args->result_matrix[thread_args->row][thread_args->col] = value;

    pthread_exit(nullptr);
}

void matrix_multiply_parallel_element(const vector<vector<int>>& matrix_a,
                               const vector<vector<int>>& matrix_b,
                               vector<vector<int>>& result_matrix) {
    pthread_t threads[matrix_a.size() * matrix_b[0].size()];//array of pthread_t
    size_t thread_index = 0;

    for (size_t i = 0; i < matrix_a.size(); ++i) {//balf 3la rows matrix1
        for (size_t j = 0; j < matrix_b[0].size(); ++j) { //balf 3la column matrix2
            ThreadArgs* args = new ThreadArgs{matrix_a, matrix_b, result_matrix, static_cast<int>(i), static_cast<int>(j)};
            pthread_create(&threads[thread_index++], nullptr, compute_element, static_cast<void*>(args));
            //pthread create (thread name ,thread paramater ,function ,function paramter)
        }
    }

    for (size_t i = 0; i < thread_index; ++i) {
        pthread_join(threads[i], nullptr);// waits for each thread to finish
    }
}
void* compute_row(void* args) {
    ThreadArgs* thread_args = static_cast<ThreadArgs*>(args);//cast 'args' to be like struct ThreadArgs

    for (size_t j = 0; j < thread_args->matrix_b[0].size(); ++j) {
        int value = 0;
        for (size_t k = 0; k < thread_args->matrix_b.size(); ++k) {
            value += thread_args->matrix_a[thread_args->row][k] * thread_args->matrix_b[k][j];
        }
        thread_args->result_matrix[thread_args->row][j] = value;
    }

    pthread_exit(nullptr);
}
void matrix_multiply_parallel_row(const vector<vector<int>>& matrix_a,
                               const vector<vector<int>>& matrix_b,
                               vector<vector<int>>& result_matrix) {
    pthread_t threads[matrix_a.size()];// array of threads

    for (size_t i = 0; i < matrix_a.size(); ++i) {
        ThreadArgs* args = new ThreadArgs{matrix_a, matrix_b, result_matrix, static_cast<int>(i)};// i for row matrix1
        pthread_create(&threads[i], nullptr, compute_row, static_cast<void*>(args));
    }

    for (size_t i = 0; i < matrix_a.size(); ++i) {// waits for each thread to finish
        pthread_join(threads[i], nullptr);
    }
}
int main() {
    // Input filename
    string filename;
    cout << "Enter the filename: ";
    cin >> filename;

    // Open the file
    ifstream inputFile(filename);

    if (!inputFile.is_open()) {
        cerr << "Error opening file." << endl;
        return 1;
    }

    size_t rows_a, cols_a, rows_b, cols_b;

    // Read matrix A size and values
    inputFile >> rows_a >> cols_a;
    vector<vector<int>> matrix_a = createMatrix(rows_a, cols_a);

    for (size_t i = 0; i < rows_a; ++i) {
        for (size_t j = 0; j < cols_a; ++j) {
            inputFile >> matrix_a[i][j];
        }
    }

    // Read matrix B size and values
    inputFile >> rows_b >> cols_b;
    vector<vector<int>> matrix_b = createMatrix(rows_b, cols_b);

    for (size_t i = 0; i < rows_b; ++i) {
        for (size_t j = 0; j < cols_b; ++j) {
            inputFile >> matrix_b[i][j];
        }
    }

    inputFile.close();

    // Check if matrices can be multiplied
    if (cols_a != rows_b) {
        cerr << "Error:columns no. in matrix A must be equal rows no. in matrix B.\n";
        return 1;
    }

    // Measure the time taken for matrix multiplication (element wise)
    auto start_time = steady_clock::now();
    vector<vector<int>> result_matrix(rows_a, vector<int>(cols_b, 0));
    matrix_multiply_parallel_element(matrix_a, matrix_b, result_matrix);
    auto end_time = steady_clock::now();
    chrono::duration<double> duration = chrono::duration_cast<chrono::duration<double>>(end_time - start_time);

    // Print the result
    cout << "\nResult matrix (element wise):\n";
    for (const auto& row : result_matrix) {
        for (int element : row) {
            cout << element << " ";
        }
        cout << endl;
    }
    // Print the time taken
    cout << fixed << setprecision(6) << "\nTime taken (element wise): " << duration.count() << " Seconds." << endl;

    // Measure the time taken for matrix multiplication (row wise)
    auto start_time_2 = steady_clock::now();
    vector<vector<int>> result_matrix_2(rows_a, vector<int>(cols_b, 0));
    matrix_multiply_parallel_row(matrix_a, matrix_b, result_matrix_2);
    auto end_time_2 = steady_clock::now();
    chrono::duration<double> duration_2 = chrono::duration_cast<chrono::duration<double>>(end_time_2 - start_time_2);

    // Print the result
    cout << "\nResult matrix (row wise):\n";
    for (const auto& row : result_matrix_2) {
        for (int element : row) {
            cout << element << " ";
        }
        cout << endl;
    }

    // Print the time taken by multiplication (row wise)
    cout << fixed << setprecision(6) << "\nTime taken (row wise): " << duration_2.count() << " Seconds." << endl;

    return 0;
}
