

#include <stdio.h>
#include <stddef.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>

#include <cuda.h>
#include <cuda_runtime_api.h>

#include <thrust/device_vector.h>
#include <thrust/host_vector.h>
#include <thrust/sort.h>
#include <thrust/copy.h>

#include "image_template.h"
#include "Gaussian.h"
#include "GPUconvolution.h"
#include "GPUMagDir.h"
#include "GPUNonMaxSuppression.h"
#include "GPUHysteresis.h"
#include "GPUEdgeLinking.h"
#include "GPUCorners.h"
//#include "image_template.h" not for this assignment C cannot handle the templates


#define WINDOW 7

//Can use sigma as a constant to run the code
//#define sigma .6

int main(int argc, char **argv)
{




    struct timeval start_total, end_total, start_image_read, end_image_read, start_edge_write, 
    end_edge_write, start_memCpy, end_memCpy, start_hor, end_hor, start_ver, end_ver, start_mag,
    end_mag, start_dir, end_dir, start_sort, end_sort, start_supp, end_supp, start_edge, end_edge,
    start_hys, end_hys, start_memCpydevice, end_memCpydevice;

  	//gettimeofday(&start_ete, NULL);

    int i, j;
    int height;
    int width;
    float *image;

    float sigma = atof(argv[2]);
    int w = 2*(round(2.5*sigma -.5))+1;

    gettimeofday(&start_image_read, NULL);
        read_image_template(argv[1], &image, &width, &height);
    gettimeofday(&end_image_read, NULL);

	gettimeofday(&start_total, NULL);

    double *gauss;// = (double*)malloc(sizeof(double)*w);
    double *gaussDeriv;// = (double*)malloc(sizeof(double)*w);
    double *flip_gaussDeriv;

    gauss = gaussianKernel(sigma);
    gaussDeriv = gaussianDerivKernel(sigma);
    flip_gaussDeriv =(double * ) malloc(w * sizeof(double));

//#pragma omp parallel for{
    for(i = w-1, j = 0; i >= 0; i--, j++){

        flip_gaussDeriv[j] = gaussDeriv[i];
        //printf("%d \n", w);
    }

    /*
    image = (int*)malloc(sizeof(int)*height*width);
    int j;
    for (j = 0; j < height *width; j++){
        image[j] = j;
        printf("%d ", image[j]);


    }
    printf("\n");
*/
    int block_dim;
    /*
    if(atoi(argv[3]) == 8 || atoi(argv[3]) == 16 ||atoi(argv[3]) == 32)
        block_dim = atoi(argv[3]);

    else
        block_dim = 16;
        */
        block_dim = atoi(argv[3]);
    //double kernel[] = {0.166379, 0.667243, 0.166379  };
    //double dkernel[] = {-.5, -0 , .5};
    int k_width = w;
    
    float *temp_hor;
    float *temp_ver;
    float *d_image;
    float *d_temp_hor;
    float *d_temp_ver;
    double *d_kernel;
    double *d_dkernel;
    float *d_grad;
    float *d_mag;
    float *temp_grad;
    float* temp_mag;
    float* d_edge;
    float* temp_edge;
    float* d_corn;
    float* temp_corn;
    float* temp_indices;
    float* d_indices;
    //Malloc for Host 
    temp_hor = (float*)malloc(sizeof(float)*height*width);
    //horizontal_grad = (int*)malloc(sizeof(int)*height*width);
    temp_ver = (float*)malloc(sizeof(float)*height*width);
    temp_grad = (float*)malloc(sizeof(float)*height*width);
    temp_mag = (float*)malloc(sizeof(float)*height*width);
    temp_edge = (float*)malloc(sizeof(float)*height*width);
    temp_corn = (float*)malloc(sizeof(float)*height*width);
    temp_indices = (float*)malloc(sizeof(float)*height*width);


    // Malloc for DEVICE GPU
    cudaMalloc((void **)&d_image,sizeof(int)*width*height);
    cudaMalloc((void **)&d_temp_hor,sizeof(float)*width*height);
    //cudaMalloc((void **)&d_horizontal_grad,sizeof(float)*width*height);
    cudaMalloc((void **)&d_temp_ver,sizeof(float)*width*height);
    
    cudaMalloc((void **)&d_grad,sizeof(float)*width*height);
    cudaMalloc((void **)&d_mag,sizeof(float)*width*height);
    cudaMalloc((void **)&d_edge,sizeof(float)*width*height);
    cudaMalloc((void **)&d_corn,sizeof(float)*width*height);
    cudaMalloc((void **)&d_indices,sizeof(float)*width*height);




    cudaMalloc((void **)&d_kernel,sizeof(double)*k_width);
    cudaMalloc((void **)&d_dkernel,sizeof(double)*k_width);

    //cudaMalloc((void **)&d_deriv_kernel,sizeof(float)*k_width);

    gettimeofday(&start_memCpy, NULL);
    //copy the items from CPU to GPU
    cudaMemcpy(d_image, image,sizeof(int)*width*height,cudaMemcpyHostToDevice);
    cudaMemcpy(d_kernel, gauss,sizeof(double)*k_width,cudaMemcpyHostToDevice);
    cudaMemcpy(d_dkernel, flip_gaussDeriv,sizeof(double)*k_width,cudaMemcpyHostToDevice);
    gettimeofday(&end_memCpy, NULL);
    //cudaMemcpy(d_deriv_kernel,deriv_kernel,sizeof(int)*k_width,cudaMemcpyHostToDevice);


    //Horizontal
    //int block_dim=atof(argv[3]);
    //int block_dim = 32;
    dim3 dmBlock(block_dim, block_dim, 1);
    dim3 dmGrid(ceil(height/block_dim), ceil(width/block_dim), 1);

    //printf("Height: %d\n", height);
    //printf("Width: %d\n", width);


    //horizontal gradient
    gettimeofday(&start_hor, NULL);
/*
    convolveVert<<<dmGrid,dmBlock>>>(d_image,d_kernel, k_width, height,width, d_temp_hor);	
    convolveHoriz<<<dmGrid,dmBlock>>>(d_temp_hor,d_dkernel, k_width, height,width, d_temp_hor);	
*/
    sharedConvol<<<dmGrid, dmBlock,sizeof(float)*block_dim*block_dim>>>(d_image,d_temp_hor, d_kernel, height, width, k_width, 1);//vertical
    sharedConvol<<<dmGrid, dmBlock,sizeof(float)*block_dim*block_dim>>>(d_temp_hor,d_temp_hor, d_dkernel, height, width, 1, k_width);//horizontal


    cudaDeviceSynchronize();

    gettimeofday(&end_hor, NULL);

    //vertical gradient
    gettimeofday(&start_ver, NULL);

/*
    convolveHoriz<<<dmGrid,dmBlock>>>(d_image,d_kernel, k_width, height, width, d_temp_ver);	
    convolveVert<<<dmGrid,dmBlock>>>(d_temp_ver,d_dkernel, k_width, height,width, d_temp_ver);	
*/


    sharedConvol<<<dmGrid, dmBlock,sizeof(float)*block_dim*block_dim>>>(d_image,d_temp_ver, d_kernel, height, width, 1, k_width);//horizontal
    sharedConvol<<<dmGrid, dmBlock,sizeof(float)*block_dim*block_dim>>>(d_temp_ver,d_temp_ver, d_dkernel, height, width, k_width, 1);//vertical
    

    cudaDeviceSynchronize();

    gettimeofday(&end_ver, NULL);




    //copy to be saved

    
    gettimeofday(&start_dir, NULL);
    direction<<<dmGrid, dmBlock>>>(d_temp_hor, d_temp_ver, d_grad, height, width);
    cudaDeviceSynchronize();
    gettimeofday(&end_dir, NULL);

    gettimeofday(&start_mag, NULL);
    magnitude<<<dmGrid, dmBlock>>>(d_temp_hor, d_temp_ver, d_mag, height, width);
    cudaDeviceSynchronize();
    gettimeofday(&end_mag, NULL);

    cudaMemcpy(temp_mag, d_mag,sizeof(float)*width*height,cudaMemcpyDeviceToHost);
    cudaMemcpy(d_edge, temp_mag,sizeof(float)*width*height,cudaMemcpyHostToDevice);
    
    
  

    gettimeofday(&start_supp, NULL);
    suppression<<<dmGrid, dmBlock>>>(d_mag, d_grad, height, width, d_edge);
    cudaDeviceSynchronize();
    gettimeofday(&end_supp, NULL);

    cudaMemcpy(temp_edge, d_edge,sizeof(float)*width*height,cudaMemcpyDeviceToHost);


    gettimeofday(&start_sort, NULL);
    thrust::device_ptr<float> thr_d(d_edge);
	thrust::device_vector<float>d_sup_vec(thr_d,thr_d+(height*width));
    thrust::sort(d_sup_vec.begin(),d_sup_vec.end());
	float index = (float)(.9*height*width);
	float t_hi = d_sup_vec[index];
    float t_lo =t_hi/2;
    gettimeofday(&end_sort, NULL);

   // printf("index = %d\n", index);
    //printf("thi = %d\n", t_hi);
    //printf("tlo = %d\n", t_lo);

    
    gettimeofday(&start_hys, NULL);
    hysteresis<<<dmGrid, dmBlock>>>(d_edge, height, width, t_hi, t_lo);
    cudaDeviceSynchronize();
    gettimeofday(&end_hys, NULL);

    gettimeofday(&start_edge, NULL);
    edgeLinking<<<dmGrid, dmBlock>>>(d_edge, height, width, d_edge );
    cudaDeviceSynchronize();
    gettimeofday(&end_edge, NULL);

  


    compute_cornerness<<<dmGrid, dmBlock,sizeof(float)*block_dim*block_dim>>>(d_temp_hor, d_temp_ver, height, width, WINDOW, d_corn);
    cudaMemcpy(temp_corn, d_corn,sizeof(float)*width*height,cudaMemcpyDeviceToHost);
    cudaDeviceSynchronize();

    float *d_cornerness;
    cudaMalloc((void **)&d_cornerness,sizeof(float)*dmGrid.x*dmGrid.y);
    float* max_corner = (float*)malloc(sizeof(float)*dmGrid.x*dmGrid.y);


    corners<<<dmGrid, dmBlock,2*sizeof(float)*block_dim*block_dim>>>(d_corn, height, width, d_cornerness);
    cudaMemcpy(max_corner, d_cornerness,sizeof(float)*width*height,cudaMemcpyDeviceToHost);
    cudaDeviceSynchronize();

    printf("indices\n");
    for (int e = 0; e < block_dim; e ++){
        printf("%ld, %ld\n", (max_corner[e]/width), ((int)(max_corner[e])%width));

    }

    gettimeofday(&start_memCpydevice, NULL);
    cudaMemcpy(temp_hor, d_temp_hor,sizeof(float)*width*height,cudaMemcpyDeviceToHost);
    cudaMemcpy(temp_ver, d_temp_ver,sizeof(float)*width*height,cudaMemcpyDeviceToHost);
    cudaMemcpy(temp_grad, d_grad,sizeof(float)*width*height,cudaMemcpyDeviceToHost);
    cudaMemcpy(temp_mag, d_mag,sizeof(float)*width*height,cudaMemcpyDeviceToHost);
    cudaMemcpy(temp_edge, d_edge,sizeof(float)*width*height,cudaMemcpyDeviceToHost);
    gettimeofday(&end_memCpydevice, NULL);

    /*
    for(l = 0; l < height * width; l++){
       // printf(" %d ", temp_edge[l]);
    }

*/
    
   // printf("End of Func\n");
/*
    for(i = 0; i < height * width; i++){
       printf("%d ", temp_hor[i]);
    }
    printf("\n");
*/
    gettimeofday(&start_edge_write, NULL);
    write_image_template("ansHor", temp_hor, width, height);
    write_image_template("ansVer", temp_ver, width, height);
    write_image_template("ansMag", temp_mag, width, height);
    write_image_template("ansDir", temp_grad, width, height);
    write_image_template("ansEdg", temp_edge, width, height);
    write_image_template("ansCorn", temp_corn, width, height);
    //write_image_template("ansInd", max_corner, width, height);


    gettimeofday(&end_edge_write, NULL);



    cudaDeviceSynchronize();
    gettimeofday(&end_total, NULL);


    //printf("Total time :%ld\n", ((end_total.tv_sec * 1000000 + end_total.tv_usec) - (start_total.tv_sec * 1000000 + start_total.tv_usec)));
    //printf("Total - File-IO: %ld\n", ((end_total.tv_sec * 1000000 + end_total.tv_usec) - (start_total.tv_sec * 1000000 + start_total.tv_usec) - ((end_image_read.tv_sec * 1000000 + end_image_read.tv_usec) - (start_image_read.tv_sec * 1000000 + start_image_read.tv_usec)) - ((end_edge_write.tv_sec * 1000000 + end_edge_write.tv_usec) - (start_edge_write.tv_sec * 1000000 + start_edge_write.tv_usec))));
    //printf("File IO image read: %ld\n", ((end_image_read.tv_sec * 1000000 + end_image_read.tv_usec) - (start_image_read.tv_sec * 1000000 + start_image_read.tv_usec)));
    //printf("File IO image write: %ld\n", ((end_edge_write.tv_sec * 1000000 + end_edge_write.tv_usec) - (start_edge_write.tv_sec * 1000000 + start_edge_write.tv_usec)));
    //printf("Host to Device: %ld\n", ((end_memCpy.tv_sec * 1000000 + end_memCpy.tv_usec) - (start_memCpy.tv_sec * 1000000 + start_memCpy.tv_usec)));
    //printf("Horizontal Convolution :%ld\n", ((end_hor.tv_sec * 1000000 + end_hor.tv_usec) - (start_hor.tv_sec * 1000000 + start_hor.tv_usec)));
    //printf("Vertical Convolution: %ld\n", ((end_ver.tv_sec * 1000000 + end_ver.tv_usec) - (start_ver.tv_sec * 1000000 + start_ver.tv_usec)));
    //printf("Magnitude Kernel: %ld\n", ((end_mag.tv_sec * 1000000 + end_mag.tv_usec) - (start_mag.tv_sec * 1000000 + start_mag.tv_usec)));
    //printf("Gradient Kernel: %ld\n", ((end_dir.tv_sec * 1000000 + end_dir.tv_usec) - (start_dir.tv_sec * 1000000 + start_dir.tv_usec)));
    //printf("Thrust Sorting: %ld\n", ((end_sort.tv_sec * 1000000 + end_sort.tv_usec) - (start_sort.tv_sec * 1000000 + start_sort.tv_usec)));
    //printf("Suppression Kernel: %ld\n", ((end_supp.tv_sec * 1000000 + end_supp.tv_usec) - (start_supp.tv_sec * 1000000 + start_supp.tv_usec)));
    //printf("Hysteresis Kernel: %ld\n", ((end_hys.tv_sec * 1000000 + end_hys.tv_usec) - (start_hys.tv_sec * 1000000 + start_hys.tv_usec)));
    //printf("Edge Linking Kernel: %ld\n", ((end_edge.tv_sec * 1000000 + end_edge.tv_usec) - (start_edge.tv_sec * 1000000 + start_edge.tv_usec)));
    //printf("Device to Host: %ld\n", ((end_memCpydevice.tv_sec * 1000000 + end_memCpydevice.tv_usec) - (start_memCpydevice.tv_sec * 1000000 + start_memCpydevice.tv_usec)));



    printf("%ld, %ld, %ld, %ld, %ld, %ld, %ld, %ld, %ld, %ld, %ld, %ld, %ld, %ld, %d\n", 
    ((end_total.tv_sec * 1000000 + end_total.tv_usec) - (start_total.tv_sec * 1000000 + start_total.tv_usec)),
    ((end_total.tv_sec * 1000000 + end_total.tv_usec) - (start_total.tv_sec * 1000000 + start_total.tv_usec) - ((end_image_read.tv_sec * 1000000 + end_image_read.tv_usec) - (start_image_read.tv_sec * 1000000 + start_image_read.tv_usec)) - ((end_edge_write.tv_sec * 1000000 + end_edge_write.tv_usec) - (start_edge_write.tv_sec * 1000000 + start_edge_write.tv_usec))), 
    ((end_image_read.tv_sec * 1000000 + end_image_read.tv_usec) - (start_image_read.tv_sec * 1000000 + start_image_read.tv_usec)),
    ((end_edge_write.tv_sec * 1000000 + end_edge_write.tv_usec) - (start_edge_write.tv_sec * 1000000 + start_edge_write.tv_usec)),
    ((end_memCpy.tv_sec * 1000000 + end_memCpy.tv_usec) - (start_memCpy.tv_sec * 1000000 + start_memCpy.tv_usec)),
    ((end_hor.tv_sec * 1000000 + end_hor.tv_usec) - (start_hor.tv_sec * 1000000 + start_hor.tv_usec)),
    ((end_ver.tv_sec * 1000000 + end_ver.tv_usec) - (start_ver.tv_sec * 1000000 + start_ver.tv_usec)),
    ((end_mag.tv_sec * 1000000 + end_mag.tv_usec) - (start_mag.tv_sec * 1000000 + start_mag.tv_usec)),
    ((end_dir.tv_sec * 1000000 + end_dir.tv_usec) - (start_dir.tv_sec * 1000000 + start_dir.tv_usec)),
    ((end_sort.tv_sec * 1000000 + end_sort.tv_usec) - (start_sort.tv_sec * 1000000 + start_sort.tv_usec)),
    ((end_supp.tv_sec * 1000000 + end_supp.tv_usec) - (start_supp.tv_sec * 1000000 + start_supp.tv_usec)),
    ((end_hys.tv_sec * 1000000 + end_hys.tv_usec) - (start_hys.tv_sec * 1000000 + start_hys.tv_usec)),
    ((end_edge.tv_sec * 1000000 + end_edge.tv_usec) - (start_edge.tv_sec * 1000000 + start_edge.tv_usec)),
    ((end_memCpydevice.tv_sec * 1000000 + end_memCpydevice.tv_usec) - (start_memCpydevice.tv_sec * 1000000 + start_memCpydevice.tv_usec)),
    block_dim);

    free(temp_hor);
    free(temp_ver);
    cudaFree(d_image);
    cudaFree(d_temp_hor);
    cudaFree(d_temp_ver);
    cudaFree(d_kernel);
    cudaFree(d_dkernel);
    cudaFree(d_grad);
    cudaFree(d_mag);
    free(temp_grad);
    free(temp_mag);
    cudaFree(d_edge);
    free(temp_edge);
    cudaFree(d_corn);
    free(temp_corn);


    return 0;




}
