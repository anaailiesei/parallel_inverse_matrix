#!/bin/bash
#SBATCH -p haswell
#SBATCH --time=00:10:00
#SBATCH -n 4
#SBATCH --job-name=profile_mpi
#SBATCH --output=slurm-%j.out

###############################################
# RUN THIS SCRIPT FROM app/ DIRECTORY
# example:
# sbatch profile_mpi.sh inputs/matrix1.txt
###############################################

# ===============================
# Argument checking
# ===============================

if [ $# -ne 1 ]; then
    echo "Usage: sbatch profile_mpi.sh <input_matrix>"
    echo "Example: sbatch profile_mpi.sh inputs/matrix1.txt"
    exit 1
fi

INPUT="$1"

if [ ! -f "$INPUT" ]; then
    echo "Error: Input file '$INPUT' does not exist!"
    exit 1
fi

# ===============================
# Paths (relative to app/)
# ===============================

APP_DIR="$SLURM_SUBMIT_DIR"       # directory where sbatch was submitted
MPI_DIR="$APP_DIR/MPI"            # your MPI folder
EXEC="$MPI_DIR/mpi"               # MPI executable
RESULTS_DIR="$MPI_DIR"            # store VTune output in MPI/

# Ensure MPI executable exists
if [ ! -x "$EXEC" ]; then
    echo "Error: Executable '$EXEC' not found or not executable!"
    exit 1
fi

# VTUNE results will be created automatically inside MPI_DIR
mkdir -p "$RESULTS_DIR"


source /opt/intel/oneapi/vtune/latest/vtune-vars.sh
module load mpi/openmpi-x86_64

# Silence fabric/openib noise (not required, but cleaner)
export OMPI_MCA_btl="^ofi,usnic,openib"
export OMPI_MCA_mtl="^ofi"

# Final location (persistent)
base="$(basename "$INPUT" .txt)" 
FINAL_BASE="$MPI_DIR/vtune_results_${base}"
mkdir -p "$FINAL_BASE"

# Local scratch (fast) for VTune working/result files
LOCAL_BASE="${SLURM_TMPDIR:-/tmp/$USER/vtune_${SLURM_JOB_ID}}"
mkdir -p "$LOCAL_BASE"

PS_DIR="$LOCAL_BASE/vtune_ps_${base}"
HS_DIR="$LOCAL_BASE/vtune_hs_${base}"

echo "=== VTune Performance Snapshot ==="
vtune -collect performance-snapshot \
      -finalization-mode=fast \
      -result-dir "$PS_DIR" \
      -- mpirun -np "${SLURM_NTASKS}" "$EXEC" "$APP_DIR/$INPUT"

echo "=== VTune Hotspots Analysis ==="
vtune -collect hotspots \
      -finalization-mode=fast \
      -knob enable-stack-collection=false \
      -result-dir "$HS_DIR" \
      -- mpirun -np "${SLURM_NTASKS}" "$EXEC" "$APP_DIR/$INPUT"

echo "=== Copying results to $FINAL_BASE ==="
cp -a "$PS_DIR" "$HS_DIR" "$FINAL_BASE/"

echo "=== DONE === Results stored in $FINAL_BASE ==="