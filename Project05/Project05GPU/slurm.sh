#!/bin/bash
  ### Partition
#SBATCH --job-name=Project5 ### Job Name
#SBATCH --time=03:00:00     ### WallTime


for item in 8 12 16 20 24 28 32
do
	for ((i=0;i<50;i = i+1))
	do

			srun ./test ../../Lenna_Images/Lenna_org_256.pgm .6 $item >>runs256.csv
			srun ./test ../../Lenna_Images/Lenna_org_512.pgm .6 $item >>runs512.csv
			srun ./test ../../Lenna_Images/Lenna_org_1024.pgm .6 $item >>runs1024.csv
			srun ./test ../../Lenna_Images/Lenna_org_2048.pgm .6 $item >>runs2048.csv
			srun ./test ../../Lenna_Images/Lenna_org_4096.pgm .6 $item >>runs4096.csv
			srun ./test ../../Lenna_Images/Lenna_org_10240.pgm .6 $item >>runs10240.csv
		done
done

