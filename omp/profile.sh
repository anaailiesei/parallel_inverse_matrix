#!/bin/bash
#SBATCH -p haswell
#SBATCH --time=00:10:00
#SBATCH -n 4
#SBATCH --job-name=profile_omp
#SBATCH --output=slurm-%j.out

###############################################
# RUN THIS SCRIPT FROM app/ DIRECTORY
# example:
# sbatch profile_omp.sh inputs/matrix1.txt
###############################################

set -euo pipefail

# ===============================
# Argument checking
# ===============================
if [ $# -ne 1 ]; then
    echo "Usage: sbatch profile_omp.sh <input_matrix>"
    echo "Example: sbatch profile_omp.sh inputs/matrix1.txt"
    exit 1
fi

INPUT="$1"
APP_DIR="$SLURM_SUBMIT_DIR"

if [ ! -f "$APP_DIR/$INPUT" ]; then
    echo "Error: Input file '$APP_DIR/$INPUT' does not exist!"
    exit 1
fi

# ===============================
# Paths (relative to app/)
# ===============================
OMP_DIR="$APP_DIR/omp"
EXEC="$OMP_DIR/omp"                 # your built binary path
RESULTS_DIR="$APP_DIR/omp"          # store VTune results under app/OMP

# Ensure executable exists
if [ ! -x "$EXEC" ]; then
    echo "Error: Executable '$EXEC' not found or not executable!"
    exit 1
fi

# ===============================
# Load VTune + MPI
# ===============================
source /opt/intel/oneapi/vtune/latest/vtune-vars.sh
module load mpi/openmpi-x86_64

# Final location (persistent)
base="$(basename "$INPUT" .txt)"
FINAL_BASE="$RESULTS_DIR/vtune_results_${base}"
mkdir -p "$FINAL_BASE"

# Local scratch (fast) for VTune working/result files
LOCAL_BASE="${SLURM_TMPDIR:-/tmp/$USER/vtune_${SLURM_JOB_ID}}"
mkdir -p "$LOCAL_BASE"

PS_DIR="$LOCAL_BASE/vtune_ps_${base}"
HS_DIR="$LOCAL_BASE/vtune_hs_${base}"

echo "=== VTune Performance Snapshot (OMP) ==="
vtune -collect performance-snapshot \
      -finalization-mode=fast \
      -result-dir "$PS_DIR" \
      -- mpirun -np 4 "$EXEC" "$APP_DIR/$INPUT"

echo "=== VTune Hotspots Analysis (OMP) ==="
vtune -collect hotspots \
      -finalization-mode=fast \
      -knob enable-stack-collection=false \
      -result-dir "$HS_DIR" \
      -- mpirun -np 4 "$EXEC" "$APP_DIR/$INPUT"

echo "=== Copying results to $FINAL_BASE ==="
cp -a "$PS_DIR" "$HS_DIR" "$FINAL_BASE/"

echo "=== DONE === Results stored in $FINAL_BASE ==="
