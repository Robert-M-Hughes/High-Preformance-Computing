/*
This will be the header file for the Non-Maximal Suppression

*/

#include "stdio.h" 
#include "math.h"
#include "stdlib.h"
#include "string.h"
#include <cuda.h>


#define PI 3.14159 //--- Don't need becaust eh math library already incldes the value for pi


__global__
void suppression(float* gxy, float* iangle, int height, int width, float* suppressionIm){
    //printf("Supression hit\n");
    //int* suppressionIm = malloc(height*width * sizeof(int));
    //printf("New Suppression made\n");

    double theta;


    int tidx, tidy;
    tidx=blockIdx.x*blockDim.x + threadIdx.x;
    tidy=blockIdx.y*blockDim.y + threadIdx.y;
    int h;

    //printf(" %d ", suppressionIm[tidx*width+tidy]);

    theta = iangle[tidx*width + tidy];
    //make sure the value is positive
    //printf("in the loop for suppression");
    if(theta < 0){
        theta = theta + PI;
    }
    //need to conver to degrees for sake of ease
    theta =    theta* (180.0 / PI);


    if(theta <= 22.5 || theta > 157.5){ //vertical  n-s
        if(tidx+1 < height && tidx-1  >0){//bound check
            if(gxy[tidx*width +tidy] < gxy[(tidx+1)*width +tidy] || gxy[tidx*width +tidy] < gxy[(tidx-1)*width +tidy]){
                suppressionIm[tidx*width + tidy] = 0;
            }
        }
    }
    else if(theta > 22.5 && theta <=67.5){//nw - se 
        if(tidx-1 > 0 && tidy-1 > 0 && tidx+1 < height && tidy+1 < width){//bounds
            if(gxy[tidx*width +tidy] < gxy[(tidx-1)*width +tidy -1] || gxy[tidx*width +tidy] < gxy[(tidx+1)*width +tidy +1]){
                suppressionIm[tidx*width + tidy] = 0;

            }
        }
    }

    else if(theta >= 67.5 && theta < 112.5){// horizontal e-w
        if(tidy + 1 < width && tidy-1 > 0){//bounds
            if(gxy[tidx*width +tidy] < gxy[tidx*width +tidy+1] || gxy[tidx*width +tidy] < gxy[tidx*width +tidy-1]){
                suppressionIm[tidx*width + tidy] = 0;
            }
        }
    }

    else if(theta > 112.5 && theta <= 157.5){// ne-sw
        if(tidx -1 > 0 && tidy-1 > 0 && tidx+1 < height && tidy+1 < width){//bounds
            if(gxy[tidx*width +tidy]  < gxy[(tidx-1)*width +tidy+1] || gxy[tidx*width +tidy]  < gxy[(tidx+1)*width + tidy-1] ){
                suppressionIm[tidx*width + tidy] = 0;
            }
        }
    
    }
    
   

}
