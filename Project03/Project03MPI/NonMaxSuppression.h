/*
This will be the header file for the Non-Maximal Suppression

*/

#include "stdio.h" 
#include "math.h"
#include "stdlib.h"
#include "string.h"


#define PI 3.14159 //--- Don't need becaust eh math library already incldes the value for pi

//Funtion Declaration
int * suppression(int* gxy, int* iangle, int height, int width);
double to_degrees(double radians);

//Function call 

double to_degrees(double radians) {
    //printf("Change to Degrees: %f\n", radians * (180.0 / PI));

    return (radians * (180.0 / PI));

}

int * suppression(int* gxy, int* iangle, int height, int width){
    //printf("Supression hit\n");
    int* suppressionIm = malloc(height*width * sizeof(int));
    //printf("New Suppression made\n");
    int i;
    int j;
    double theta;


    memcpy(suppressionIm, gxy, sizeof(int)*height*width);
    /*
    for(i = 0; i < height; i++ ){
        for(j = 0; j < width; j++){
            suppressionIm[i*width + j] += gxy[i*width + j];
        }
    }

*/
    //printf("GXY copied\n");
/*

    for(i = 0; i < height; i++ ){
        for(j = 0; j < width; j++){
            printf("GXY: %d SUP: %d\n", gxy[i*width + j], suppressionIm[i*width + j]);
        }
    }
*/
    #pragma omp parallel for private(j)
    for(i = 0; i < height; i++ ){
        for(j = 0; j < width; j++){
            theta = iangle[i*width + j];
            //make sure the value is positive
            //printf("in the loop for suppression");
            if(theta < 0){
                theta = theta + PI;
            }
            //need to conver to degrees for sake of ease
            theta = to_degrees(theta);

            if(theta <= 22.5 || theta > 157.5){ //vertical  n-s
                if(i+1 < height && i-1  >0){//bound check
                    if(gxy[i*width +j] < gxy[(i+1)*width +j] || gxy[i*width +j] < gxy[(i-1)*width +j]){
                        suppressionIm[i*width + j] = 0;
                    }
                }
            }
            else if(theta > 22.5 && theta <=67.5){//nw - se 
                if(i-1 > 0 && j-1 > 0 && i+1 < height && j+1 < width){//bounds
                    if(gxy[i*width +j] < gxy[(i-1)*width +j -1] || gxy[i*width +j] < gxy[(i+1)*width +j +1]){
                        suppressionIm[i*width + j] = 0;

                    }
                }
            }

            else if(theta >= 67.5 && theta < 112.5){// horizontal e-w
                if(j + 1 < width && j-1 > 0){//bounds
                    if(gxy[i*width +j] < gxy[i*width +j+1] || gxy[i*width +j] < gxy[i*width +j-1]){
                        suppressionIm[i*width + j] = 0;
                    }
                }
            }

            else if(theta > 112.5 && theta <= 157.5){// ne-sw
                if(i -1 > 0 && j-1 > 0 && i+1 < height && j+1 < width){//bounds
                    if(gxy[i*width +j]  < gxy[(i-1)*width +j+1] || gxy[i*width +j]  < gxy[(i+1)*width + j-1] ){
                        suppressionIm[i*width + j] = 0;
                    }
                }
            }

        }
        //printf("At: %d\n", i*width +j);
    }
    
    return suppressionIm;

}
