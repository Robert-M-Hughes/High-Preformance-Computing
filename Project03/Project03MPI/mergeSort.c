/* COMP 137-1 - Spring 2018 - HW07
 * Merge sort using MPI.
 *
 * This initial version of the code just runs a serial version of
 * merge sort, although it is set up to use MPI. The only real MPI
 * functionality that is used if reading the comm size and accessing
 * the MPI timer.S
 *
 * For HW07: Parallelize the program. Leave the serial code (comm_sz == 1)
 *   as it is, in order to find timing data for the serial case.
 */
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <memory.h>

long num_elements;
int rank;
int comm_sz;

/* compute log_2 of a */
double logbase2(double a) {
   return log(a) / log(2.0);
}

/* fill array with random integers */
void generateData(long* data, int length, long max_value) {
    int i;
	srand((unsigned int)time(NULL));
	for (i = 0; i < length; i++) { data[i] = rand() % max_value; }
}

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

/* main */
int main(int argc,  char* argv[]) {

	double start_time, end_time;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

	if (argc != 2)
	{
		if (rank == 0)
			fprintf(stderr, "array size must be supplied as a command line argument.\n");
		MPI_Finalize();
		exit(1);
	}

	num_elements = atoi(argv[1]);

	if (comm_sz == 1)
	{
		printf("serial merge sort: sorting %d elements using %d processes\n", num_elements, comm_sz);
		long* data = (long*)malloc(num_elements*sizeof(long));
		generateData(data, num_elements, num_elements / 2);

		start_time = MPI_Wtime();
		serialMergeSort(data, num_elements);
		end_time = MPI_Wtime();

		fprintf(stdout, "serial merge sort: complete\n");
		fprintf(stdout, "Time: %lf seconds\n", end_time - start_time);

		if (verifyOrder(data, num_elements))
			fprintf(stdout, "Data was properly sorted.\n");
		else
			fprintf(stdout, "Data was not properly sorted.\n");

		free(data);
	}
	else
	{
//        printf("parallel merge sort: sorting %d elements using %d processes\n", num_elements, comm_sz);
		long* data = (long*)malloc(num_elements*sizeof(long));
		generateData(data, num_elements, num_elements / 2);
start_time = MPI_Wtime();		
	//divide the work into equal sizes so each knows how much to get
	int size = num_elements/comm_sz;
	
	//send each portion to the other processors
	int *array_piece = malloc(size * sizeof(int));
	MPI_Scatter(data, size, MPI_INT, array_piece, size, MPI_INT, 0, MPI_COMM_WORLD);
	
//parallel sort
	int *tmp_array = malloc(size * sizeof(int));
	parallelMergeSort(array_piece, tmp_array, 0, (size - 1));
	

//gather the sorted data up
	int *sorted = NULL;
	if(rank == 0) {
		
		sorted = malloc(num_elements * sizeof(int));
		
		}
	
	MPI_Gather(array_piece, size, MPI_INT, sorted, size, MPI_INT, 0, MPI_COMM_WORLD);
	
//sort the data for the last time
	if(rank == 0) {
		
		int *other_array = malloc(num_elements * sizeof(int));
		parallelMergeSort(sorted, other_array, 0, (num_elements - 1));
		
end_time = MPI_Wtime();

//show the sorted data
		printf("This is the sorted array: ");
        int c;
		for(c = 0; c < num_elements; c++) {
			
			printf("%d ", sorted[c]);
			
			}
			
		printf("\n");
		printf("\n");
		printf("Time took to sort %lf seconds\n", end_time-start_time);	
//fre the allocated memory
		free(sorted);
		free(other_array);
			
		}
	free(data);
	free(array_piece);
	free(tmp_array);
	
//end the MPI
	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Finalize();

	}


	return 0;
}


