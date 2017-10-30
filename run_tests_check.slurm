#!/bin/bash
#SBATCH -n 1  # Number of tasks
#SBATCH -c 20 # Number of cores requested
#SBATCH -N 1  # Number of nodes requested
#SBATCH -t 300  # Runtime in minutes
#SBATCH -o on_a_single_node.out # Standard out goes to this file
#SBATCH -e on_a_single_node.err # Standard err goes to this file
#SBATCH --mail-type ALL
#SBATCH --mail-user tknappra@hawaii.edu
#SBATCH -p ics632.q
#SBATCH --account=ics_632

# setup vars
echo "hello" > temp.txt
