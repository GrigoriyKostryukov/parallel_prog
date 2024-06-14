#include <iostream>
#include <stdlib.h>
#include <omp.h>
#include <stdio.h>
#include <windows.h>
#include <string>

#define MATRIX_SIZE 4096

using namespace std;

void transpose(double* A, double* B, int n) {
    int i, j;
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            B[j * n + i] = A[i * n + j];
        }
    }
}

void multiply_omp(double* A, double* B, double* C, int n)
{
    double* B2;
    B2 = (double*)malloc(sizeof(double) * n * n);
    transpose(B, B2, n);
#pragma omp parallel
    {
        int i, j, k;
#pragma omp for
        for (i = 0; i < n; i++) {
            for (j = 0; j < n; j++) {
                double dot = 0;
                for (k = 0; k < n; k++) {
                    dot += A[i * n + k] * B2[j * n + k];
                }
                C[i * n + j] = dot;
            }
        }

    }
    free(B2);
}

int main() {
    int i, size, thread_number, max_thread_number, value;
    string input;
    double* A, * B, * C, run_time;

    size = MATRIX_SIZE;
    A = (double*)malloc(sizeof(double) * size * size);
    B = (double*)malloc(sizeof(double) * size * size);
    C = (double*)malloc(sizeof(double) * size * size);
    for (i = 0; i < size * size; i++) {
        A[i] = 1.0 * rand() / RAND_MAX;
        B[i] = rand() / RAND_MAX; 
    }

    max_thread_number = omp_get_max_threads();

    while (true) {
        cout << max_thread_number << " threads available" << endl;
        cout << "Enter the number of threads: ";
        try {
            cin >> input;
            if (input == "exit") {
                return 0;
            }

            value = stoi(input);

            if (value < 0 || value > max_thread_number) {
                cin.clear();
                cin.ignore(cin.rdbuf()->in_avail());
                cout << "Inorrect value. Try again" << endl;
                continue;

            }
            omp_set_dynamic(0);
            omp_set_num_threads(value);
            break;

        }
        catch (invalid_argument) {
            cin.clear();
            cin.ignore(cin.rdbuf()->in_avail());
            cout << "Inorrect value. Try again" << endl;
        }
    }
    run_time = omp_get_wtime();
    multiply_omp(A, B, C, size);
    run_time = omp_get_wtime() - run_time;
    cout << "Running time: " << run_time << " s" << endl;
    return 0;

}