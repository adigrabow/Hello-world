/*
 * main_aux.c
 *
 *  Created on: 29 αιεμ 2016
 *      Author: Maayan Sivroni
 */

#include "SPConfig.h"
//#include "SPPoint.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "KDArray.h"
#include "SPBPriorityQueue.h"
#include <limits.h> /* for INT_MIN, INT_MAX - for function extractIndexFromQuery */
#include "main_aux.h"
#include <string.h>

#define LEN_OF_CHAR 2

/****************
 Logger Messages
 ****************/
#define LOGGER_ERROR_FAILED_TO_ALLOCATE_MEM_FOR_SPPOINT ("Failed to allocate memory for SPPoint array.\n")
#define LOGGER_ERROR_FAILED_TO_OPEN_FILE ("Failed to open file.\n")
#define LOGGER_ERROR_FAILED_TO_ALLOCATE_MEMORY ("Failed to allocate memory.\n")
#define LOGGER_ERROR_FAILED_TO_CREATE_BPQ ("Failed to create bpq.\n")
#define LOGGER_ERROR_FUNCTION_ARGUMENT_IS_EMPTY ("Function received empty parameter.\n")
#define LOGGER_ERROR_K_NEAREST_NEIGHBORS_CUR_NODE_AND_QUERY_HAVE_DIFFERENT_DIM ("The dim of query SPPoint is different from the dim of the current node SPPoint.\n")
#define LOGGER_ERROR_FAILED_TO_CREATE_SP_LIST_ELEMENT ("Failed to create SPListElement.\n")
#define LOGGER_ERROR_FAILED_TO_ENQUEUE_TO_BPQ ("Failed to enqueue to BPQ.\n")
#define LOGGER_ERROR_KDTREE_CHOSEN_CHILD_IS_NULL ("KDTree 'ChosenChild' is NULL.\n")
#define LOGGER_ERROR_NEGATIVE_NUNMBER_OF_PICS ("'Number Of Pictures' variable was negative.\n")


/* Use malloc to create the point array.
 * For every image go to .feats and read each line (but the first...)
 * the index of each point will be shown in line 0 + total numOfFeat lines.
 * Use realloc() to change the size of the point array (keep track of the previous size)
 */


SPPoint *extractFromFiles(SPConfig config, int * size){ // WORKING

	SP_CONFIG_MSG msg = SP_CONFIG_SUCCESS;
	SPPoint* pointArray = (SPPoint*) malloc(sizeof(SPPoint)* 1);
	/* We return an array of all the points (all features) */

	if (NULL == pointArray) {
		spLoggerPrintError(LOGGER_ERROR_FAILED_TO_ALLOCATE_MEM_FOR_SPPOINT,__FILE__, __func__, __LINE__ );
		return NULL;
	}

	int totalSize = 0; /*size of the point array */
	int numOfPics = spConfigGetNumOfImages(config, &msg); /* number of images */
	int dim = spConfigGetPCADim(config,&msg);
	//char* imagePath; /* saves the image path that was created in  spConfigGetImagePath */ //TODO delete?
	char featPath [_MAX]; /* will contain the path for image .feat */
	FILE *fp; /* the file created .feat per image */
	int i,j,k;
	const char s[LEN_OF_CHAR] = "#"; /* Delimiter */
	char line [_MAX]; /* line extracted from file.feat */
	char * c_double; /* for each double value - to be converted to double */
	char * cNumofFeat; /* numOfFeat extracted from file as string */
	char * cindex; /* index extracted from file as string */
	int numOfFeats; /* numOfFeats after convert to int */
	int index; /* index after convert to int */

	double * data = (double *) calloc (dim,sizeof(double));

	if (NULL == data) {
		spLoggerPrintError(LOGGER_ERROR_FAILED_TO_ALLOCATE_MEMORY,__FILE__, __func__, __LINE__ );
		return NULL;
	}

	SPPoint point = NULL; /*point extracted */
	int point_index = 0; /* current index in pointArray */

	for (i=0; i<numOfPics; i++){ /* for each image */
		// create featPath (the string) - for the i-th picture
		spConfigGetImagePathFeat(featPath,config,i);
		fp = fopen(featPath, "r+");

		if (NULL == fp) {
			spLoggerPrintError(LOGGER_ERROR_FAILED_TO_OPEN_FILE,__FILE__, __func__, __LINE__ );
			return NULL;
		}

		/* Get parameters from first line - index and actual number of extracted features  */
		fgets(line, _MAX, fp);
		cNumofFeat = strtok(line, s);
		numOfFeats = atoi(cNumofFeat); /* convert to int */
		cindex = strtok(NULL, s);
		index = atoi(cindex); /* convert to int */

		totalSize += numOfFeats; /* increase size for realloc */
		pointArray = (SPPoint *) realloc(pointArray, totalSize* sizeof(SPPoint));

		if (NULL == pointArray) {
			spLoggerPrintError(LOGGER_ERROR_FAILED_TO_ALLOCATE_MEM_FOR_SPPOINT,__FILE__, __func__, __LINE__ );
			return NULL;
		}

		for (k=0; k<numOfFeats; k++){ /* each row is a new point to create  */
			// consider that every line ends with # also
			/* extract all data (double) */
			fgets(line, _MAX, fp); /* get line */
			c_double = strtok(line, s); /* get first double */
			j=0;
			data[j] = atof(c_double); /* convert to double  and add to data */
			/* walk through other tokens */
			//VERSION WITH WHILE LOOP
			/*while( c_double != NULL ) {
				j++;
				c_double = strtok(NULL, s);
				data[j] = atof(c_double);
			}*/

			//VERSION WITH FOR LOOP
			for(j=1;j<dim;j++){
				c_double = strtok(NULL, s);
				data[j] = atof(c_double);
			}

			point = spPointCreate(data,dim,index);
			pointArray[point_index] = point;
			point_index++;

		}

		fclose(fp);
	}
	free(data);
	(*size) = totalSize;
	return pointArray;
}


