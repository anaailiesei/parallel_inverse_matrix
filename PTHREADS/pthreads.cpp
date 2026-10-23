#include <pthread.h>

#include <filesystem>
#include <iostream>
#include <vector>

#include "helpers.h"

#define NUM_THREADS 4

using namespace std;
namespace fs = std::filesystem;

pthread_barrier_t barrier;
Matrix mat;
int n;

void* worker(void* arg) {
	long tid = *(long*)arg;
	for (int i = tid; i < n; i += NUM_THREADS) {
		mat[i][i + n] = 1;
	}
	pthread_barrier_wait(&barrier);
	if (tid == 0) {
		for (int i = n - 1; i > 0; i--) {
			if (mat[i - 1][0] < mat[i][0]) {
				swap(mat[i], mat[i - 1]);
			}
		}
	}
	pthread_barrier_wait(&barrier);
    for (int i = 0; i < n; i++) {
		pthread_barrier_wait(&barrier);
		for (int j = tid; j < n; j += NUM_THREADS) {
			if (j == i) {
				continue;
			}
			double temp = mat[j][i] / mat[i][i];
			for (int k = 0; k < 2 * n; k++) {
				mat[j][k] -= mat[i][k] * temp;
			}
		}
		pthread_barrier_wait(&barrier);
	}
	pthread_barrier_wait(&barrier);
	for (int i = tid; i < n; i += NUM_THREADS) {
		double temp = mat[i][i];
		for (int j = 0; j < 2 * n; j++) {
            mat[i][j] /= temp;
        }
	}
	pthread_barrier_wait(&barrier);
	return NULL;
}

void findInvMatGaussJordanParallel(Matrix& input) {
	mat = input;
	n = mat.size();
	pthread_barrier_init(&barrier, NULL, NUM_THREADS);
	pthread_t threads[NUM_THREADS];
	long ids[NUM_THREADS];
	for (long id = 0; id < NUM_THREADS; id++) {
		ids[id] = id;
		pthread_create(&threads[id], NULL, worker, &ids[id]);
	}
	for (int id = 0; id < NUM_THREADS; id++) {
		pthread_join(threads[id], NULL);
	}
	input = mat;
	pthread_barrier_destroy(&barrier);
}

int main(int argc, char* argv[]) {
	main_loop(findInvMatGaussJordanParallel, argv[1]);

	return 0;
}
