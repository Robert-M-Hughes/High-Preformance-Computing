#!/bin/bash
  ### Partition
#SBATCH --job-name=Project5 ### Job Name
#SBATCH --time=03:00:00     ### WallTime


for ((i=0;i<30;i = i+1))
do

		srun ./myCurandApp 1 >>kilo1.csv
		srun ./myCurandApp 2 >>kilo2.csv
		srun ./myCurandApp 4 >>kilo4.csv
		srun ./myCurandApp 8 >>kilo8.csv
		srun ./myCurandApp 16 >>kilo16.csv
		srun ./myCurandApp 32 >>kilo32.csv
		srun ./myCurandApp 64 >>kilo64.csv
		srun ./myCurandApp 128 >>kilo128.csv
		srun ./myCurandApp 256 >>kilo256.csv
		srun ./myCurandApp 512 >>kilo512.csv


		
done
