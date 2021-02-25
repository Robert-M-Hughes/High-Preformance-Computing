/*
This will serve as the file that has all the code to preform the convolution
Author: Robert Hughes

*/

#include "stdio.h" 
#include "stddef.h"
#include "math.h"
#include "stdlib.h"
#include "string.h"

//Function declaration 
double * convolveVert(int *input_image, double* kernel, int w, int height, int width );
double * convolveHoriz(int* input_image, double* kernel, int w, int height, int width);


//Function

double * convolveHoriz(int* input_image, double* kernel, int w, int height, int width){
    printf("reached horizontal convolution\n");
   // printf("height %d", height);

   double * array = malloc(height*width*sizeof(double));

    for(int i = 0; i < height; i++){
        //printf("i %d\n");
        for(int j = 0; j < width; j++){
            int sum = 0;
            int temp_loc = i*width+j;
            for(int x = 0; x < w; x++){
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
                
            }

            array[temp_loc] = (sum);
            //printf("%f ", sum);
        }
        //printf("\n");
    }
    printf("done\n");


    
    return array;
    free(array);
}

double * convolveVert(int* input_image, double* kernel, int w, int height, int width){
    printf("reached Vertical convolution\n");
    
    double * array = malloc(height*width*sizeof(double));
    for(int i = 0; i < height; i++){
        //printf("i %d\n");
        for(int j = 0; j < width; j++){
            int sum = 0;
            int temp_loc = i*width+j;
            for(int x = 0; x < w; x++){
                int offset = (-1*floor(w/2)+x);
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

                        sum += input_image[temp_loc + offset*width] * kernel[x];
                       // printf("hit \n");
                        // printf(" sum %f input %f kernel %f\n", sum, input_image[temp_loc + offset], kernel[x]);
                    }
                
                }
                
            }

            array[temp_loc] = (sum);
            //printf("%f ", sum);
        }
       //printf("\n");
    }
    printf("done\n");
    


    
    return array;
    free(array);
}