import argparse
import os
import random


def generate_diagonally_dominant_matrix(n, low=-1000000, high=1000000):
    A = [[0] * n for _ in range(n)]

    for i in range(n):
        row = []
        for j in range(n):
            if i == j:
                row.append(0)
            else:
                row.append(random.randint(low, high))
        sum_off = sum(abs(x) for x in row)
        diag = sum_off + 1
        row[i] = diag
        A[i] = row

    return A


def generate_tridiagonal_matrix(n, diag=4, off=-1):
    A = [[0]*n for _ in range(n)]
    for i in range(n):
        A[i][i] = diag
        if i-1 >= 0: A[i][i-1] = off
        if i+1 < n:  A[i][i+1] = off
    return A


def generate_k_sparse_diagonally_dominant_matrix(n, k=5, low=-5, high=5):
    A = [[0]*n for _ in range(n)]
    for i in range(n):
        cols = list(range(n))
        cols.remove(i)
        pick = random.sample(cols, k=min(k, n-1))
        s = 0
        for j in pick:
            v = random.randint(low, high)
            A[i][j] = v
            s += abs(v)
        A[i][i] = s + 1
    return A


def main():
    parser = argparse.ArgumentParser(
        description="generate square input matrix"
    )
    parser.add_argument("n", type=int, help="dimension")
    parser.add_argument(
        "filename",
        type=str,
        help="output filename",
    )
    parser.add_argument(
        "--seed",
        type=int,
        default=None,
    )

    args = parser.parse_args()

    if args.seed is not None:
        random.seed(args.seed)

    n = args.n
    if n <= 0:
        raise ValueError("n must be positive")

    # mat = generate_diagonally_dominant_matrix(n)
    # mat =  generate_tridiagonal_matrix(n)
    mat = generate_k_sparse_diagonally_dominant_matrix(n)

    os.makedirs("inputs", exist_ok=True)

    out_path = os.path.join("inputs", args.filename)

    with open(out_path, "w") as f:
        f.write(f"{n}\n")
        for i in range(n):
            row_str = " ".join(str(x) for x in mat[i])
            f.write(row_str + "\n")

if __name__ == "__main__":
    main()
