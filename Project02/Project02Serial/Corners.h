/* 
Header file for the Corners functon
*/

#include "stdio.h" 
#include "math.h"
#include "stdlib.h"
#include "string.h"

//Function Declaration
int* cornersList(int* vertical, int* horizontal, int height, int width, int window);


//Function:
int* cornersList(int* vertical, int* horizontal, int height, int width, int window){

    int i, j, e, k, ixx, iyy, ixiy;
    int* cornerness = malloc(height*width*sizeof(int));

    for(i = 0; i < height; i ++){
        for(j = 0; j < width; j ++){
            ixx = 0;
            iyy = 0;
            ixiy = 0;
            for( e = -window/2; e < window/2; e++){
                for( k = -window/2; k < window/2; k++){
                    if(i+e < height && i+e > 0 && j+k < width && j+k > 0){
                        ixx += pow(vertical[(i+e)*width +(j+k)], 2);
                        iyy += pow(horizontal[(i+e)*width +(j+k)], 2);
                        ixiy += vertical[(i+e)*width +(j+k)] * horizontal[(i+e)*width +(j+k)];
                        
                    }
                }
            }
            cornerness[i*width+j] = (ixx * iyy - ixiy * ixiy) - .4*(ixx+iyy)*(ixx + iyy);
        }
    }
    return cornerness;
}