/* Maintain a BPQ of the candidate nearest neighbors, called 'bpq', Set the maximum size of 'bpq' to spKNN*/
SPBPQueue initBPQ(SPConfig config){

	SPBPQueue bpq = NULL;
	bpq = spBPQueueCreate(spConfigGetspKNN(config));

	if (NULL == bpq) {
		spLoggerPrintError(LOGGER_ERROR_FAILED_TO_CREATE_BPQ,__FILE__, __func__, __LINE__ );
		return NULL;
	}

	return bpq;
}

void kNearestNeighbors(kdTree currNode, SPBPQueue bpq, SPPoint queryPoint){

	if (NULL == currNode || NULL == bpq || NULL == queryPoint ) {
		spLoggerPrintError(LOGGER_ERROR_FUNCTION_ARGUMENT_IS_EMPTY,__FILE__, __func__, __LINE__ );
		return;
	}

	SPPoint currPoint = kdTreeGetData(currNode);
	SPListElement element = NULL;
	kdTree chosenChild = NULL;
	kdTree otherChild = NULL;
	SP_BPQUEUE_MSG bpqReruenMsg = SP_BPQUEUE_SUCCESS;

	double bpqMaxVal = spBPQueueMaxValue(bpq);
	double currNodeVal = kdTreeGetVal(currNode);
	int currDimension = kdTreeGetDimension(currNode);
	double queryPointAxisCoor = spPointGetAxisCoor(queryPoint,currDimension );

	/*if currNode is leaf:
	 *  Add the current point to the BPQ. Note that this is a no-op
	 *  if the * point is not as good as the points we've seen so far.*/
	if (NULL != currPoint) {

		if (spPointGetDimension(currPoint) != spPointGetDimension(queryPoint)) {
			spLoggerPrintError(LOGGER_ERROR_K_NEAREST_NEIGHBORS_CUR_NODE_AND_QUERY_HAVE_DIFFERENT_DIM,
					__FILE__, __func__, __LINE__ );
			return;
		}

		double distance = spPointL2SquaredDistance(currPoint, queryPoint);
		element = spListElementCreate(spPointGetIndex(currPoint), distance);

		if (NULL == element){
			spLoggerPrintError(LOGGER_ERROR_FAILED_TO_CREATE_SP_LIST_ELEMENT,
					__FILE__, __func__, __LINE__ );
			return;
		}

		bpqReruenMsg = spBPQueueEnqueue(bpq, element);
		if (bpqReruenMsg != SP_BPQUEUE_SUCCESS) {
			spLoggerPrintError(LOGGER_ERROR_FAILED_TO_ENQUEUE_TO_BPQ,
					__FILE__, __func__, __LINE__ );
			spListElementDestroy(element);
			return;
		}

		spListElementDestroy(element);

		return;
	}

	/* Recursively search the half of the tree that contains the test point. */
	chosenChild = (queryPointAxisCoor <= currNodeVal) ? kdTreeGetLeft(currNode) : kdTreeGetRight(currNode);
	otherChild = (queryPointAxisCoor <= currNodeVal) ? kdTreeGetRight(currNode) : kdTreeGetLeft(currNode);

	if (NULL == chosenChild) {
		spLoggerPrintError(LOGGER_ERROR_KDTREE_CHOSEN_CHILD_IS_NULL,
				__FILE__, __func__, __LINE__ );
		return;
	}

	kNearestNeighbors(chosenChild, bpq, queryPoint);

	if ((!spBPQueueIsFull(bpq)) || (pow((currNodeVal - queryPointAxisCoor),2) < bpqMaxVal )) {
		kNearestNeighbors(otherChild, bpq, queryPoint);
	}


}


