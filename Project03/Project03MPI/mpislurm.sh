#!/bin/bash
#SBATCH --partition=compute   ### Partition
#SBATCH --job-name=Project03MPI ### Job Name
#SBATCH --time=03:00:00     ### WallTime
#SBATCH --nodes=1           ### Number of Nodes
#SBATCH --ntasks-per-node=2 ### Number of tasks (MPI processes=nodes*tasks-per-node. In this case, 32. Below each MPI process spawns 4 threads)




for item in 0.6 1.1 1.25
do
	for (i = 0; i < 16; i = i + 4)
	do
		for ((j=0;j<10;j++))
		do
			srun ./main ~/Lenna_Images/Lenna_org_1024.pgm $item 1024 i >>runsSerial2.csv
			srun ./main ~/Lenna_Images/Lenna_org_2048.pgm $item 2048 i >>runsSerial2.csv
			srun ./main ~/Lenna_Images/Lenna_org_4096.pgm $item 4096 i >>runsSerial2.csv
			srun ./main ~/Lenna_Images/Lenna_org_5120.pgm $item 5120 i >>runsSerial2.csv
			srun ./main ~/Lenna_Images/Lenna_org_7680.pgm $item 7680 i >>runsSerial2.csv
			srun ./main ~/Lenna_Images/Lenna_org_10240.pgm $item 10240 i >>runsSerial2.csv
			srun ./main ~/Lenna_Images/Lenna_org_12800.pgm $item 12800 i >>runsSerial2.csv
		done
	done
done
