/*
 * main_aux.h
 *
 *  Created on: 29 αιεμ 2016
 *      Author: Maayan Sivroni
 */

typedef struct Img {
	int index; /*this will store the index of the image this feature belongs to */
	int hits; /* this will store the number of hits this img had */

}Img;

#include "KDTree.h"
#include "SPConfig.h"
#define _MAX 1024

#include "SPBPriorityQueue.h"

SPPoint *extractFromFiles(SPConfig config, int * size);
SPBPQueue initBPQ(SPConfig config);
void kNearestNeighbors(kdTree currNode, SPBPQueue bpq, SPPoint queryPoint);
int extractIndexFromQuery(char * query);
int * initCount(int numOfPics);
void destroyCount(int * array);
void addToCount(SPBPQueue bpq,int * allPicsCount);
Img * initImgArray(int * allPicsCount, int numOfPics);
int compareHits (const void * a, const void * b);
