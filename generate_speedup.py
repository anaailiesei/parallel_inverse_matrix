import matplotlib.pyplot as plt

# t_sec = 7.37 # pentru 600
# t_sec = 9.01 # pentru 700
t_sec = 10.77 # pentru 800

cores = [1, 4, 8, 16]

# # Date matrice 600
# t_pthreads = [t_sec, 2.831, 1.710, 1.190]
# t_omp = [t_sec, 3.232, 2.091, 1.347]
# t_mpi = [t_sec, 3.506, 3.244, 3.415]

# # Date matrice 700
# t_pthreads = [t_sec, 4.359, 2.535, 1.647]
# t_omp = [t_sec, 4.893, 2.955, 2.000]
# t_mpi = [t_sec, 4.929, 4.862, 4.510]

# Date matrice 800
t_pthreads = [t_sec, 5.731, 3.535, 2.218]
t_omp = [t_sec, 7.126, 4.217, 2.609]
t_mpi = [t_sec, 6.019, 4.862, 4.510]

s_pthreads = [t_sec / t for t in t_pthreads]
s_omp = [t_sec / t for t in t_omp]
s_mpi = [t_sec / t for t in t_mpi]

plt.figure(figsize=(10, 6))
plt.plot(cores, s_pthreads, marker='o', color='blue', label='Pthreads', linewidth=2)
plt.plot(cores, s_omp, marker='s', color='green', label='OpenMP', linewidth=2)
plt.plot(cores, s_mpi, marker='^', color='red', label='MPI', linewidth=2)

plt.title('Analiza Speedup: Pthreads vs OpenMP vs MPI (Matrice 800x800)', fontsize=14)
plt.xlabel('Numar de Nuclee / Procese', fontsize=12)
plt.ylabel('Speedup ($S = T_{sec} / T_{par}$)', fontsize=12)
plt.xticks(cores)
plt.grid(True, linestyle=':', alpha=0.6)
plt.legend()

plt.show()