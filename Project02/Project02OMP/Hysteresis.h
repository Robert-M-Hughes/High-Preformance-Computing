/*
This will be the header file for the Hysteresis

*/

#include "stdio.h" 
#include "math.h"
#include "stdlib.h"
#include "string.h"



//function definition
int* hysteresis(int* hysteresis, int height, int width);
int cmpfunc(const void * a, const void * b);

//Function 

int cmpfunc (const void * a, const void * b) {
   return ( *(int*)a - *(int*)b );
}

int* hysteresis(int* suppressed, int height, int width){


    int* hysteresisIm= malloc(height*width * sizeof(int)); 
    memcpy(hysteresisIm, suppressed, sizeof(int)*height*width);

    int* list= malloc(height*width * sizeof(int)); 
    memcpy(list, suppressed, sizeof(int)*height*width);


    qsort(list, height* width, sizeof(int), cmpfunc);

    int loc = floor(.95*(height * width));
    int t_hi = list[loc];
    int t_lo = floor(.5*t_hi);

    int i;
    int j;
#pragma omp parallel for private(j)
    for(i = 0; i < height; i++){
        for(j = 0; j < width; j++){
            if( hysteresisIm[i*width +j] > t_hi ){
                hysteresisIm[i*width +j] = 255;
            }
            else if (hysteresisIm[i*width +j] >t_lo){
                hysteresisIm[i*width +j] = 125;
            }
            else{
                hysteresisIm[i*width +j] = 0;
            }
        }
    }

//printf("Leaving hysteresis\n");
return hysteresisIm;

}