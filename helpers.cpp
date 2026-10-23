#include "helpers.h"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <cmath>

#define PRECISION 10

Matrix readMatrix(string filename, bool augmented) {
	ifstream fin(filename);
	if (!fin.is_open()) {
		cerr << "File opening failed\n";
		return {};
	}

	int n;
	fin >> n;
	if (!fin) return {};

	Matrix values;

	if (augmented)
		values.resize(n, vector<double>(2 * n, 0.0f));
	else
		values.resize(n, vector<double>(n, 0.0f));

	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			fin >> values[i][j];
			if (!fin) {
				return {};
			}
		}
	}
	return values;
}

void printInverseMatrix(Matrix mat, string filename) {
	ofstream fout(filename);
	if (!fout.is_open()) {
		cerr << "File opening failed\n";
		return;
	}
	int n = mat.size();
	fout << n << endl;

	for (int i = 0; i < n; i++) {
		for (int j = n; j < n * 2; j++) {
			fout << fixed << setprecision(PRECISION) << mat[i][j] << "\t";
		}
		fout << endl;
	}
	return;
}

void printMatrix(Matrix mat) {
	int n = mat.size();

	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			cout << fixed << setprecision(PRECISION) << mat[i][j] << "\t";
		}
		cout << endl;
	}
	return;
}

bool equalMatrices(const Matrix &a, const Matrix &b, double eps) {
    if (a.size() != b.size())
        return false;

    if (a.empty())
        return true;

    if (a[0].size() != b[0].size()) {
		return false;
	}
	
    for (size_t i = 0; i < a.size(); ++i) {
        if (a[i].size() != b[i].size())
            return false;

        for (size_t j = 0; j < a.size(); ++j) {
            if (fabs(a[i][j] - b[i][j]) > eps)
				return false;
                
        }
    }
    return true;
}

void last_half(Matrix &mat) {
    if (mat.empty()) return;
    std::size_t cols = mat[0].size();
    std::size_t half = cols / 2;

    for (auto &row : mat) {
        if (row.size() != cols) {
            continue;
        }
        row.erase(row.begin(), row.begin() + half);
    }
}

void main_loop(void (*findInvMatGaussJordan)(Matrix&),  string file) {
	fs::path in_path = fs::absolute(file);
	fs::path output_dir = fs::absolute("outputs");

	if (!fs::exists(in_path)) {
        std::cerr << "Input file does not exist: " << in_path << "\n";
        return;
    }

	fs::path out_path = fs::absolute("outputs") / in_path.filename();

	if (!fs::exists(out_path)) {
        std::cerr << "Expected output file does not exist: " << out_path << "\n";
        return;
    }

	Matrix mat = readMatrix(in_path.string());
    Matrix expected = readMatrix(out_path.string(), false);

    findInvMatGaussJordan(mat);
    last_half(mat);

    if (equalMatrices(mat, expected, STD_ERR_VAL)) {
        std::cout << in_path.filename().string() << " : OK\n";
    } else {
        std::cout << in_path.filename().string() << " : MISMATCH\n";
    }
}
