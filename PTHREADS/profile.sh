#!/bin/bash
#SBATCH -p haswell
#SBATCH --time=00:10:00
#SBATCH -n 1
#SBATCH --cpus-per-task=4
#SBATCH --job-name=profile_pthreads
#SBATCH --output=slurm-%j.out
#SBATCH --export=ALL

###############################################
# RUN THIS SCRIPT FROM app/ DIRECTORY
# example:
# sbatch profile_pthreads.sh inputs/matrix1.txt
###############################################

set -euo pipefail

# ===============================
# Argument checking
# ===============================
if [ $# -ne 1 ]; then
    echo "Usage: sbatch profile_pthreads.sh <input_matrix>"
    echo "Example: sbatch profile_pthreads.sh inputs/matrix1.txt"
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
PTHREADS_DIR="$APP_DIR/PTHREADS"
EXEC="$PTHREADS_DIR/pthreads"
RESULTS_DIR="$PTHREADS_DIR"

if [ ! -x "$EXEC" ]; then
    echo "Error: Executable '$EXEC' not found or not executable!"
    exit 1
fi

# ===============================
# Load VTune
# ===============================
source /opt/intel/oneapi/vtune/latest/vtune-vars.sh

# Bind threads nicely
export OMP_NUM_THREADS="${SLURM_CPUS_PER_TASK:-4}"   # harmless if unused
export SLURM_CPU_BIND=cores

# Final location (persistent)
base="$(basename "$INPUT" .txt)"
FINAL_BASE="$RESULTS_DIR/vtune_results_${base}_${SLURM_JOB_ID}"
mkdir -p "$FINAL_BASE"

# Local scratch (fast) for VTune working/result files
LOCAL_BASE="${SLURM_TMPDIR:-/tmp/$USER/vtune_${SLURM_JOB_ID}}"
mkdir -p "$LOCAL_BASE"

PS_DIR="$LOCAL_BASE/vtune_ps_${base}"
HS_DIR="$LOCAL_BASE/vtune_hs_${base}"

THREADS="${SLURM_CPUS_PER_TASK:-4}"

echo "=== VTune Performance Snapshot (PTHREADS, ${THREADS} threads) ==="
srun --ntasks=1 --cpus-per-task="$THREADS" --cpu-bind=cores \
  vtune -collect performance-snapshot \
        -finalization-mode=full \
        -result-dir "$PS_DIR" \
        -- "$EXEC" "$APP_DIR/$INPUT"

echo "=== VTune Hotspots Analysis (PTHREADS, ${THREADS} threads) ==="
srun --ntasks=1 --cpus-per-task="$THREADS" --cpu-bind=cores \
  vtune -collect hotspots \
        -finalization-mode=full \
        -knob enable-stack-collection=false \
        -result-dir "$HS_DIR" \
        -- "$EXEC" "$APP_DIR/$INPUT"

# Sanity check: avoid copying empty results silently
if [ ! -d "$PS_DIR" ] || [ -z "$(ls -A "$PS_DIR" 2>/dev/null)" ]; then
    echo "Error: VTune performance-snapshot result dir is missing or empty: $PS_DIR"
    exit 1
fi
if [ ! -d "$HS_DIR" ] || [ -z "$(ls -A "$HS_DIR" 2>/dev/null)" ]; then
    echo "Error: VTune hotspots result dir is missing or empty: $HS_DIR"
    exit 1
fi

echo "=== Copying results to $FINAL_BASE ==="
cp -a "$PS_DIR" "$HS_DIR" "$FINAL_BASE/"

echo "=== DONE === Results stored in $FINAL_BASE ==="
