/*
This will be the header file for the edge linking function
*/

#include "stdio.h" 
#include "math.h"
#include "stdlib.h"
#include "string.h"
#include "stdbool.h"


//Function declaration
bool anyNeighbor(int* hysteresis, int height, int width, int x, int y);
int *edgeLinking(int *hysteresis, int height, int width);

bool anyNeighbor(int* hysteresis, int height, int width, int x, int y){
//printf("Neighbor...\n");
    int i;
    int j;
  
    for (i = -1; i <= 1; i++){//may switch to 0->3 like the matlab
        for (j = -1; j <= 1; j++){
            if(x + i < height && x - i > 0 && y + j > width && y - j > 0){
                if(hysteresis[(i+x)*width +(j+y)] == 255){
                    return true;
                }else{
                    return false;
                }
            }
        }
    }

}



int *edgeLinking(int *hysteresis, int height, int width){

    //printf("Edge Linking...\n");

    int i; 
    int j; 

    int* edges= malloc(height*width * sizeof(int)); 
    memcpy(edges, hysteresis, sizeof(int)*height*width);   
    #pragma omp parallel for private(j)
    for (i = 0; i < height; i ++){
        
        for (j = 0; j  < width; j ++){
            if (hysteresis[i *width + j] == 125){
                if (anyNeighbor(hysteresis, height, width, i, j)){
                    edges[i*width +j] = 255;
                }
                else{
                  edges[i*width +j] = 0;  
                }
            }
        }
       // printf("Count: %d\n", i*width+j);
    }
    
    return edges;

}
