#include <omp.h>

#include <iostream>
#include <vector>

#include "helpers.h"

void findInvMatGaussJordan(Matrix &mat) {
	int n = mat.size();

#pragma omp parallel for
	for (int i = 0; i < n; ++i) {
		mat[i][i + n] = 1;
	}

	std::vector<double> temp_row(n);

	for (int i = 0; i < n; ++i) {
		double pivot = mat[i][i];

		#pragma omp parallel for
		for (int j = 0; j < n; ++j) {
			temp_row[j] = (j == i) ? 0.0 : mat[j][i] / pivot;
		}

		#pragma omp parallel for collapse(2)
		for (int j = 0; j < i; ++j) {
			for (int k = 0; k < 2 * n; ++k) {
				mat[j][k] -= mat[i][k] * temp_row[j];
			}
		}

		#pragma omp parallel for collapse(2)
		for (int j = i + 1; j < n; ++j) {
			for (int k = 0; k < 2 * n; ++k) {
				mat[j][k] -= mat[i][k] * temp_row[j];
			}
		}
	}

	#pragma omp parallel for
	for (int i = 0; i < n; ++i) {
		temp_row[i] = mat[i][i];
	}

	#pragma omp parallel for collapse(2)
	for (int i = 0; i < n; ++i) {
		for (int j = 0; j < 2 * n; ++j) {
			mat[i][j] = mat[i][j] / temp_row[i];
		}
	}

	return;
}
int main(int argc, char* argv[]) {
	main_loop(findInvMatGaussJordan, argv[1]);
	return 0;
}