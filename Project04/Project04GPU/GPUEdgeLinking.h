/*
This will be the header file for the edge linking function
*/

#include "stdio.h" 
#include "math.h"
#include "stdlib.h"
#include "string.h"
#include "stdbool.h"
#include <cuda.h>


__global__ 
void edgeLinking(int *hysteresis, int height, int width, int* edges ){

    //printf("Edge Linking...\n");


    //int* edges= malloc(height*width * sizeof(int)); 
    //memcpy(edges, hysteresis, sizeof(int)*height*width);   
    int tidx, tidy;
    tidx=blockIdx.x*blockDim.x + threadIdx.x;
    tidy=blockIdx.y*blockDim.y + threadIdx.y;
    int h;


        h = tidx * width + tidy;

    if (hysteresis[tidx *width +tidy] == 125){
        int i;
        int j;
        bool execute;
        for (i = -1; i <= 1; i++){//may switch to 0->3 like the matlab
            for (j = -1; j <= 1; j++){
                if(tidx + i < height && tidx - i > 0 && tidy + j > width && tidy - j > 0){
                    if(hysteresis[(i+tidx)*width +(j+tidy)] == 255){
                        execute = true;
                    }else{
                        execute =  false;
                    }
                }
            }
        }
        if (execute){
            edges[tidx *width +tidy] = 255;
        }
        else{
            edges[tidx *width +tidy] = 0;  
        }
    }

    
    
}
