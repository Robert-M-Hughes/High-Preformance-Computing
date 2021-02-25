#!/bin/bash
#SBATCH --partition=compute   ### Partition
#SBATCH --job-name=Project02 ### Job Name
#SBATCH --time=03:00:00     ### WallTime
#SBATCH --nodes=2           ### Number of Nodes
#SBATCH --tasks-per-node=16 ### Number of tasks (MPI processes=nodes*tasks-per-node. In this case, 32. Below each MPI process spawns 4 threads)




for item in 0.6 1.1 1.25
do
    for ((i=2;i<16;i = i*2))
    do
		for ((j=0;j<10;j++))
		do
			srun --ntasks=i ./main ~/Lenna_Images/Lenna_org_1024.pgm $item 1024 >>runsSerial.csv
			srun --ntasks=i ./main ~/Lenna_Images/Lenna_org_2048.pgm $item 2048 >>runsSerial.csv
			srun --ntasks=i ./main ~/Lenna_Images/Lenna_org_4096.pgm $item 4096 >>runsSerial.csv
			srun --ntasks=i ./main ~/Lenna_Images/Lenna_org_5120.pgm $item 5120 >>runsSerial.csv
			srun --ntasks=i ./main ~/Lenna_Images/Lenna_org_7680.pgm $item 7680 >>runsSerial.csv
			srun --ntasks=i ./main ~/Lenna_Images/Lenna_org_10240.pgm $item 10240 >>runsSerial.csv
			srun  --ntasks=i ./main ~/Lenna_Images/Lenna_org_12800.pgm $item 12800 >>runsSerial.csv
		done
    done
done
