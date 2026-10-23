import matplotlib.pyplot as plt

seq = {
    300: 0.580,
    500: 2.610,
    600: 7.370,
    700: 9.010,
    800: 10.770
}

times = {
    300: {4: {"Pthreads": 0.635, "OpenMP": 0.715, "MPI": 1.860},
          8: {"Pthreads": 0.435, "OpenMP": 0.515, "MPI": 1.950},
          16: {"Pthreads": 0.235, "OpenMP": 0.315, "MPI": 2.050}},
    500: {4: {"Pthreads": 1.710, "OpenMP": 2.029, "MPI": 2.711},
          8: {"Pthreads": 1.010, "OpenMP": 1.320, "MPI": 2.910},
          16: {"Pthreads": 0.710, "OpenMP": 0.920, "MPI": 3.110}},
    600: {4: {"Pthreads": 2.831, "OpenMP": 3.232, "MPI": 3.506},
          8: {"Pthreads": 1.710, "OpenMP": 2.091, "MPI": 3.244},
          16: {"Pthreads": 1.190, "OpenMP": 1.347, "MPI": 3.415}},
    700: {4: {"Pthreads": 4.359, "OpenMP": 4.893, "MPI": 4.929},
          8: {"Pthreads": 2.535, "OpenMP": 2.955, "MPI": 4.862},
          16: {"Pthreads": 1.647, "OpenMP": 2.000, "MPI": 4.510}},
    800: {4: {"Pthreads": 5.731, "OpenMP": 7.126, "MPI": 6.019},
          8: {"Pthreads": 3.535, "OpenMP": 4.217, "MPI": 4.862},
          16: {"Pthreads": 2.218, "OpenMP": 2.609, "MPI": 4.510}},
}

sizes = sorted(seq.keys())
methods = ["Pthreads", "OpenMP", "MPI"]
cores_list = [4, 8, 16]

plt.figure(figsize=(10, 5))

for method in methods:
    for c in cores_list:
        speedups = []
        for n in sizes:
            tp = times[n][c][method]
            speedups.append(seq[n] / tp)
        plt.plot(sizes, speedups, marker="o", label=f"{method} - {c} nuclee")

plt.axhline(1.0, linestyle="--")

plt.xticks(sizes, [f"{n}x{n}" for n in sizes])
plt.xlabel("Dimensiune matrice")
plt.ylabel("Speedup vs. secvential (T_secv / T_paralel)")
plt.title("Speedup si vizualizare overhead")
plt.grid(True, alpha=0.3)
plt.legend(fontsize=8, ncol=3)
plt.tight_layout()
plt.show()
