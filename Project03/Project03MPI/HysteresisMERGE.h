/*
This will be the header file for the Hysteresis

*/

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <memory.h>
#include "string.h"



//function definition
int* hysteresis(int* hysteresis, int height, int width);
int cmpfunc(const void * a, const void * b);
double logbase2(double a);
void printArray(long* data, int length);
int verifyOrder(long* data, int length);
void merge(long* array, long* left_data, int left_length, long* right_data, int right_length);
void parallelMergeSort(int *a, int *b, int l, int r);
void serialMergeSort(long* array, int length);
void parallelMerge(int *a, int *b, int l, int m, int r);

//Function 

/* print an array */
void printArray(long* data, int length) {
	int i;
	printf("[ ");
	for (i = 0; i < length; i++) printf("%ld ", data[i]);
	printf("]\n");
}

/* verify that array is ordered */
int verifyOrder(long* data, int length) {
	int i;
	for (i=1; i<length; i++)
		if (data[i-1] > data[i]) return 0;
	return 1;
}

/* merge the the elements in left_data and right_data
 * and store them in order in array.
 * Assumptions:
 *    (1) elements in left_data and right_data are in order.
 *    (2) array has sufficient size to hold all elements
 *        (size of array >= left_length + right_length).
 */
void merge(long* array, long* left_data, int left_length, long* right_data, int right_length)
{
	int l = 0; /* left_data index */
	int r = 0; /* right_data index */
	int i = 0; /* result array index */

	/* merge halves back into array */
	for ( i = 0, r = 0, l = 0; l < left_length && r < right_length; i++) {
		if ( left_data[l] < right_data[r] ) { array[i] = left_data[l++]; }
		else { array[i] = right_data[r++]; }
	}
	/* copy leftovers from whichever half hasn't finished */
	for ( ; l < left_length; i++, l++) { array[i] = left_data[l]; }
	for ( ; r < right_length; i++, r++) { array[i] = right_data[r]; }
}

/* sort the elements in array */
void serialMergeSort(long* array, int length) {
	int left_length, right_length;
	long* left_data;
	long* right_data;
	/* Base case, nothing to sort */
	if (length <= 1) return;

    // split (copy) array into two halves
    left_length = (length+1)/2;
    right_length = length - left_length;
    left_data = (long*)malloc(left_length*sizeof(long));
    right_data = (long*)malloc(right_length*sizeof(long));
    memcpy(left_data, array, left_length*sizeof(long));
    memcpy(right_data, &(array[left_length]), right_length*sizeof(long));

	serialMergeSort(left_data, left_length);   /* sort left half */
	serialMergeSort(right_data, right_length); /* sort right half */

	/* merge sorted halves back together */
	merge(array, left_data, left_length, right_data, right_length);
	free(left_data);
	free(right_data);
}
/********** Merge Function **********/
void parallelMerge(int *a, int *b, int l, int m, int r) {
	int h, i, j, k;
	h = l;
	i = l;
	j = m + 1;
	while((h <= m) && (j <= r)) {
		if(a[h] <= a[j]) {
			b[i] = a[h];
			h++;
			}
			else {
		b[i] = a[j];
			j++;
	        }
		i++;
		}
	if(m < h) {
		for(k = j; k <= r; k++) {
			b[i] = a[k];
			i++;
			}
		}
	else {
		for(k = h; k <= m; k++) {
			b[i] = a[k];
			i++;
		}
			}
		for(k = l; k <= r; k++) {
		a[k] = b[k];
	    }
	}
void parallelMergeSort(int *a, int *b, int l, int r) {
	
	int m;
	
	if(l < r) {
		
		m = (l + r)/2;
		
		parallelMergeSort(a, b, l, m);
		parallelMergeSort(a, b, (m + 1), r);
		parallelMerge(a, b, l, m, r);
		
		}
		
	}

/*compare function for the qsort*/
int cmpfunc (const void * a, const void * b) {
   return ( *(int*)a - *(int*)b );
}

int* hysteresis(int* suppressed, int height, int width){

    /*  initialize MPI, determine number of processes and rank. */
    MPI_Init(NULL, NULL);
    //Get the number of tasks
    MPI_Comm_size(MPI_COMM_WORLD, &comm_size);

    //Get my task #
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    
	int size = height*width/comm_size;
    int* hysteresisIm= malloc(height*width * sizeof(int)); 
    memcpy(hysteresisIm, suppressed, sizeof(int)*height*width);

    int* data= malloc(height*width * sizeof(int)); 
    memcpy(data, suppressed, sizeof(int)*height * width);



	//send each portion to the other processors
	int *array_piece = malloc(size * sizeof(int));
	MPI_Scatter(data, size, MPI_INT, array_piece, size, MPI_INT, 0, MPI_COMM_WORLD);
	
//parallel sort
	int *tmp_array = malloc(size * sizeof(int));
	parallelMergeSort(array_piece, tmp_array, 0, (size - 1));
	

//gather the sorted data up
	int *list = NULL;
	if(rank == 0) {
		
		list = malloc(size*comm_size * sizeof(int));
		
		}
	
	MPI_Gather(array_piece, size, MPI_INT, list, size, MPI_INT, 0, MPI_COMM_WORLD);
	
//sort the data for the last time
	if(rank == 0) {
		
		int *other_array = malloc(size * comm_size* sizeof(int));
		parallelMergeSort(list, other_array, 0, (height*width - 1));
        /*printf("This is the sorted array: ");
        int c;
		for(c = 0; c < num_elements; c++) {
			
			printf("%d ", sorted[c]);
			
			}*/
//fre the allocated memory
		free(other_array);
    }





    int loc = floor(.95*(height * width));
    int t_hi = list[loc];
    int t_lo = floor(.5*t_hi);

    int i;
    int j;

    for(i = 0; i < height; i++){
        for(j = 0; j < width; j++){
            if( hysteresisIm[i*width +j] > t_hi ){
                hysteresisIm[i*width +j] = 255;
            }
            else if (hysteresisIm[i*width +j] >t_lo){
                hysteresisIm[i*width +j] = 125;
            }
            else{
                hysteresisIm[i*width +j] = 0;
            }
        }
    }

	free(array_piece);
	free(tmp_array);
//printf("Leaving hysteresis\n");
return hysteresisIm;

}