#include <stdio.h>
#include <iostream>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include "mpi.h"
#include <windows.h>
#include <string>

using namespace std;
#define SIZE 2048

MPI_Status status;

int A[SIZE][SIZE];
int B[SIZE][SIZE];
int res_matrix[SIZE][SIZE];

int i, j, k;

int main(int argc, char** argv)
{
	int max_proc_number, proc_rank, slaves_number;
	string input;
	int source_proc, dest_proc, rows, processed_rows;


	MPI_Init(&argc, &argv);

	MPI_Comm_size(MPI_COMM_WORLD, &max_proc_number);

	MPI_Comm_rank(MPI_COMM_WORLD, &proc_rank);

	slaves_number = max_proc_number - 1;

	// Главный процессор-координатор
	if (proc_rank == 0) {
		cout << "Slave processes number: " << slaves_number << endl;
		for (i = 0; i < SIZE; i++) {
			for (j = 0; j < SIZE; j++) {
				A[i][j] = rand();
				B[i][j] = rand();
			}
		}

		rows = SIZE / slaves_number;
		processed_rows = 0;
		clock_t begin = clock();

		for (dest_proc = 1; dest_proc <= slaves_number; dest_proc++) {
			MPI_Send(&processed_rows, 1, MPI_INT, dest_proc, 1, MPI_COMM_WORLD);
			MPI_Send(&rows, 1, MPI_INT, dest_proc, 1, MPI_COMM_WORLD);

			MPI_Send(&A[processed_rows][0], rows * SIZE, MPI_INT, dest_proc, 1, MPI_COMM_WORLD);

			MPI_Send(&B, SIZE * SIZE, MPI_INT, dest_proc, 1, MPI_COMM_WORLD);

			processed_rows = processed_rows + rows;
		}

		// Сбор результатов со всех зависимых процессоров
		for (i = 1; i <= slaves_number; i++) {
			source_proc = i;
			MPI_Recv(&processed_rows, 1, MPI_INT, source_proc, 2, MPI_COMM_WORLD, &status);
			MPI_Recv(&rows, 1, MPI_INT, source_proc, 2, MPI_COMM_WORLD, &status);
			MPI_Recv(&res_matrix[processed_rows][0], rows * SIZE, MPI_INT, source_proc, 2, MPI_COMM_WORLD, &status);
		}

		clock_t end = clock();

		double runTime = (double)(end - begin) / 1000;
		cout << "Runtime: " << runTime << "s" << endl;
	}


	// Зависимый процессор
	if (proc_rank > 0) {
		source_proc = 0;
		MPI_Recv(&processed_rows, 1, MPI_INT, source_proc, 1, MPI_COMM_WORLD, &status);
		MPI_Recv(&rows, 1, MPI_INT, source_proc, 1, MPI_COMM_WORLD, &status);
		MPI_Recv(&A, rows * SIZE, MPI_INT, source_proc, 1, MPI_COMM_WORLD, &status);
		MPI_Recv(&B, SIZE * SIZE, MPI_INT, source_proc, 1, MPI_COMM_WORLD, &status);
		for (k = 0; k < SIZE; k++) {
			for (i = 0; i < rows; i++) {
				res_matrix[i][k] = 0.0;
				for (j = 0; j < SIZE; j++) {
					res_matrix[i][k] = res_matrix[i][k] + A[i][j] * B[j][k];
				}
			}
		}

		MPI_Send(&processed_rows, 1, MPI_INT, 0, 2, MPI_COMM_WORLD);
		MPI_Send(&rows, 1, MPI_INT, 0, 2, MPI_COMM_WORLD);
		MPI_Send(&res_matrix, rows * SIZE, MPI_INT, 0, 2, MPI_COMM_WORLD);
	}

	MPI_Finalize();
}

