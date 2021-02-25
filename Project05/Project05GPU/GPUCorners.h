/* 
Header file for the Corners functon
*/

#include <cuda.h>

/*
__global__

//Function:
void cornersList(int* vertical, int* horizontal, int* conerness, int height, int width, int window){

    int tidx, tidy;

    tidx = blockIdx.x * blockDim.x + threadIdx.x;
    tidy = blockIdx.y * blockDim.y + threadIdx.y;


    int  e, k, ixx, iyy, ixiy;


    ixx = 0;
    iyy = 0;
    ixiy = 0;
    for( e = -window/2; e < window/2; e++){
        for( k = -window/2; k < window/2; k++){
            if(i+e < height && i+e > 0 && j+k < width && j+k > 0){
                ixx += pow(vertical[(i+e)*width +(j+k)], 2);
                iyy += pow(horizontal[(i+e)*width +(j+k)], 2);
                ixiy += vertical[(i+e)*width +(j+k)] * horizontal[(i+e)*width +(j+k)];
                
            }
        }
    }
    cornerness[i*width+j] = (ixx * iyy - ixiy * ixiy) - .4*(ixx+iyy)*(ixx + iyy);

}
*/

__global__

void compute_cornerness(float * Ix, float* Iy,int h,int w,int window, float* corners){

    int k, m;

    //get the local and global thread IDs
    int lix = threadIdx.x;
    int liy = threadIdx.y;
    int gix = blockIdx.x * blockDim.x + threadIdx.x;
    int giy = blockIdx.y * blockDim.y + threadIdx.y;


    //set up the shared memory
    extern __shared__ float IxS[];
    IxS[lix*blockDim.y+liy] = Ix[gix*w+giy];  
    __syncthreads();
    extern __shared__ float IyS[];
    IyS[lix*blockDim.y+liy] = Iy[gix*w+giy];   
    __syncthreads();
    float ixx = 0, iyy = 0, ixy = 0;

    for(k=-window/2; k < window/2; k++){
        for(m = -window/2; m < window/2; m++){
            if( (lix+k) >=0 && (lix+k) < blockDim.x && (liy+m) >=0 && (liy+m) < blockDim.y){
                ixx += IxS[(lix + k)*blockDim.y + (liy + m)] * IxS[(lix + k)*blockDim.y + (liy + m)];
                iyy += IyS[(lix + k)*blockDim.y + (liy + m)] * IyS[(lix + k)*blockDim.y + (liy + m)];
                ixy += IxS[(lix + k)*blockDim.y + (liy + m)] * IyS[(lix + k)*blockDim.y + (liy + m)];
        }
            else{
                if( (gix+k) >=0 && (gix+k) < h && (giy+m) >=0 && (giy+m) < w){
                    ixx += Ix[(gix + k)*blockDim.y + (giy + m)] * Ix[(gix + k)*blockDim.y + (giy + m)];
                    iyy += Iy[(gix + k)*blockDim.y + (giy + m)] * Iy[(gix + k)*blockDim.y + (giy + m)];
                    ixy += Ix[(gix + k)*blockDim.y + (giy + m)] * Iy[(gix + k)*blockDim.y + (giy + m)];
                }
            }
        }
    }
    float det = (ixx*iyy) - (ixy*ixy);
    float trace = ixx + iyy;
    *(corners+(gix * w)+giy) = det - .4*trace*trace;
    __syncthreads();


}


__global__ 
void corners(float* corner, int h, int w, float* indices){

    int stride;
    int local_index;
    //get the local and global thread IDs
    int lix = threadIdx.x;
    int liy = threadIdx.y;
    int gix = blockIdx.x * blockDim.x + threadIdx.x;
    int giy = blockIdx.y * blockDim.y + threadIdx.y;  

         //set up the shared memory
    extern __shared__ float Shared[];
    __shared__ float* cornerS, *globalS;
    
    cornerS = Shared;
    globalS = Shared + (blockDim.x *blockDim.y);
    cornerS[lix *blockDim.y + liy] = corner[gix * w + giy];
    globalS[lix*blockDim.y+liy] = (gix*w+giy); 
    __syncthreads();

    for(stride = ((blockDim.x*blockDim.y)/2); stride >= 1; stride = stride/2 ){
        __syncthreads();
        local_index= lix * blockDim.y + liy; 

        if(local_index < stride){
            globalS[local_index] = cornerS[local_index] > cornerS[local_index + stride] ? globalS[local_index] : globalS[local_index + stride];

            if(cornerS[local_index] > cornerS[local_index +stride]){
                cornerS[local_index] = cornerS[local_index];
            }
            else{
                cornerS[local_index] = cornerS[local_index + stride];
            }
        }
        if( lix == 0 && liy == 0){
            //int i = local_index/w;
            //int j = local_index%w;
            indices[blockIdx.x*gridDim.y + blockIdx.x] = globalS[0];
        }
    }

}
