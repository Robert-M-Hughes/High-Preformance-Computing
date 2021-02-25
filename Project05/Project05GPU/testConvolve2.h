#include "stdio.h" 
#include "stddef.h"
#include "math.h"
#include "stdlib.h"
#include "string.h"
#include <cuda.h>


__global__
void * convolveHoriz(int* input_image, double* kernel, int w, int height, int width, int *array){
  //  printf("reached horizontal convolution\n");
   // printf("height %d", height);

    int tidx,tidy;
    tidx=blockIdx.x*blockDim.x + threadIdx.x;
    tidy=blockIdx.y*blockDim.y + threadIdx.y;


   //int * array = malloc(height*width*sizeof(int));
   if(tidx < height && tidy < width){
        int x;

        int sum = 0;
        int temp_loc = i*width+j;
        for( x = 0; x < w; x++){
            int offset = -1*floor(w/2)+x;
            //printf("Offset %d\n", offset);
            if( (temp_loc + offset) >= 0 && (temp_loc + offset) < (height*width) ){//check bounds
            //printf("in bounds\n");
                if((j + offset) >= 0 ){//make sure not exceeding row onto another row
                //printf("Not Hit col\n");
                    if((temp_loc + offset) < (i * width - i)){ // if less than the row (off the side)
                        sum += 0;
                    // printf("fell off left\n");
                    }
                    else if ( (temp_loc + offset) >= (i * width + width) ){ // off the side to the right
                        sum +=0;
                        //printf("fell off right\n");
                    }
                    else{

                        sum += input_image[temp_loc + offset] * kernel[x];
                        //printf("hit \n");
                    // printf(" sum %f input %f kernel %f\n", sum, input_image[temp_loc + offset], kernel[x]);
                    }
                }
            }
            
        

            array[temp_loc] = (int)(sum);
                //printf("%f ", sum);
            
            //printf("\n");
        }
    //printf("done\n");
   }