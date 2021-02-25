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
int * convolveVert(int *input_image, double* kernel, int w, int height, int width );
int * convolveHoriz(int* input_image, double* kernel, int w, int height, int width);
void * dataNormalize(int * data, int height, int width);


//Function

int * convolveHoriz(int* input_image, double* kernel, int w, int height, int width){
  //  printf("reached horizontal convolution\n");
   // printf("height %d", height);

   int * array = malloc(height*width*sizeof(int));
    int i, j, x;
    //#pragma omp parallel for private(j,x)
    for(i = 0; i < height; i++){
        //printf("i %d\n");
        for(j = 0; j < width; j++){
            float sum = 0;
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
                
            }

            array[temp_loc] = (int)(sum);
            //printf("%f ", sum);
        }
        //printf("\n");
    }
   //printf("done\n");


    //dataNormailize(array, height, width);
   
    return array;
    free(array);
}

int * convolveVert(int* input_image, double* kernel, int w, int height, int width){
    //printf("reached Vertical convolution\n");
    int i,j,x;
    //#pragma omp parallel for private(j,x)
    int * array = malloc(height*width*sizeof(int));
    for(i = 0; i < height; i++){
        //printf("i %d\n");
        for(j = 0; j < width; j++){
            float sum = 0;
            int temp_loc = i*width+j;
            for(x = 0; x < w; x++){
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

            array[temp_loc] = (int)(sum);
            //printf("%f ", sum);
        }
       //printf("\n");
    }
   // printf("done\n");
    
    //dataNormailize(array, height, width);

    
    return array;
    free(array);
}

//do not need to use the data because the gaussian kernel is the normalization
void dataNormailize(int * data, int height, int width){
printf("Normalizing the Data...\n");
int min = 0;
int max = 0;
int i;
for(i =0; i < height* width; i++){
    if (data[i] < min){
        min = data[i];
    }
    else if(data[i] > max){
        max = data[i];
    }
}
printf("Min: %d\nMax: %d\n", min, max);

for(i = 0; i < height*width; i++){
    data[i] = (255-0)/(max-min)*(data[i]-max)+255;//normalize the data 0 - 255
}
printf("Data has been Normalized!\n");


}
