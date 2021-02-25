/************************************************
*	   					*
*		Author: Sohil Singh		*
*		Project 2: Corner Detection	* 
*						*
*		Date: 2/06/2019			*
*						*
************************************************/

#include <cuda.h>
#include <thrust/device_vector.h>
#include <thrust/host_vector.h>
#include <thrust/sort.h>
#include <thrust/copy.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <math.h>
#include <ctype.h>
#include "image_template.h"
#include "gaussian.h"
#include "gaussian_derivative.h"
#include "convolution_horizontal.h"
#include "convolution_horizontal_shared.h"
#include "convolution_vertical.h"
#include "convolution_vertical_shared.h"
#include "magnitude.h"
#include "direction.h"
#include "suppress.h"
#include "comparator.h"
#include "hysteresis.h"
#include "edges.h"
//#include "corners.h"
//#include "output_corners.h"
#include <time.h>
#include <sys/time.h>

int main(int argc, char *argv[])
{
	
	// Time each kernel
	// Communication times memcpy, device to host, host to device
	// No file i/o

	// Pointer logic
	// int a = 7; stores value to memory address as 7
	// int *a_ptr = &a; get memory address where a value is located
	// int a_address = a_ptr; a_address gets memory address a_ptr stores
	// int a_value = *a_ptr; get value at a_ptr memory address
	// To change a variable in a function must call by reference by passing the 		// address of the variable
	// Read input image file

	cudaFree(0);
	// CUDA Device synchronize
	//	
	int j,t,comp_time,ete_time;
	int height;
	int width, size,ninety_percentile_index;
	char sys_arr[2];
	// create 2D array to hold input image	
	float *input_image=NULL,t_hi,t_lo;
	
	float sigma=atof(argv[2]);
	int block_size = atoi(argv[3]);
	int gaussian_width,gd_width;
	float *hor_gaussian=NULL,*hor_gaussian_derivative=NULL,*temp_horizontal=NULL,*horizontal=NULL,*temp_vertical=NULL,*vertical=NULL,*mag_output=NULL,*dir_output=NULL,*sup_output=NULL,*sort_sup_output=NULL,*hyst_output=NULL,*edges_output=NULL,*corner_output=NULL,*output_image=NULL;
	float *d_input_image, *d_hor_gaussian, *d_hor_gaussian_deriv, *d_temp_horizontal, *d_horizontal; 
	//"/Users/sss/hpc_ecpe_251/Lenna_org_256.pgm"
	float *d_temp_vertical, *d_vertical;
	float *d_mag_output, *d_dir_output;
	float *d_sup_output, *d_hyst_output,*d_edges_output;
	char *input_filepath = argv[1];
	// printf("Filepath: %s\n", input_filepath);		// Test line
	//char *output_filepath = "/Users/sss/hpc_ecpe_251/output.pgm";
	// printf("Filepath: %s\n", output_filepath);		// Test line
	//omp_set_num_threads(atoi(argv[3])); //Set num of threads eq. to procs
	
	int iterations = atoi(argv[1]);
	// ete = end to end
	// comp = computation
  	struct timeval start_ete, end_ete, start_comp, end_comp, start_fileio1, end_fileio1;
	struct timeval start_fileio2, end_fileio2, start_h2d, end_h2d, start_conv_hor, end_conv_hor;
	struct timeval start_conv_ver, end_conv_ver, start_mag, end_mag, start_dir, end_dir, start_sort, end_sort;
	struct timeval start_sup, end_sup, start_edge, end_edge, start_hyst, end_hyst, start_d2h, end_d2h;

  	//gettimeofday(&start_ete, NULL);

	// Read in image
        gettimeofday(&start_fileio1, NULL);
        read_image_template(input_filepath,&input_image,&width,&height);
        gettimeofday(&end_fileio1, NULL);
	
//	malloc GPU buffers
	cudaMalloc((void **)&d_input_image, sizeof(float)*height*width);
	cudaMalloc((void **)&d_hor_gaussian, sizeof(float)*gaussian_width);
	cudaMalloc((void **)&d_hor_gaussian_deriv, sizeof(float)*gd_width);
	cudaMalloc((void **)&d_temp_horizontal, sizeof(float)*height*width);
	cudaMalloc((void **)&d_horizontal, sizeof(float)*height*width);
	cudaMalloc((void **)&d_temp_vertical, sizeof(float)*height*width);
        cudaMalloc((void **)&d_vertical, sizeof(float)*height*width);	
	cudaMalloc((void **)&d_mag_output, sizeof(float)*height*width);
        cudaMalloc((void **)&d_dir_output, sizeof(float)*height*width);
	cudaMalloc((void **)&d_sup_output, sizeof(float)*height*width);
	cudaMalloc((void **)&d_hyst_output, sizeof(float)*height*width);
	cudaMalloc((void **)&d_edges_output, sizeof(float)*height*width);	

       //malloc host buffers
	horizontal=(float*)malloc(sizeof(float)*(height*width));	
	temp_horizontal=(float*)malloc(sizeof(float)*(height*width));
	temp_vertical=(float*)malloc(sizeof(float)*(height*width));
	vertical=(float*)malloc(sizeof(float)*(height*width));	
	mag_output=(float*)malloc(sizeof(float)*(height*width));
        dir_output=(float*)malloc(sizeof(float)*(height*width));
	sup_output=(float*)malloc(sizeof(float)*(height*width));	
	hyst_output=(float*)malloc(sizeof(float)*(height*width));
	edges_output=(float*)malloc(sizeof(float)*(height*width));	

	gettimeofday(&start_ete, NULL);

	
        // Generate horizontal gaussian kernel
        gaussian(sigma,&hor_gaussian,&gaussian_width);

        // Generate horizontal gaussian derivative kernel
        gaussian_derivative(sigma,&hor_gaussian_derivative,&gd_width);

//	Transfer gaussian data to GPGPU device
	gettimeofday(&start_h2d, NULL);
	cudaMemcpy(d_input_image, input_image, sizeof(float)*height*width, cudaMemcpyHostToDevice);
	cudaMemcpy(d_hor_gaussian, hor_gaussian, sizeof(float)*gaussian_width, cudaMemcpyHostToDevice);
	cudaMemcpy(d_hor_gaussian_deriv, hor_gaussian_derivative, sizeof(float)*gd_width, cudaMemcpyHostToDevice);
	cudaDeviceSynchronize();
	gettimeofday(&end_h2d, NULL);
	// write_image_template("Input.pgm",input_image,width,height);
	//printf("Input[0][0] = %f",input_image[65535]);
	//printf("Input[0][5] = %d",0*0);
	
	dim3 dimBlock(block_size, block_size);
	dim3 dimGrid(height/block_size, width/block_size);
	
	// Convolution horizontal
	gettimeofday(&start_conv_hor, NULL);
//	convolution_vertical<<<dimGrid, dimBlock>>>(d_input_image,d_temp_horizontal,d_hor_gaussian,gd_width,1,height,width);
//	convolution_horizontal<<<dimGrid, dimBlock>>>(d_temp_horizontal,d_horizontal,d_hor_gaussian_deriv,gaussian_width,height,width);
	convolution_vertical_shared<<<dimGrid, dimBlock,sizeof(float)*block_size*block_size>>>(d_input_image,d_temp_horizontal,d_hor_gaussian,gd_width,1,height,width);
	convolution_horizontal_shared<<<dimGrid, dimBlock,sizeof(float)*block_size*block_size>>>(d_temp_horizontal,d_horizontal,d_hor_gaussian_deriv,1,gaussian_width,height,width);
	cudaDeviceSynchronize();
	gettimeofday(&end_conv_hor, NULL);

	// Convolution vertical
	gettimeofday(&start_conv_ver, NULL);
//	convolution_horizontal<<<dimGrid, dimBlock>>>(d_input_image,d_temp_vertical,d_hor_gaussian,gaussian_width,height,width);
//	convolution_vertical<<<dimGrid, dimBlock>>>(d_temp_vertical,d_vertical,d_hor_gaussian_deriv,gd_width,1,height,width);
	convolution_horizontal_shared<<<dimGrid, dimBlock,sizeof(float)*block_size*block_size>>>(d_input_image,d_temp_vertical,d_hor_gaussian,1,gaussian_width,height,width);
	convolution_vertical_shared<<<dimGrid, dimBlock,sizeof(float)*block_size*block_size>>>(d_temp_vertical,d_vertical,d_hor_gaussian_deriv,gd_width,1,height,width);
	cudaDeviceSynchronize();
	gettimeofday(&end_conv_ver, NULL);	

	// Magnitude
	gettimeofday(&start_mag, NULL);	
	magnitude<<<dimGrid, dimBlock>>>(d_mag_output,d_horizontal,d_vertical,height,width);
	cudaDeviceSynchronize();
	gettimeofday(&end_mag, NULL);

	// Direction
	gettimeofday(&start_dir, NULL);
	direction<<<dimGrid, dimBlock>>>(d_dir_output,d_horizontal,d_vertical,height,width);
	cudaDeviceSynchronize();
	gettimeofday(&end_dir, NULL);

	// Non-max suppression
	//memcpy(sup_output, mag_output, height*width*sizeof(float));
	gettimeofday(&start_sup, NULL);
//	cudaMemcpy(d_sup_output, d_mag_output, sizeof(float)*height*width, cudaMemcpyDeviceToDevice);
	suppress<<<dimGrid, dimBlock>>>(d_sup_output,d_mag_output,d_dir_output,height,width);
	cudaDeviceSynchronize();
	gettimeofday(&end_sup, NULL);
	
//	cudaDeviceSynchronize();	
	gettimeofday(&start_sort, NULL);
	thrust::device_ptr<float> thr_d(d_sup_output);
	thrust::device_vector<float>d_sup_vec(thr_d,thr_d+(height*width));
	thrust::sort(d_sup_vec.begin(),d_sup_vec.end());
	int index = (int)(.9*height*width);
	float th_high = d_sup_vec[index];
	float th_low =th_high/5;
//	cudaDeviceSychronize();
	gettimeofday(&end_sort, NULL);

	// Hysteresis
	// Sort values
	
	gettimeofday(&start_hyst, NULL);
	hysteresis<<<dimGrid, dimBlock>>>(d_hyst_output,d_sup_output,height,width,th_high,th_low);
	cudaDeviceSynchronize();
	gettimeofday(&end_hyst, NULL);

	// Edges
	gettimeofday(&start_edge, NULL);
	edges<<<dimGrid, dimBlock>>>(d_edges_output,d_hyst_output,height,width);
	cudaDeviceSynchronize();
	gettimeofday(&end_edge, NULL);
	
	// Device to host
	gettimeofday(&start_d2h, NULL);
	cudaMemcpy(temp_horizontal, d_temp_horizontal, sizeof(float)*height*width, cudaMemcpyDeviceToHost);
        cudaMemcpy(horizontal, d_horizontal, sizeof(float)*height*width, cudaMemcpyDeviceToHost);
	cudaMemcpy(temp_vertical, d_temp_vertical, sizeof(float)*height*width, cudaMemcpyDeviceToHost);
	cudaMemcpy(vertical, d_vertical, sizeof(float)*height*width, cudaMemcpyDeviceToHost);
	cudaMemcpy(mag_output, d_mag_output, sizeof(float)*height*width, cudaMemcpyDeviceToHost);
	cudaMemcpy(dir_output, d_dir_output, sizeof(float)*height*width, cudaMemcpyDeviceToHost);
	cudaMemcpy(sup_output, d_sup_output, sizeof(float)*height*width, cudaMemcpyDeviceToHost);
	cudaMemcpy(hyst_output, d_hyst_output, sizeof(float)*height*width, cudaMemcpyDeviceToHost);	
	cudaMemcpy(edges_output, d_edges_output, sizeof(float)*height*width, cudaMemcpyDeviceToHost);
	cudaDeviceSynchronize();
	gettimeofday(&end_d2h, NULL);
       
	// Write out images; file output 2
	gettimeofday(&start_fileio2, NULL);
        write_image_template("Temp_Horizontal.pgm",temp_horizontal,width,height);
        write_image_template("Horizontal.pgm",horizontal,width,height);
	write_image_template("Temp_Vertical.pgm",temp_vertical,width,height);
	write_image_template("Vertical.pgm",vertical,width,height);
	write_image_template("Magnitude.pgm",mag_output,width,height);
	write_image_template("Gradient.pgm",dir_output,width,height);
	write_image_template("Suppress.pgm",sup_output,width,height);
	write_image_template("Hysteresis.pgm",hyst_output,width,height);
	write_image_template("Edges.pgm",edges_output,width,height);
//	cudaDeviceSynchronize();
	gettimeofday(&end_fileio2, NULL);


/*
//	corners(&corner_output,vertical,horizontal,height,width,7);
	
//	output_corners(&output_image, corner_output, input_image, height, width);

*/
//	gettimeofday(&end_comp, NULL);
//	write_image_template("Corner.pgm",output_image,width,height);
	cudaDeviceSynchronize();
	gettimeofday(&end_ete, NULL);
	/*
	start_fileio1, end_fileio1;
        struct timeval start_fileio2, end_fileio2, start_h2d, end_h2d, start_conv_hor, end_conv_hor;
        struct timeval start_conv_ver, end_conv_ver, start_mag, end_mag, start_dir, end_dir, start_sort, end_sort;
        struct timeval start_sup, end_sup, start_edge, end_edge, start_hyst, end_hyst, start_d2h, end_d2h;
	*/
//	printf("%ld,", ((end_comp.tv_sec * 1000000 + end_comp.tv_usec) - (start_comp.tv_sec * 1000000 + start_comp.tv_usec)));
	printf("%ld\n", ((end_ete.tv_sec * 1000000 + end_ete.tv_usec) - (start_ete.tv_sec * 1000000 + start_ete.tv_usec)));
	printf("%ld\n", ((end_ete.tv_sec * 1000000 + end_ete.tv_usec) - (start_ete.tv_sec * 1000000 + start_ete.tv_usec) - ((end_fileio1.tv_sec * 1000000 + end_fileio1.tv_usec) - (start_fileio1.tv_sec * 1000000 + start_fileio1.tv_usec)) - ((end_fileio2.tv_sec * 1000000 + end_fileio2.tv_usec) - (start_fileio2.tv_sec * 1000000 + start_fileio2.tv_usec))));
	printf("%ld\n", ((end_fileio1.tv_sec * 1000000 + end_fileio1.tv_usec) - (start_fileio1.tv_sec * 1000000 + start_fileio1.tv_usec)));
	printf("%ld\n", ((end_fileio2.tv_sec * 1000000 + end_fileio2.tv_usec) - (start_fileio2.tv_sec * 1000000 + start_fileio2.tv_usec)));
	printf("%ld\n", ((end_h2d.tv_sec * 1000000 + end_h2d.tv_usec) - (start_h2d.tv_sec * 1000000 + start_h2d.tv_usec)));
        printf("%ld\n", ((end_fileio2.tv_sec * 1000000 + end_fileio2.tv_usec) - (start_fileio2.tv_sec * 1000000 + start_fileio2.tv_usec)));
	printf("%ld\n", ((end_conv_hor.tv_sec * 1000000 + end_conv_hor.tv_usec) - (start_conv_hor.tv_sec * 1000000 + start_conv_hor.tv_usec)));
        printf("%ld\n", ((end_conv_ver.tv_sec * 1000000 + end_conv_ver.tv_usec) - (start_conv_ver.tv_sec * 1000000 + start_conv_ver.tv_usec)));
        printf("%ld\n", ((end_mag.tv_sec * 1000000 + end_mag.tv_usec) - (start_mag.tv_sec * 1000000 + start_mag.tv_usec)));
        printf("%ld\n", ((end_dir.tv_sec * 1000000 + end_dir.tv_usec) - (start_dir.tv_sec * 1000000 + start_dir.tv_usec)));
	printf("%ld\n", ((end_sort.tv_sec * 1000000 + end_sort.tv_usec) - (start_sort.tv_sec * 1000000 + start_sort.tv_usec)));
        printf("%ld\n", ((end_sup.tv_sec * 1000000 + end_sup.tv_usec) - (start_sup.tv_sec * 1000000 + start_sup.tv_usec)));
	printf("%ld\n", ((end_hyst.tv_sec * 1000000 + end_hyst.tv_usec) - (start_hyst.tv_sec * 1000000 + start_hyst.tv_usec)));
        printf("%ld\n", ((end_edge.tv_sec * 1000000 + end_edge.tv_usec) - (start_edge.tv_sec * 1000000 + start_edge.tv_usec)));
	
	return 0;


}


