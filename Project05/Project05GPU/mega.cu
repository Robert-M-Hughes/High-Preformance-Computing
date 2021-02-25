/*
 * This program uses the host CURAND API to generate 100 
 * pseudorandom floats.
 */
 #include <stdio.h>
 #include <stdlib.h>
 #include <cuda.h>
 #include <curand.h>
 #include <time.h>
 #include <sys/time.h>
 
 #define CUDA_CALL(x) do { if((x)!=cudaSuccess) { \
     printf("Error at %s:%d\n",__FILE__,__LINE__);\
     return EXIT_FAILURE;}} while(0)
 #define CURAND_CALL(x) do { if((x)!=CURAND_STATUS_SUCCESS) { \
     printf("Error at %s:%d\n",__FILE__,__LINE__);\
     return EXIT_FAILURE;}} while(0)
 
 int main(int argc, char *argv[])
 {

    struct timeval start_h2d, end_h2d, start_d2h, end_d2h;


     int n;
     n = 1000 * 256 * atof(argv[1]); //256 * 1000 floats per mB
     size_t i;
     curandGenerator_t gen;
     float *devData, *hostData;
 
     /* Allocate n floats on host */
     hostData = (float *)calloc(n, sizeof(float));
 
     /* Allocate n floats on device */
     CUDA_CALL(cudaMalloc((void **)&devData, n*sizeof(float)));
 
     /* Create pseudo-random number generator */
     CURAND_CALL(curandCreateGenerator(&gen, 
                 CURAND_RNG_PSEUDO_DEFAULT));
     
     /* Set seed */
     CURAND_CALL(curandSetPseudoRandomGeneratorSeed(gen, 
                 1234ULL));
 
     /* Generate n floats on device */
     CURAND_CALL(curandGenerateUniform(gen, devData, n));
 
     /* Copy device memory to host */
     CUDA_CALL(cudaMemcpy(hostData, devData, n * sizeof(float),
         cudaMemcpyDeviceToHost));
 
     /* Show result 
     for(i = 0; i < n; i++) {
         printf("%1.4f ", hostData[i]);
     }
     printf("\n");
    */

    float* rando = (float*)malloc(n*sizeof(float));
    float* d_rand;
    cudaMalloc((void **)&d_rand,sizeof(float)*n);


    gettimeofday(&start_h2d, NULL);
        cudaMemcpy(d_rand, rando,sizeof(float)*n,cudaMemcpyHostToDevice);
    gettimeofday(&end_h2d, NULL);


    gettimeofday(&start_d2h, NULL);
        cudaMemcpy(rando, d_rand,sizeof(float)*n,cudaMemcpyDeviceToHost);
    gettimeofday(&end_d2h, NULL);

    printf("%ld, %ld\n", ((end_h2d.tv_sec * 1000000 + end_h2d.tv_usec) - (start_h2d.tv_sec * 1000000 + start_h2d.tv_usec)),((end_d2h.tv_sec * 1000000 + end_d2h.tv_usec) - (start_d2h.tv_sec * 1000000 + start_d2h.tv_usec)));
    
     /* Cleanup */
     CURAND_CALL(curandDestroyGenerator(gen));
     CUDA_CALL(cudaFree(devData));
     free(hostData);    
     return EXIT_SUCCESS;
 }