#!/bin/bash
#SBATCH --partition=SP3
#SBATCH -J cooling
#SBATCH --nodes=20
#SBATCH --ntasks-per-node=20
#SBATCH --mail-type=ALL
#SBATCH --mail-user=artur.vemado@usp.br
#SBATCH --time=192:00:00

#run the application:
cd /scratch/8961981/TestSimCluster
mpiexec ./pluto
