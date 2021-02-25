/*
This will serve as the file that has all the code to preform the convolution
Author: Robert Hughes

*/

#include "stdio.h" 
#include "stddef.h"
#include "math.h"
#include "stdlib.h"
#include "string.h"
#include <cuda.h>
#include <cuda_runtime_api.h>

//Function declaration 
/*
void * convolveVert(int *input_image, double* kernel, int j, int height, int width , int *array);
void * convolveHoriz(int* input_image, double* kernel, int j, int height, int width, int *array);
void * dataNormalize(int * data, int height, int width);
*/

//Function
__global__
void sharedConvol(int* image,int* output, double *g_kernel, int height, int width, int k_height, int k_width){

    int tidx, tidy;


    tidx = blockIdx.x * blockDim.x + threadIdx.x;
    tidy = blockIdx.y * blockDim.y + threadIdx.y;

    float kerw = (k_width> k_height)?k_width:k_height;
    int tx = threadIdx.x;
    int ty=threadIdx.y;

    int offseti;
    int offsetj;


    extern __shared__ float Ashared[];
    Ashared[tx*blockDim.y+ty] = image[tidx*width+tidy];
    __syncthreads();

    if(tidx<height && tidy < width){
        float sum = 0;
        int m;
        for(m=0; m<kerw;m++){
            offseti = k_height>1?(-1*(k_height/2)+m):0;
            offsetj = k_width>1?(-1*(k_width/2)+m):0;

            if( (tx+offseti) >=0 && (tidx +offseti) < blockDim.x && (ty +offsetj) >=0 && (ty+offsetj) < blockDim.y){
                sum += Ashared[(tx+offseti)*blockDim.y + (ty+offsetj)] *g_kernel[m];
                __syncthreads();
            }else if((tidx+offseti) >=0 && (tidx+offseti) < height && (tidy +offsetj) >= 0 && (tidy +offsetj) < width){
                sum += image[(tidx +offseti)*width+(tidy+offsetj)] * g_kernel[m];
                __syncthreads();
            }

        }
        output[tidx*width +tidy] = (int)sum;
    }

}






__global__
void convolveHoriz(int* input, double* kernel, int k_w, int height, int width, int* output){
    
    int offseth, offsetw, i, j, m;

    float sum;

    i = blockIdx.x * blockDim.x + threadIdx.x;
    j = blockIdx.y * blockDim.y + threadIdx.y;

    //printf("i: %d    j: %d\n", i, j);

    if(i < height && j < width ){
        sum = 0;
        for(m=0; m<k_w; m++){
            int offseth=0;
            int offsetw = -1*(k_w/2)+m;
            if((j+offsetw) >= 0 && (j+offsetw) < width){
                sum += input[(i+offseth)*width+j+offsetw]*kernel[m];
            }
        }
        //printf("%f", sum);
        output[i*width+j] = (int)sum;

    }

}

__global__
void convolveVert(int* input, double* kernel, int k_w, int height, int width, int* output){
    
    int offseth, offsetw, i, j, m;

    float sum;

    i = blockIdx.x * blockDim.x + threadIdx.x;
    j = blockIdx.y * blockDim.y + threadIdx.y;

    //printf("i: %d    j: %d\n", i, j);

    if(i < height && j < width ){
        sum = 0;
        for(m=0; m<k_w; m++){
            int offseth=(-1*(k_w/2)+m);
            int offsetw = 0;
            if((i+offseth) >= 0 && (i+offseth) < height){
                sum += input[(i+offseth)*width+j+offsetw]*kernel[m];
            }
        }
        //printf("%f", sum);
        output[i*width+j] = (int)sum;

    }

}










__global__
void convolveHoriz1(int* input_image, double* kernel, int w, int height, int width, int *array){
  //  printf("reached horizontal convolution\n");
   // printf("height %d", height);

    int i,j;
    i=blockIdx.x*blockDim.x + threadIdx.x;
    j=blockIdx.y*blockDim.y + threadIdx.y;


   //int * array = malloc(height*width*sizeof(int));
   if(i < height && j < width){
        int x;
    //printf("i %d\n");
        int sum = 0;
        int temp_loc = i*width+j;
        for( x = 0; x < j; x++){
            int offset = -1*(j/2)+x;
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
            
        }

                array[temp_loc] = (int)(sum);
                printf("%f ", sum);
    //printf("done\n");
   }


    //dataNormailize(array, height, width);
   
}


__global__
void convolveVert1(int* input_image, double* kernel, int w, int height, int width, int *array){
    //printf("reached Vertical convolution\n");
    int i,j,x;
    i=blockIdx.x*blockDim.x + threadIdx.x;
    j=blockIdx.y*blockDim.y + threadIdx.y;
    //nt * array = malloc(height*width*sizeof(int));

    int sum = 0;
    int temp_loc = i*width+j;
    for(x = 0; x < j; x++){
        int offset = (-1*(j/2)+x);
        //printf("Offset %d\n", offset);
        if( (temp_loc + offset*width) >= 0 && (temp_loc + offset*width) < (height*width) ){//check bounds
            //printf("in bounds\n");
            
            if((temp_loc + offset * width) < 0){ // if less than the col (off the top)
                sum += 0;
                // printf("fell off top\n");
            }
            else if ( (temp_loc + offset * width) >= height*width ){ // off the bottom
                sum +=0;
                //printf("fell off bottom\n");
            }
            else{
                //printf("HIT___________________________");
                sum += input_image[temp_loc + offset*width] * kernel[x];
                // printf("hit \n");
                // printf(" sum %f input %f kernel %f\n", sum, input_image[temp_loc + offset], kernel[x]);
            }
        
        }
        
    }

    array[temp_loc] = (int)(sum);
    //printf("%f ", sum);

   // printf("done\n");
    
    //dataNormailize(array, height, width);

    
    
}
