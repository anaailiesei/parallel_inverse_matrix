#include <mpi.h>

#include <iostream>
#include <vector>

#include "helpers.h"

int owner_of_row(int i, int n, int size) {
	int base = n / size;
	int rem = n % size;
	int start = 0;
	for (int r = 0; r < size; ++r) {
		int rows_r = base + (r < rem ? 1 : 0);
		int end = start + rows_r;
		if (i >= start && i < end) return r;
		start = end;
	}
	return size - 1;
}

int row_start_for_rank(int r, int n, int size) {
	int base = n / size;
	int rem = n % size;
	return r * base + std::min(r, rem);
}

void bcastMatrix(Matrix& mat, int rank) {
	int n = 0, m = 0;
	if (rank == 0) {
		n = static_cast<int>(mat.size());
		m = n ? static_cast<int>(mat[0].size()) : 0;
	}

	MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&m, 1, MPI_INT, 0, MPI_COMM_WORLD);

	if (rank != 0) {
		mat.assign(n, std::vector<double>(m));
	}

	std::vector<double> buf(n * m);
	if (rank == 0) {
		for (int i = 0; i < n; ++i)
			for (int j = 0; j < n; ++j) buf[i * m + j] = mat[i][j];
	}

	MPI_Bcast(buf.data(), n * m, MPI_DOUBLE, 0, MPI_COMM_WORLD);

	if (rank != 0) {
		for (int i = 0; i < n; ++i)
			for (int j = 0; j < n; ++j) mat[i][j] = buf[i * m + j];
	}
}

Matrix gatherFullMatrix(const Matrix& mat) {
	int rank, size;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	int n = static_cast<int>(mat.size());
	int cols = mat.empty() ? 0 : static_cast<int>(mat[0].size());

	int base = n / size;
	int rem = n % size;
	int localRows = base + (rank < rem ? 1 : 0);
	int rowStart = row_start_for_rank(rank, n, size);

	std::vector<double> sendbuf(localRows * cols);
	for (int li = 0; li < localRows; ++li) {
		int global_i = rowStart + li;
		for (int k = 0; k < cols; ++k) {
			sendbuf[li * cols + k] = mat[global_i][k];
		}
	}

	std::vector<int> recvcounts;
	std::vector<int> displs;
	std::vector<double> recvbuf;

	if (rank == 0) {
		recvcounts.resize(size);
		displs.resize(size);

		int offset = 0;
		for (int r = 0; r < size; ++r) {
			int rows_r = base + (r < rem ? 1 : 0);
			recvcounts[r] = rows_r * cols;
			displs[r] = offset;
			offset += recvcounts[r];
		}

		recvbuf.resize(n * cols);
	}

	MPI_Gatherv(sendbuf.data(), localRows * cols, MPI_DOUBLE,
				rank == 0 ? recvbuf.data() : nullptr,
				rank == 0 ? recvcounts.data() : nullptr,
				rank == 0 ? displs.data() : nullptr, MPI_DOUBLE, 0,
				MPI_COMM_WORLD);

	Matrix full;

	if (rank == 0) {
		full.assign(n, std::vector<double>(cols));

		int offset = 0;
		for (int r = 0; r < size; ++r) {
			int rows_r = base + (r < rem ? 1 : 0);
			int rowStart =
				row_start_for_rank(r, n, size);

			for (int li = 0; li < rows_r; ++li) {
				int global_i = rowStart + li;
				for (int k = 0; k < cols; ++k) {
					full[global_i][k] = recvbuf[offset + li * cols + k];
				}
			}
			offset += rows_r * cols;
		}
	}

	return full;
}

void main_loop_mpi(void (*findInvMatGaussJordan)(Matrix&), std::string file) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (file.empty()) {
        if (rank == 0)
            std::cerr << "No input file provided.\n";
        return;
    }

    fs::path in_path = fs::absolute(file);
    fs::path out_path = fs::absolute("outputs") / in_path.filename();

    Matrix mat, expected;

    if (rank == 0) {
        if (!fs::exists(in_path)) {
            std::cerr << "Input file does not exist: " << in_path << "\n";
            return;
        }

        if (in_path.extension() != ".txt") {
            std::cerr << "Input file must be a .txt file: " << in_path << "\n";
            return;
        }

        if (!fs::exists(out_path)) {
            std::cerr << "Expected output file does not exist: " << out_path << "\n";
            return;
        }

        mat = readMatrix(in_path.string());
        expected = readMatrix(out_path.string(), false);
    }

    bcastMatrix(mat, rank);

    if (mat.empty()) {
        if (rank == 0)
            std::cerr << "ERROR: Failed to read input matrix.\n";
        return;
    }

    findInvMatGaussJordan(mat);

    MPI_Barrier(MPI_COMM_WORLD);

    Matrix full = gatherFullMatrix(mat);

    if (rank == 0) {
        last_half(full);

        if (equalMatrices(full, expected, STD_ERR_VAL)) {
            std::cout << in_path.filename().string() << " : OK\n";
        } else {
            std::cout << in_path.filename().string() << " : MISMATCH\n";
        }
    }
}


void findInvMatGaussJordan(Matrix& mat) {
	int rank, size;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	int n = mat.size();

	int base = n / size;
	int rem = n % size;
	int localRows = base + (rank < rem ? 1 : 0);
	int rowStart = row_start_for_rank(rank, n, size);
	int cols = 2 * n;

	for (int local_i = 0; local_i < localRows; ++local_i) {
		int global_i = rowStart + local_i;
		mat[global_i][global_i + n] = 1;
	}

	std::vector<double> pivot_row(cols);

	for (int i = 0; i < n; ++i) {
		int pivot_owner = owner_of_row(i, n, size);

		if (rank == pivot_owner) {
			std::copy(mat[i].begin(), mat[i].end(), pivot_row.begin());
		}

		MPI_Bcast(pivot_row.data(), cols, MPI_DOUBLE, pivot_owner,
				  MPI_COMM_WORLD);

		double pivot = pivot_row[i];

		for (int local_j = 0; local_j < localRows; ++local_j) {
			int global_j = rowStart + local_j;
			if (global_j == i) continue;

			double factor = mat[global_j][i] / pivot;

			for (int k = 0; k < cols; ++k) {
				mat[global_j][k] -= factor * pivot_row[k];
			}
		}
	}

	for (int local_i = 0; local_i < localRows; ++local_i) {
		int global_i = rowStart + local_i;
		double diag = mat[global_i][global_i];
		for (int j = 0; j < cols; ++j) {
			mat[global_i][j] /= diag;
		}
	}
}
int main(int argc, char** argv) {
	MPI_Init(&argc, &argv);
	main_loop_mpi(findInvMatGaussJordan, argv[1]);
	MPI_Finalize();
	return 0;
}