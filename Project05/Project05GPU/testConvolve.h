#include "stdio.h" 
#include "stddef.h"
#include "math.h"
#include "stdlib.h"
#include "string.h"
#include <cuda.h>
#include <cuda_runtime_api.h>
//Function declaration 
//void * convolve(int *image, int *output, int *g_kernel, int height, int width, int k_height, int k_width);

//Function
__global__
void convolve(int *image, int *output, double *g_kernel, int height, int width, int k_height, int k_width)

{
    printf("Height: %d\n", height);
    printf("Width: %d\n", width);

        int tidx,tidy;
        tidx=blockIdx.x*blockDim.x + threadIdx.x;
        tidy=blockIdx.y*blockDim.y + threadIdx.y;
        
        int kerw=(k_width>k_height)?k_width:k_height;
        int sum;

    //printf("tidx: %d\n", tidx);
    //printf("tidy: %d\n", tidy);

    if (tidx<height && tidy < width)
    {	sum = 0;

                // Iteration through  gaussian kernel
        for(int k=0; k<kerw;k++)
            {
        
            int offseti = k_height>1?(-1*(k_height/2)+k):0; 
            int offsetj =k_width>1? (-1*(k_width/2)+k):0;
            //printf("offseti: %d\n", offseti);
            //printf("offsetj: %d\n", offsetj);
            //printf("tidx +off: %d\n", tidx+offseti);
            //printf("tidy +off: %d\n", tidy+offsetj);       
                    
            if( (tidx+offseti)>=0 && (tidx+offseti)<height && (tidy+offsetj)>=0 && (tidy+offsetj)<width )
                {
                    printf("inside\n");
                //printf("image %d\n", image[(tidx+offseti)*width+tidy+offsetj]);
                //printf("kernel %f\n", g_kernel[k]);

                    sum += (image[(tidx+offseti)*width+tidy+offsetj])*(g_kernel[k]);
                
                        

                }
            }
                    *(output +(tidx*width) + tidy) = (int)sum;


    }
}
