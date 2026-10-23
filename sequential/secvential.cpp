#include <iostream>
#include <vector>
#include <cmath>

#include "helpers.h"


bool check_identity(Matrix &mat) {
	int n = mat.size();
	int m = mat[0].size();

	if (2 * n != m) return false;

	for (int i = 0; i < n; ++i)
		for (int j = 0; j < n; ++j) {
			if (i == j && fabs(mat[i][j] - 1) > STD_ERR_VAL) {
				cout <<  n << " " <<  i << " " << j << " " << mat[i][j] << endl;
				return false;
			}
			if (i != j && fabs(mat[i][j]) > STD_ERR_VAL) {
				cout <<  n << " " <<  i << " " << j << " " << mat[i][j] << endl;
				return false;
			}
		}

	return true;
}

void findInvMatGaussJordan(Matrix &mat) {
	double temp;
	int n = mat.size();

	for (int i = 0; i < n; i++) {
		for (int j = 0; j < 2 * n; j++) {
			if (j == (i + n)) mat[i][j] = 1;
		}
	}
	
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			if (j != i) {
				temp = mat[j][i] / mat[i][i];
				for (int k = 0; k < 2 * n; k++) {
					mat[j][k] -= mat[i][k] * temp;
				}
			}
		}
	}
	for (int i = 0; i < n; i++) {
		temp = mat[i][i];
		for (int j = 0; j < 2 * n; j++) {
			mat[i][j] = mat[i][j] / temp;
		}
	}
	return;
}
int main(int argc, char** argv) {
	fs::path input_dir = fs::absolute("inputs");
	fs::path output_dir = fs::absolute("outputs");

	if (argc != 2) {
		cerr << "Provide 1 input. all or the name of the input file" << endl;
	}

	if (std::string(argv[1]) == "all") {
			for (const auto &entry : fs::directory_iterator(input_dir)) {
			if (!entry.is_regular_file() || entry.path().extension() != ".txt")
				continue;

			fs::path in_path = entry.path();

			Matrix mat = readMatrix(in_path.string());
			findInvMatGaussJordan(mat);

			if (!check_identity(mat)) cout << "Matrix is not the correct inverse: " << in_path.filename() << endl;

			fs::path out_path = output_dir / in_path.filename();
			printInverseMatrix(mat, out_path.string());
		}
	} else {
		main_loop(findInvMatGaussJordan, argv[1]);
	}

	return 0;
}