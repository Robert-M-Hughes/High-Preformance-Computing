

#include <stdio.h>
#include <stddef.h>
#include <time.h>
#include <omp.h>
#include <math.h>
#include <cuda.h>
#include "image.h"

#include "testConvolve.h"
#include "GPUMagDir.h"
//#include "image_template.h" not for this assignment C cannot handle the templates



//Can use sigma as a constant to run the code
//#define sigma .6

int main(int argc, char **argv)
{

    int height;
    int width;
    int *vert;
    int *horiz;
    int *mag;
    int *grad;

    //read_image_template(argv[1], &vert, &width, &height);
    //read_image_template(argv[2], &horiz, &width, &height);


    


    int block_dim = 16;

    double kernel[] = {.25, .5, .25 };
    int k_width = 3;
    
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
    
    //Malloc for Host 
    temp_hor = (int*)malloc(sizeof(int)*height*width);
    //horizontal_grad = (int*)malloc(sizeof(int)*height*width);
    temp_ver = (int*)malloc(sizeof(int)*height*width);
    //vertical_grad = (int*)malloc(sizeof(int)*height*width);

    mag = (int*)malloc(sizeof(int)*height*width);
    grad = (int*)malloc(sizeof(int)*height*width);





    // Malloc for DEVICE GPU
    cudaMalloc((void **)&d_image,sizeof(int)*width*height);
    cudaMalloc((void **)&d_temp_hor,sizeof(int)*width*height);
    //cudaMalloc((void **)&d_horizontal_grad,sizeof(int)*width*height);
    cudaMalloc((void **)&d_temp_ver,sizeof(int)*width*height);
    //cudaMalloc((void **)&d_vertical_grad,sizeof(int)*width*height);

    cudaMalloc((void **)&d_kernel,sizeof(double)*k_width);
    //cudaMalloc((void **)&d_deriv_kernel,sizeof(int)*k_width);


    cudaMalloc((void **)&d_vert,sizeof(int)*width*height);
    cudaMalloc((void **)&d_horiz,sizeof(int)*width*height);
    cudaMalloc((void **)&d_grad,sizeof(int)*width*height);
    cudaMalloc((void **)&d_mag,sizeof(int)*width*height);

    //copy the items from CPU to GPU
    cudaMemcpy(d_vert, vert,sizeof(int)*width*height,cudaMemcpyHostToDevice);
    cudaMemcpy(d_horiz, horiz,sizeof(int)*width*height,cudaMemcpyHostToDevice);
    //cudaMemcpy(d_deriv_kernel,deriv_kernel,sizeof(int)*k_width,cudaMemcpyHostToDevice);


    //Horizontal
    //int block_dim=atof(argv[3]);
    //int block_dim = 32;
    dim3 dmBlock(block_dim, block_dim, 1);
    dim3 dmGrid(ceil(height/block_dim), ceil(width/block_dim), 1);

    //printf("Height: %d\n", height);
    //printf("Width: %d\n", width);


    magnitude<<<dmGrid,dmBlock>>>(d_horiz, d_vert, d_mag, height, width);	
    cudaMemcpy(mag, d_mag,sizeof(int)*width*height,cudaMemcpyDeviceToHost);

    direction<<<dmGrid,dmBlock>>>(d_horiz, d_vert, d_grad, height, width);	
    cudaMemcpy(grad, d_grad,sizeof(int)*width*height,cudaMemcpyDeviceToHost);

    //convoultion<<<dmGrid,dmBlock>>>(d_temp_hor,d_horizontal_grad,d_deriv_kernel,height,width,1,k_width); 

    printf("End of Func");
    int i;
    for(i = 0; i < height * width; i++){
       // printf("%d ", grad[i]);
    }
    printf("\n\n\n\n");
    for(i = 0; i < height * width; i++){
        //printf("%d ", mag[i]);
    }
    write_image_template("mag", mag, width, height);
    write_image_template("grad",grad, width, height);


    cudaDeviceSynchronize();

    return 0;




}

