/*
This will be the header file for the Hysteresis

*/

#include "stdio.h" 
#include "math.h"
#include "stdlib.h"
#include "string.h"
#include <cuda.h>



//function definition

__global__
void hysteresis(int* hysteresisIm, int height, int width, int t_hi, int t_lo){



    int i;
    int j;
    i = blockIdx.x * blockDim.x + threadIdx.x;
    j = blockIdx.y * blockDim.y + threadIdx.y;


    if( hysteresisIm[i*width +j] > t_hi ){
        hysteresisIm[i*width +j] = 255;
    }
    else if (hysteresisIm[i*width +j] >t_lo){
        hysteresisIm[i*width +j] = 125;
    }
    else{
        hysteresisIm[i*width +j] = 0;
    }


//printf("Leaving hysteresis\n");
//return hysteresisIm;

}