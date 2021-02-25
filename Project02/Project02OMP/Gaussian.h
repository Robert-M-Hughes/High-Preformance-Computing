/*
This will be the header file for the Guassian and Gaussian derivative masks

*/

#include "stdio.h" 
#include "math.h"
#include "stdlib.h"
#include "string.h"
#include <omp.h>

//Funtion Declaration
double * gaussianKernel(double sigma);
double * gaussianDerivKernel(double sigma);


//Function Definition

//Function to call to get the Gaussian Kernel 
double * gaussianKernel(double sigma){
    double a;  // since this number is rounded it can be cast to an int

    float sum = 0; // set the intial sum
    a = round(2.5*sigma-.5);//calculate and round the value of a
    //printf("%f\n", a);
    const float w = 2*a+1; // calculate the width of the kernel
    double * gaussiankernel = malloc(w*sizeof(double)); // since we know the width of the array we can statically declare it
    double two_SigxSig = 2*sigma*sigma;
   // printf("%f\n", two_SigxSig);
    int i;
    #pragma omp parallel for
      for( i=0; i<(int)w; i++){
         // printf("I am number: %d\n", omp_get_thread_num());

          gaussiankernel[i] = exp((-1*(i-a)*(i-a))/two_SigxSig);
          sum = sum + gaussiankernel[i];
        // printf("%f ", gaussiankernel[i]);

      }
    //printf("%f\n");
   // printf("Gaussian: ");
    #pragma omp parallel for
      for(i = 0; i < (int)w; i++){//have to re-step through the array to divide by the sum
          gaussiankernel[i] = gaussiankernel[i]/sum;
      //     printf("%f ", gaussiankernel[i]);
      }
  //  printf("\n");
    //printf("%f\n", sum);
    return gaussiankernel ;
}

//Function to call the Gaussian Kernel
double * gaussianDerivKernel( double sigma){
    double a;  // since this number is rounded it can be cast to an int
    float w; // the width of the kernel
    float sum = 0; // set the intial sum
    a = round(2.5*sigma-.5);//calculate and round the value of a
    w = 2*a+1; // calculate the width of the kernel
    double * gaussianDeriv = malloc(w*sizeof(double));//can statically allocate since we know the width
    int i;
    #pragma omp parallel for
    for(i = 0; i < (int)w; i++){
        gaussianDeriv[i] = -1*(i-a)*exp((-1*(i-a)*(i-a))/(2*sigma*sigma));
        sum = sum - (i+1)*gaussianDeriv[i];
        //printf("%f ", gaussianDeriv[i]);
    }
    //printf("%f\n");
  //  printf("Gaussian Derivative: ");
    #pragma omp parallel for
    for(i = 0; i < (int)w; i++){//have to re-step through the array to divide by the sum
        gaussianDeriv[i] = gaussianDeriv[i]/sum;
  //      printf("%f ", gaussianDeriv[i]);

    }
    //printf("\n");
   // printf("%f\n", sum);
    return gaussianDeriv ;

}