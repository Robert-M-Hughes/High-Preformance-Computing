//This will be the c File for program 1
/*
Author: Robert Hughes
This program will be creaed by using the documentation that was given to us in class along with using the code from ecpe 124 that Dr.Pallipuram gave us last semester


*/
#include <stdio.h>
#include <stddef.h>
#include "image.h"
//#include "image_template.h" not for this assignment C cannot handle the templates
#include "Gaussian.h"
#include "convolution.h"
#include <math.h>

//Can use sigma as a constant to run the code
//#define sigma .6

int main( int argc, char** argv ){
//allow Sigma to be put in the command line to be set
float sigma = atof(argv[2]);

//integers for the loops
int i =0;
int j =0;

//get the size of the kernel so we can store it after the functions are preformed
int w = 2*(round(2.5*sigma -.5))+1;
//printf("w= %d \n", w);
//allocate these according to the width of the kernel
double * gauss = malloc(w * sizeof(double));
double * gaussDeriv = malloc(w * sizeof(double));
/*
used these to test the convolution because they are easier to check the work to make sure that the function is behaving correctly
double temp[] = {0, 1, 2, 3, 
                4, 5, 6, 7,
                8, 9, 10, 11, 
                12, 13, 14, 15};

double kernel[] = {.25, .5, .25};
*/
//pre-define these so that we can pull the image in using the library provided to us
int height, width;
int *image;
//printf("Hello\n");

//calculate the two kernels we need to run the program
gauss = gaussianKernel(sigma);
gaussDeriv = gaussianDerivKernel(sigma);

read_image_template(argv[1], &image, &width, &height);


/*
printf("%d\n", height);
printf("%d\n", width);
for(int i = 0; i < height*width; i++){
    printf("%d\n", image[i]);
}
*/
//w=3;
//int k_height = 4;
//int k_width = 4;


//free( array );
//free( otherarray );

//Flip the Gaussian Derivative Kernel since it is not symetric
double * flip_gaussDeriv = malloc(w * sizeof(double));

for(i = w-1, j = 0; i >= 0; i--, j++){

flip_gaussDeriv[j] = gaussDeriv[i];
    //printf("%d \n", w);
}
//printf("Flipped Gaussian Derivative: ");
    for(j = 0; j < w; j++){
  //      printf("%f ", flip_gaussDeriv[j]);

    }
  //  printf("\n");
free(gaussDeriv);//we can now free the gaussian derivative kernel since we have the new flipped kernel





//*****Section to compute the Grandient and Magnitude********

//preform the convolutions


//compute the horizontal and vertical gradients
//make space for the arrays

int * Vertical = malloc(height*width * sizeof(int));
int * Horizontal = malloc(height*width * sizeof(int));
//tempV = convolveVert( image, gauss, w, height, width );

// create characer arrays to name the files for when we use the image write
char tempVer[] = "temp_vertical";
char tempHor[] = "temp_horizontal";
char ver[] = "vertical";
char hor[] = "horizontal";
char grad[] = "gradient";
char mag[] = "magnitude";

//Vertical
Vertical = convolveHoriz(image, gauss, w, height, width );
write_image_template(tempVer, Vertical, width, height);

Vertical = convolveVert(Vertical, flip_gaussDeriv, w, height, width);
write_image_template(ver, Vertical, width, height);



//Horizontal
Horizontal = convolveVert(image, gauss, w, height, width);
write_image_template(tempHor, Horizontal, width, height);

Horizontal = convolveHoriz(Horizontal, flip_gaussDeriv, w, height, width);
write_image_template(hor, Horizontal, width, height);



free(flip_gaussDeriv);
free(gauss);

//****Calculate the Maginitude and Gradient****
//allocate the memory
int * Magnitude = malloc(height*width * sizeof(int));
int * Gradient = malloc(height*width * sizeof(int));

//can put these two together in one loop to try to increase the speed, could preform loop unrolling however it is trivial and we are going to use openMP in the future anyway

for(i = 0; i < height*width; i++){
Gradient[i] = atan2(Horizontal[i], Vertical[i]);
//using the euclidean distance for the magnitude  ----- if I chage it to have local variables I can increase the use of the cache to make the program run faster
Magnitude[i] = sqrt((Vertical[i]*Vertical[i])+(Horizontal[i]*Horizontal[i]));

}
free(Vertical);
free(Horizontal);
//write out te files
write_image_template(mag, Magnitude, width, height);
write_image_template(grad, Gradient, width, height);
free(Magnitude);
free(Gradient);


/*debugger code
for(int i = 0; i < height*width; i++){
    printf("%f", Magnitude[i]);
}
*/
//printf("finished!\n");



printf("Program Executed: Magnitude and Gradient Images found.\n");
    return 0;
}
