#!/bin/bash
#SBATCH --partition=compute   ### Partition
#SBATCH --job-name=Project02 ### Job Name
#SBATCH --time=2:00:00     ### WallTime
#SBATCH --nodes=1           ### Number of Nodes
#SBATCH --ntasks-per-node=1 ### Number of tasks (MPI processes)
##SBATCH --cpus-per-task=2  ### Number of threads per task (OMP threads). Get num threads using program arguments




for item in 0.6 1.1 1.25
do
	for (i = 0; i < 16; i = i + 4)
	do
		for ((j=0;j<10;j++))
		do
			./main ~/Lenna_Images/Lenna_org_1024.pgm $item $i>>runs6.csv
			./main ~/Lenna_Images/Lenna_org_2048.pgm $item $i>>runs6.csv
			./main ~/Lenna_Images/Lenna_org_4096.pgm $item $i>>runs6.csv
			./main ~/Lenna_Images/Lenna_org_5120.pgm $item $i>>runs6.csv
			./main ~/Lenna_Images/Lenna_org_7680.pgm $item $i>>runs6.csv
			./main ~/Lenna_Images/Lenna_org_10240.pgm $item $i>>runs6.csv
			./main ~/Lenna_Images/Lenna_org_12800.pgm $item $i>>runs6.csv
		done
	done
done
