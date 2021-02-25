//This will be the c File for program 1
/*
Author: Robert Hughes
This program will be creaed by using the documentation that was given to us in class along with using the code from ecpe 124 that Dr.Pallipuram gave us last semester


*/
#include <stdio.h>
#include <stddef.h>
#include <time.h>
#include <omp.h>
#include <math.h>
#include <mpi.h>


#include "image.h"
//#include "image_template.h" not for this assignment C cannot handle the templates
#include "Gaussian.h"
#include "convolution.h"
#include "NonMaxSuppression.h"
//#include "Hysteresis.h"
#include "EdgeLinking.h"
#include "Corners.h"
#include "HysteresisMERGE.h"


//Can use sigma as a constant to run the code
//#define sigma .6
#define WINDOW 7




//functions that we will utilize
void sendRec_Ghost(int *chunk_with_ghost, int rank, int comm_size, int height, int width, int a){
    printf("SENDING AND RECIEVING GHOSTS: %d\n", rank);
    //get the rows above(needed for all ranks except for rank = 0)
    

    int bottom_start = height*width/comm_size;//this will be where the bottom rows are because of the buffer left by the upper rows of the same size
//tops
    if(rank > 0 && rank < (comm_size-1)) {
        MPI_Sendrecv(&chunk_with_ghost[bottom_start],a*width,MPI_INT,rank+1, 0,chunk_with_ghost, a*width, MPI_INT, rank-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE );
    }
    else if(rank == 0){
        memset(chunk_with_ghost, 0, a*width*sizeof(int));
        MPI_Send(&chunk_with_ghost[bottom_start], a*width, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
    }
    else if(rank == comm_size -1){
        MPI_Recv(chunk_with_ghost, a*width, MPI_INT, rank-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    int top_start=a*width;
//bottoms
    if(rank > 0 && rank < (comm_size-1)) {
        MPI_Sendrecv(&chunk_with_ghost[top_start],a*width,MPI_INT,rank-1, 0, &chunk_with_ghost[bottom_start+a*width], a*width, MPI_INT, rank+1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE );
    }
    else if(rank == comm_size -1){
        memset(&chunk_with_ghost[bottom_start+a*width], 0, a*width*sizeof(int));
        MPI_Send(&chunk_with_ghost[top_start], a*width, MPI_INT, rank - 1, 0, MPI_COMM_WORLD);
    }
    else if(rank == 0){
        MPI_Recv(&chunk_with_ghost[bottom_start+a*width], a*width, MPI_INT, rank+1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    }

      
}




int main( int argc, char** argv ){
//setup
    int rank = 0;       // rank of this process
    int comm_size = 0; // number of processes
    //integers for the loops

    // the width of the gaussian kernel
    int w =0;
    //the value of the sigma value to be read later by the command line
    float sigma = 0;
    //allocate the memory to hold all of the arrays that we need

    double * gauss;//the data allocation for the gaussian kernel
    double * gaussDeriv;//the data allocatin for the gaussian derivative kernel
    double * flip_gaussDeriv; //allocation for the flipped gaussian derivative so that the image is not flipped
    //pre-define these so that we can pull the image in using the library provided to us
    int height, width;
    int *image;


    /*  initialize MPI, determine number of processes and rank. */
    MPI_Init(NULL, NULL);
    //Get the number of tasks
    MPI_Comm_size(MPI_COMM_WORLD, &comm_size);

    //Get my task #
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    //	printf ("Task %d started\n", rank);

    sigma = atof(argv[2]);

    int i =0;
    int j =0;

    //get the size of the kernel so we can store it after the functions are preformed
    w = 2*(round(2.5*sigma -.5))+1;
    //printf("w= %d \n", w);
    //allocate these according to the width of the kernel
    gauss = malloc(w * sizeof(double));
    gaussDeriv = malloc(w * sizeof(double));
    /*
    used these to test the convolution because they are easier to check the work to make sure that the function is behaving correctly


    double kernel[] = {.25, .5, .25};
    */


    //calculate the two kernels we need to run the program
    gauss = gaussianKernel(sigma);

    gaussDeriv = gaussianDerivKernel(sigma);

    //printf("Hello\n");
    if(rank==0){
        read_image_template(argv[1], &image, &width, &height);
    }
    MPI_Barrier(MPI_COMM_WORLD);

    MPI_Bcast(&height, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&width, 1, MPI_INT, 0, MPI_COMM_WORLD);

    //Flip the Gaussian Derivative Kernel since it is not symetric
    flip_gaussDeriv = malloc(w * sizeof(double));

    for(i = w-1, j = 0; i >= 0; i--, j++){

        flip_gaussDeriv[j] = gaussDeriv[i];
        //printf("%d \n", w);
        }
    //}
    //printf("Flipped Gaussian Derivative: ");
    /*
    for(j = 0; j < w; j++){
         printf("%f   %d", flip_gaussDeriv[j], rank);

        }
    //  printf("\n");
    */
    free(gaussDeriv);//we can now free the gaussian derivative kernel since we have the new flipped kernel



    printf ("Task %d started\n", rank);
    /* DEBUGGING CODE
    int temp[] = {0, 1, 2, 3, 
                        4, 5, 6, 7,
                        8, 9, 10, 11, 
                        12, 13, 14, 15};
    */
    int a = round(2.5*sigma-.5);
    int chunk_size = width*height/comm_size;
    int chunk_ghost_size = chunk_size+width*2*a;
    int middle_start=width*a;

  
    int *chunk_with_ghosts = malloc(chunk_ghost_size*sizeof(int));        
//Scatter 
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Scatter(image, chunk_size, MPI_INT, &chunk_with_ghosts[middle_start],chunk_size, MPI_INT, 0, MPI_COMM_WORLD);
    //need the a value for the ghost rows
 
    MPI_Barrier(MPI_COMM_WORLD);

    sendRec_Ghost(chunk_with_ghosts, rank, comm_size, height, width, a);

    MPI_Barrier(MPI_COMM_WORLD);


//Convolution
    
    int *check;
    if(rank == 0){
        check = malloc(width*height*sizeof(int));
    }
    MPI_Gather(&chunk_with_ghosts[middle_start], chunk_size, MPI_INT, check, chunk_size, MPI_INT, 0, MPI_COMM_WORLD );
    if(rank == 0){
        write_image_template("check", check, width, height);

    }
    printf("Started the canny edge on process %d\n", rank);
    int * Vertical;
    int * Horizontal;
    int new_height;

    Vertical = malloc((width*(height/comm_size+2*a))*sizeof(int));  
    Horizontal = malloc((width*(height/comm_size+2*a))*sizeof(int));  
    new_height = (height/comm_size+2*a);      
    
    //tempV = convolveVert( image, gauss, w, height, width );

    // create characer arrays to name the files for when we use the image write


    //Vertical
    Vertical = convolveHoriz(chunk_with_ghosts, gauss, w, new_height, width );

    Vertical = convolveVert(Vertical, flip_gaussDeriv, w, new_height, width);



    //Horizontal
    Horizontal = convolveVert(chunk_with_ghosts, gauss, w, new_height, width);

    Horizontal = convolveHoriz(Horizontal, flip_gaussDeriv, w, new_height, width);


//*****Section to compute the Grandient and Magnitude********

//preform the convolutions



/*MPI_Gather(
    void* send_data,
    int send_count,
    MPI_Datatype send_datatype,
    void* recv_data,
    int recv_count,
    MPI_Datatype recv_datatype,
    int root,
    MPI_Comm communicator)*/

//compute the horizontal and vertical gradients
//make space for the arrays

/*
    int * Horizontal_new;
    int * Vertical_new;

    if (rank == 0){
        printf("Started the canny edge\n");
        Vertical_new = malloc(height*width * sizeof(int));
        Horizontal_new = malloc(height*width * sizeof(int));
    }

    MPI_Gather(&Vertical[middle_start], chunk_size, MPI_INT, Vertical_new, chunk_size, MPI_INT, 0, MPI_COMM_WORLD );
    MPI_Gather(&Horizontal[middle_start], chunk_size, MPI_INT, Horizontal_new, chunk_size, MPI_INT, 0, MPI_COMM_WORLD );

    */
    
    //tempV = convolveVert( image, gauss, w, height, width );

    // create characer arrays to name the files for when we use the image write
    /*
    char tempVer[] = "temp_vertical";
    char tempHor[] = "temp_horizontal";
    char ver[] = "vertical";
    char hor[] = "horizontal";
    char grad[] = "gradient";
    char mag[] = "magnitude";
    

    //Vertical
    //write_image_template(tempVer, Vertical_new, width, height);

    //write_image_template(ver, Vertical_new, width, height);

    //Horizontal
    //write_image_template(tempHor, Horizontal_new, width, height);

    //write_image_template(hor, Horizontal_new, width, height);
    */

    //****Calculate the Maginitude and Gradient****
    //allocate the memory
    int * Magnitude = malloc(chunk_size * sizeof(int));
    int * Gradient = malloc(chunk_size * sizeof(int));

    //can put these two together in one loop to try to increase the speed, could preform loop unrolling however it is trivial and we are going to use openMP in the future anyway
    for(i = 0; i < chunk_size; i++){
        int loc = middle_start + i;
        Gradient[i] = atan2(Horizontal[loc], Vertical[loc]);
        //using the euclidean distance for the magnitude
        Magnitude[i] = sqrt((Vertical[loc]*Vertical[loc])+(Horizontal[loc]*Horizontal[loc]));

    }
    //write out the files

    //write_image_template(mag, Magnitude, width, height);
    //write_image_template(grad, Gradient, width, height);


    //printf("Done first part\n");

    //preform the non-max suppression
    //char sup[] = "Suppression";


    int * edges = malloc(chunk_size * sizeof(int));
    edges = suppression(Magnitude, Gradient, height/comm_size, width);
    //printf("Finished the supression\n");

    //write_image_template(sup, edges, width, height);


    //preform the hysteresis
    //char hys[] = "Hyteresis";
    int* hyster;
    if(rank = 0){
        hyster = malloc(height*width * sizeof(int));

    }
    MPI_Gather(edges, chunk_size, MPI_INT, hyster, chunk_size, MPI_INT, 0, MPI_COMM_WORLD );
    
    MPI_Finalize();
    //killed the MPI to restart in the hysteresis
    edges = hysteresis(hyster, height, width);
    
    /*  initialize MPI, determine number of processes and rank. */
    MPI_Init(NULL, NULL);
    //Get the number of tasks
    MPI_Comm_size(MPI_COMM_WORLD, &comm_size);

    //Get my task #
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    //printf("Finished the hyteresis\n");

    //write_image_template(hys, edges, width, height);


    //preform the edge linking
    

    edges = edgeLinking(edges, height/comm_size, width);
    printf("Finished the edge linking on Rank: %d\n", rank);
    //char edg[] = "Edges";
    //write_image_template(edg, edges, width, height);

    int * corners = malloc(chunk_size * sizeof(int));
    corners = cornersList(Vertical, Horizontal, height/comm_size, width, WINDOW);
    //write_image_template("corners", corners, width, height);
    int * corner_full;
    int * edge_full;
    int * mag_full;
    int * grad_full;
    int * vert_full;
    int * hori_full;
    

    if(rank == 0){
        corner_full = malloc(height*width * sizeof(int));
        edge_full = malloc(height*width * sizeof(int));
        grad_full = malloc(height*width * sizeof(int));
        mag_full = malloc(height*width * sizeof(int));
        vert_full = malloc(height*width * sizeof(int));
        hori_full = malloc(height*width * sizeof(int));

    }

    MPI_Gather(edges, chunk_size, MPI_INT, edge_full, chunk_size, MPI_INT, 0, MPI_COMM_WORLD );
    MPI_Gather(corners, chunk_size, MPI_INT, corner_full, chunk_size, MPI_INT, 0, MPI_COMM_WORLD );
    MPI_Gather(Gradient, chunk_size, MPI_INT, grad_full, chunk_size, MPI_INT, 0, MPI_COMM_WORLD );
    MPI_Gather(Magnitude, chunk_size, MPI_INT, mag_full, chunk_size, MPI_INT, 0, MPI_COMM_WORLD );
    MPI_Gather(&Vertical[middle_start], chunk_size, MPI_INT, vert_full, chunk_size, MPI_INT, 0, MPI_COMM_WORLD );
    MPI_Gather(&Horizontal[middle_start], chunk_size, MPI_INT, hori_full, chunk_size, MPI_INT, 0, MPI_COMM_WORLD );


    
    if(rank == 0){
        write_image_template("Edges", edge_full, width, height);
        write_image_template("Corners", corner_full, width, height);
        write_image_template("magnitude", mag_full, width, height);
        write_image_template("gradient", grad_full, width, height);
        write_image_template("Vertical", vert_full, width, height);
        write_image_template("Horizontal", hori_full, width, height);


    }
    
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
    free(edges);
    free(Vertical);
    free(Horizontal);
    free(flip_gaussDeriv);
    free(gauss);
    free(Magnitude);
    free(Gradient);
    free (corners);

    
    //free (chunk);
    //printf("Program Executed: Egde found.\n");
    //end = clock();
    //cpu_time_used = ((double)(end - start))/ CLOCKS_PER_SEC;
    //printf("Time taken to execute: %f\n", cpu_time_used);
    //printf("%s, %f, %f \n", argv[1], sigma, cpu_time_used);

    MPI_Finalize();


    return 0;
}



