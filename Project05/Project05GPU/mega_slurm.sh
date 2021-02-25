#!/bin/bash
  ### Partition
#SBATCH --job-name=MProject5 ### Job Name
#SBATCH --time=03:00:00     ### WallTime


for ((i=0;i<30;i = i+1))
do

		srun ./myCurandAppM 1 >>mega1.csv
		srun ./myCurandAppM 2 >>mega2.csv
		srun ./myCurandAppM 4 >>mega4.csv
		srun ./myCurandAppM 8 >>mega8.csv
		srun ./myCurandAppM 16 >>mega16.csv
		srun ./myCurandAppM 32 >>mega32.csv
		srun ./myCurandAppM 64 >>mega64.csv
		srun ./myCurandAppM 128 >>mega128.csv
		srun ./myCurandAppM 256 >>mega256.csv
		srun ./myCurandAppM 512 >>mega512.csv


		
done
