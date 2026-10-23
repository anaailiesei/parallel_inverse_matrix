#!/usr/bin/env bash
set -euo pipefail

INPUT_DIR="./inputs"
PROFILE_SCRIPT="./PTHREADS/profile.sh"

if [ ! -d "$INPUT_DIR" ]; then
    echo "Error: input directory '$INPUT_DIR' does not exist."
    exit 1
fi

if [ ! -x "$PROFILE_SCRIPT" ]; then
    echo "Error: profiling script '$PROFILE_SCRIPT' not found or not executable."
    exit 1
fi

for matrix_file in "$INPUT_DIR"/*.txt; do
    [ -f "$matrix_file" ] || continue

    echo "=========================================="
    echo "Running profiling for: $matrix_file"
    echo "=========================================="

    sbatch --wait -p haswell -n 4 "$PROFILE_SCRIPT" "$matrix_file"

    echo
done

echo "All inputs processed."
