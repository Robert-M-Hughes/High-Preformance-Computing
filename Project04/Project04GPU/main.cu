//This will be the c File for program 1
/*
Author: Robert Hughes
This program will be creaed by using the documentation that was given to us in class along with using the code from ecpe 124 that Dr.Pallipuram gave us last semester


*/
#include <stdio.h>
#include <stddef.h>
#include <time.h>
#include <math.h>

#include "Gaussian.h"
#include "image.h"
#include "GPUconvolution.h"
#include "testConvolve.h"
//#include "image_template.h" not for this assignment C cannot handle the templates
#include <cuda.h>
//#include "Gaussian.h"

//Function Definition


//Function to call to get the Gaussian Kernel 


//Can use sigma as a constant to run the code
//#define sigma .6
#define WINDOW 7

int main( int argc, char** argv ){

    int block_dim = 16;
    
    
//set up the omp environment
//printf ("The number of processors is: %d\n", num_procs);
//omp_set_num_threads(atof(argv[3]));
//printf ("The number of threads is: %f\n", atof(argv[3]));


//allow Sigma to be put in the command line to be set
clock_t start, end;
start = clock();
double cpu_time_used;
//char filename[] = "Corners.csv";

//FILE *fp;
    int *temp_hor;
    int *temp_ver;
    int *d_image;
    int *d_temp_hor;
    int *d_temp_ver;
    double *d_kernel;
    int *d_vert;
    int *d_horiz;
    int *d_grad;
    int *d_mag;

    double *flip_gaussDeriv;    
    double *d_derivKernel;






float sigma = atof(argv[2]);

//integers for the loops
int i =0;
int j =0;

//get the size of the kernel so we can store it after the functions are preformed
int w = 2*(round(2.5*sigma -.5))+1;
//printf("w= %d \n", w);
//allocate these according to the width of the kernel
/*
used these to test the convolution because they are easier to check the work to make sure that the function is behaving correctly
double temp[] = {0, 1, 2, 3, 
                4, 5, 6, 7,
                8, 9, 10, 11, 
                12, 13, 14, 15};

double kernel[] = {.25, .5, .25};
*/
//pre-define these so that we can pull the image in using the library provided to us
int height;
int width;
int *image;

//calculate the two kernels we need to run the program
double *gauss;// = (double*)malloc(sizeof(double)*w);
double *gaussDeriv;// = (double*)malloc(sizeof(double)*w);


gauss = gaussianKernel(sigma);

gaussDeriv = gaussianDerivKernel(sigma);

//printf("Hello\n");

read_image_template(argv[1], &image, &width, &height);


/*
printf("%d\n", height);
printf("%d\n", width);
for(int i = 0; i < height*width; i++){
    printf("%d\n", image[i]);
}
*/
//w=3;


//free( array );
//free( otherarray );

//Flip the Gaussian Derivative Kernel since it is not symetric
flip_gaussDeriv =(double * ) malloc(w * sizeof(double));

//#pragma omp parallel for{
    for(i = w-1, j = 0; i >= 0; i--, j++){

    flip_gaussDeriv[j] = gaussDeriv[i];
        //printf("%d \n", w);
    }
//}
printf("Flipped Gaussian Derivative: ");

for(j = 0; j < w; j++){

        printf("%f ", flip_gaussDeriv[j]);

    }
    printf("\nGauss: ");
    for(j = 0; j < w; j++){
    
        printf("%f ", gauss[j]);

    }
    printf("\n");
  
free(gaussDeriv);//we can now free the gaussian derivative kernel since we have the new flipped kernel


dim3 dmBlock(block_dim, block_dim, 1);
dim3 dmGrid(ceil(height/block_dim), ceil(width/block_dim), 1);


//*****Section to compute the Grandient and Magnitude********

//preform the convolutions
    // Malloc for DEVICE GPU
    cudaMalloc((void **)&d_image,sizeof(int)*width*height);
    cudaMalloc((void **)&d_kernel,sizeof(double)*w);
    cudaMalloc((void **)&d_derivKernel,sizeof(double)*w);

    cudaMalloc((void **)&d_temp_hor,sizeof(int)*width*height);
    cudaMalloc((void **)&d_horiz,sizeof(int)*width*height);
    cudaMalloc((void **)&d_temp_ver,sizeof(int)*width*height);
    cudaMalloc((void **)&d_vert,sizeof(int)*width*height);

    cudaMalloc((void **)&d_kernel,sizeof(double)*w);
\

    cudaMalloc((void **)&d_grad,sizeof(int)*width*height);
    cudaMalloc((void **)&d_mag,sizeof(int)*width*height);

    //copy the items from CPU to GPU
    cudaMemcpy(d_image, image,sizeof(int)*width*height,cudaMemcpyHostToDevice);
    cudaMemcpy(d_kernel, gauss,sizeof(int)*w,cudaMemcpyHostToDevice);
    cudaMemcpy(d_derivKernel, flip_gaussDeriv,sizeof(int)*w,cudaMemcpyHostToDevice);


//compute the horizontal and vertical gradients
//make space for the arrays
int *Vertical;
int *Horizontal;

Vertical = (int*)malloc(height*width * sizeof(int));
Horizontal = (int*)malloc(height*width * sizeof(int));
//tempV = convolveVert( image, gauss, w, height, width );

// create characer arrays to name the files for when we use the image write
char tempVer[] = "temp_vertical";
char tempHor[] = "temp_horizontal";
char ver[] = "vertical";
char hor[] = "horizontal";
char grad[] = "gradient";
char mag[] = "magnitude";

//Horiz
convolveVert<<<dmGrid,dmBlock>>>(d_image, d_kernel, w, height,width, d_horiz);	
//cudaMemcpy(Horizontal, d_temp_hor,sizeof(int)*height*width,cudaMemcpyDeviceToHost);

convolveHoriz<<<dmGrid,dmBlock>>>(d_horiz,d_derivKernel, w, height,width, d_horiz);	
cudaMemcpy(Horizontal, d_horiz,sizeof(int)*height*width,cudaMemcpyDeviceToHost);
for(i = 0; i < height * width; i++){
    printf("%d ", Horizontal[i]);
 }
write_image_template(hor, Horizontal, width, height);


/*

//Vertical
convolveVert<<<dmGrid,dmBlock>>>(d_image,d_kernel, w, height, width,d_temp_ver);
cudaMemcpy(Vertical, d_temp_hor,sizeof(int)*height*width,cudaMemcpyDeviceToHost);

convolveVert<<<dmGrid,dmBlock>>>(d_temp_hor, d_derivKernel, w, height,width, d_vert); 
cudaMemcpy(Vertical, d_vert,sizeof(int)*height*width,cudaMemcpyDeviceToHost);

write_image_template(ver, Vertical, width, height);
*/

/*

//****Calculate the Maginitude and Gradient****
//allocate the memory
int * Magnitude = malloc(height*width * sizeof(int));
int * Gradient = malloc(height*width * sizeof(int));

//can put these two together in one loop to try to increase the speed, could preform loop unrolling however it is trivial and we are going to use openMP in the future anyway
for(i = 0; i < height*width; i++){
    Gradient[i] = atan2(Horizontal[i], Vertical[i]);
    //using the euclidean distance for the magnitude
    Magnitude[i] = sqrt((Vertical[i]*Vertical[i])+(Horizontal[i]*Horizontal[i]));

}

//write out the files

write_image_template(mag, Magnitude, width, height);
write_image_template(grad, Gradient, width, height);


//printf("Done first part\n");

//preform the non-max suppression
char sup[] = "Suppression";


int * edges = malloc(height*width * sizeof(int));
edges = suppression(Magnitude, Gradient, height, width);
//printf("Finished the supression\n");

write_image_template(sup, edges, width, height);


//preform the hysteresis
char hys[] = "Hyteresis";

edges = hysteresis(edges, height, width);
//printf("Finished the hyteresis\n");

write_image_template(hys, edges, width, height);


//preform the edge linking
char edg[] = "Edges";

edges = edgeLinking(edges, height, width);
//printf("Finished the edge linking\n");

write_image_template(edg, edges, width, height);

int * corners = malloc(height*width * sizeof(int));
corners = cornersList(Vertical, Horizontal, height, width, WINDOW);
write_image_template("corners", corners, width, height);


/*
fp = fopen(filename, "w+");
fprintf(fp, "Corners");
for(i = 0; i < atof(argv[3]); i ++){
    fprintf(fp, "\n%d", i+1);
}
*/
/*debugger code
for(int i = 0; i < height*width; i++){
    printf("%f", Magnitude[i]);
}
*/
//printf("finished!\n");
free(Vertical);
free(Horizontal);
free(flip_gaussDeriv);
free(gauss);
/*
free(Magnitude);
free(Gradient);
free (corners);
free(edges);
*/


//printf("Program Executed: Egde found.\n");
end = clock();
cpu_time_used = ((double)(end - start))/ CLOCKS_PER_SEC;
//printf("Time taken to execute: %f\n", cpu_time_used);
//printf("%s, %f, %f \n", argv[3], sigma, cpu_time_used);


    return 0;
}