void addToCount(SPBPQueue bpq,int * allPicsCount){

	if (NULL == allPicsCount || NULL == bpq) {
		spLoggerPrintError(LOGGER_ERROR_FUNCTION_ARGUMENT_IS_EMPTY,__FILE__, __func__, __LINE__ );
		return;
	}

	int size = spBPQueueSize(bpq);
	SPListElement element;

	for (int i = 0; i < size; i++){
		element = spBPQueuePeek(bpq); /* look at one element */
		allPicsCount[spListElementGetIndex(element)]++; /*increase count for each image - each index */
		spBPQueueDequeue(bpq); /* dequeue from queue */

	}
}

int extractIndexFromQuery(char * query){ //WORKING
	//printf("query: ./images/img10.png, printed: %s\n", query);
	/* extract index of image: between last `/` and last `.` */
	/* EXAMPLE: "./images/img10.png" ====> 10 */
	const char s1[LEN_OF_CHAR] = "/"; /* Delimiter 1 */
	const char s2[LEN_OF_CHAR] = "."; /* Delimiter 2 */

	int index; /* output */
	char * str;
	char tmpQuery[_MAX];
	strcpy(tmpQuery,query);
	/* find last appearance of /  */
	char * tmp = strtok(tmpQuery, s1);
	//printf("%s!!",tmp);
	while( tmp != NULL ) {
		tmp = strtok(NULL, s1);
		if (tmp !=NULL){
			str = tmp;
		}
	}
	/* now str == img10.png */
	//printf("goal: img10.png, printed: %s\n", str);
	char * tmp2 = strtok(str, s2); /* get string before . */
	//printf("goal: img10, printed: %s\n", tmp2);


	sscanf(tmp2, "%*[^0123456789]%d", &index);
	return index;


}

int * initCount(int numOfPics){// WORKING
	if(numOfPics >= 0){
		int * result = (int *) calloc(numOfPics,sizeof(int));

		if (NULL == result) {
			spLoggerPrintError(LOGGER_ERROR_FAILED_TO_ALLOCATE_MEMORY,
					__FILE__, __func__, __LINE__ );
			return NULL;
		}
		return result;
	}
	else{
		spLoggerPrintError(LOGGER_ERROR_NEGATIVE_NUNMBER_OF_PICS,
				__FILE__, __func__, __LINE__ );
		return NULL;
	}
}

void destroyCount(int * array){// WORKING

	if ( NULL == array) {
		return;
	}

	free(array);

}



int compareHits (const void * a, const void * b){//WORKING

	struct Img * f1 = (Img *) a;
	struct Img * f2 = (Img *) b;

	return(f2->hits - f1->hits);

}

Img * initImgArray(int * allPicsCount, int numOfPics){ //WORKING
	/* Convert from (int *) to (Img *) and order by hits */
	Img * hitsArray = (Img *) malloc(numOfPics* sizeof(Img)); /*an array with "num" entries - to count the hits*/

	if ( NULL == hitsArray ) {
		spLoggerPrintError(LOGGER_ERROR_FAILED_TO_ALLOCATE_MEMORY,
				__FILE__, __func__, __LINE__ );
		return NULL;
	}

	/*initialize struct array with empty images*/
	for(int i = 0; i < numOfPics; i++){
		hitsArray[i].index = i;
		hitsArray[i].hits = 0;
	}
	/* update Img element with the correct hits */
	for(int j = 0; j < numOfPics; j++){
		hitsArray[j].hits = allPicsCount[j];
	}

	qsort(hitsArray, numOfPics, sizeof(Img),compareHits);
	return hitsArray;

}
