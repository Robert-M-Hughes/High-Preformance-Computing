#include <cuda_runtime_api.h>
#include <cuda.h>


//Function
/*
__host__ __device__ int d_atan2( int a, int b )
{
    return atan2(a,b);
}
__host__ __device__ int D_sqrt( int a)
{
    return sqrt(a);
}
*/

__global__
void magnitude(int *Horizontal, int *Vertical, int *output_mag, int height, int width){

        int tidx, tidy;
        tidx=blockIdx.x*blockDim.x + threadIdx.x;
        tidy=blockIdx.y*blockDim.y + threadIdx.y;
        int h;


        h = tidx * width + tidy;

        //printf(" %d ", h);
        if(tidx < height && tidy < width){
            //using the euclidean distance for the magnitude
            *(output_mag+h) = (int)(sqrt((float)(Vertical[h]*Vertical[h])+(Horizontal[h]*Horizontal[h])));
        }


}


__global__
void direction(int *Horizontal, int *Vertical, int * output_grad, int height, int width){

        int tidx, tidy;
        tidx=blockIdx.x*blockDim.x + threadIdx.x;
        tidy=blockIdx.y*blockDim.y + threadIdx.y;
        int h;


        h = tidx * width + tidy;

        //printf(" %d ", (int)(atan2((float)Horizontal[h], (float)Vertical[h])));
        if(tidx < height && tidy < width){
            *(output_grad+h) = (int)(atan2((float)Horizontal[h], (float)Vertical[h]));

        }


}

