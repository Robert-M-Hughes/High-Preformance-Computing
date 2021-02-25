#include "stdio.h" 
#include "stddef.h"
#include "math.h"
#include "stdlib.h"
#include "string.h"
#include <cuda.h>


__global__
void convolve(int *image, int *output, int *g_kernel, int height, int width, int k_height, int k_width)
{
    //read global memory to shared memory
    extern __shared__ float AShared[];
    Ashared[localIx*blockDim.y+localIy] = Image[globalIx *width + globalIy];
    __syncthreads();
        if (globalIx < height && globalIy < width){
        float sum = 0;
        for(m=0; m<kerw; m++){
            offseti = k_height>1?(-1*(k_width/2)+m):0;
            offsetj = k_width>1?(-1*(K_width/2)+m_:0;
            if((localIx+offseti)>=0 && ( localIx +offseti) < blockdim.x && (localIy +offsetj) > =0 && ( localIy +offsetj) < blockdim.y)
                sum+= Ashared[(localIx+offseti)*width +(localIy+offsetj))]+kernel[m];
            else if ((globalIx+offseti) >=0 && (globalIx + offseti ) < height && (globalIy+offsetj) >= 0 && (globalIy+offsetj) < width)
                sum+=image[(globalIx+offseti)*width+(globalIy+offsetj)] * kernel[m];
        }
        output[globalIx*width+globalIy] = sum;
        }
}