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


int main(){

	SP_KDTREE_SPLIT_METHOD_TYPE splitMethod = MAX_SPREAD;
	double data1[2] = {1999.0,1.0};
	double data2[2]= {29999.0,2.0};
	double data3[2]= {399999.0,3.0};
	double data4[2]= {100.0,100.0};
	double data5[2]= {200.0,200.0};
	double queryData[2]= {0.0,0.0};


	SPPoint p1 = spPointCreate(data1, 2, 0);
	SPPoint p2 = spPointCreate(data2, 2, 1);
	SPPoint p3 = spPointCreate(data3, 2, 2);
	SPPoint p4 = spPointCreate(data4, 2, 3);
	SPPoint p5 = spPointCreate(data5, 2, 4);

	SPPoint SPPointArr[5] = {p1,p2,p3,p4,p5};
	kdArray resultArray = Init(SPPointArr , 5);
	srand(time(NULL));
	int size = -1;
	int * ptr = &size;

	kdTree tree = init(resultArray, ptr, splitMethod);
	SPBPQueue bpq = spBPQueueCreate(3);
	SPPoint queryPoint = spPointCreate(queryData, 2, 5);
	kNearestNeighbors(tree, bpq, queryPoint);

	printQueue(bpq);

	spPointDestroy(p1);
	spPointDestroy(p2);
	spPointDestroy(p3);
	spPointDestroy(p4);
	spPointDestroy(p5);
	spPointDestroy(queryPoint);
	destroyKdTree(tree);

	return 0;

}

/* Use malloc to create the point array.
 * For every image go to .feats and read each line (but the first...)
 * the index of each point will be shown in line 0 + total numOfFeat lines.
 * Use realloc() to change the size of the point array (keep track of the previous size)
 */

SPPoint *extractFromFiles(SPConfig config, int * size){ // WORKING
	SP_CONFIG_MSG msg = SP_CONFIG_SUCCESS;
	SPPoint* pointArray = (SPPoint*) malloc(sizeof(SPPoint)* 1);
	/* We return an array of all the points (all features) */
	if (!pointArray){
		// PRINT ERROR?
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
	/* tmp double array containing data to init point - size=dim*/
	if (!data){
		// PRINT ERROR?
	}

	SPPoint point = NULL; /*point extracted */
	int point_index = 0; /* current index in pointArray */
	//SPPoint spPointCreate(double* data, int dim, int index)

	for (i=0; i<numOfPics; i++){ /* for each image */
		// create featPath (the string) - for the i-th picture
		spConfigGetImagePathFeat(featPath,config,i);
		fp = fopen(featPath, "r+");
		if (!fp){
			// ERROR
		}
		/* Get parameters from first line - index and actual number of extracted features  */
		fgets(line, _MAX, fp);
		cNumofFeat = strtok(line, s);
		numOfFeats = atoi(cNumofFeat); /* convert to int */
		cindex = strtok(NULL, s);
		index = atoi(cindex); /* convert to int */

		totalSize += numOfFeats; /* increase size for realloc */
		pointArray = (SPPoint *) realloc(pointArray, totalSize* sizeof(SPPoint));
		if (!pointArray){
			//ERROR
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
	//size = &totalSize;
	(*size) = totalSize;
	return pointArray;
}



SPBPQueue initBPQ(SPConfig config){
	/* Maintain a BPQ of the candidate nearest neighbors, called 'bpq', Set the maximum size of 'bpq' to spKNN*/
	return spBPQueueCreate(spConfigGetspKNN(config));
}



void kNearestNeighbors(kdTree currNode, SPBPQueue bpq, SPPoint queryPoint){
	if (NULL == currNode) {
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
	//	double queryPointAxisCoor =  spPointGetAxisCoor(queryPoint, currDimension);

	/*if currNode is leaf
	 *  Add the current point to the BPQ. Note that this is a no-op
	 *  if the * point is not as good as the points we've seen so far.*/
	if (NULL != currPoint) {


		if (spPointGetDimension(currPoint) != spPointGetDimension(queryPoint)) {
			//TODO add logger
			return;
		}
		double distance = spPointL2SquaredDistance(currPoint, queryPoint);
		//		double distance = abs(currPointAxisCoor - queryPointAxisCoor);
		element = spListElementCreate(spPointGetIndex(currPoint), distance);

		if (NULL == element){
			//TODO add logger
			return;
		}

		bpqReruenMsg = spBPQueueEnqueue(bpq, element);
		if (bpqReruenMsg != SP_BPQUEUE_SUCCESS) {
			spListElementDestroy(element);
			//TODO add logger
			return;
		}

		spListElementDestroy(element);

		return;
	}

	/* Recursively search the half of the tree that contains the test point. */
	chosenChild = (queryPointAxisCoor <= currNodeVal) ? kdTreeGetLeft(currNode) : kdTreeGetRight(currNode);
	otherChild = (queryPointAxisCoor <= currNodeVal) ? kdTreeGetRight(currNode) : kdTreeGetLeft(currNode);

	if (NULL == chosenChild) {
		//TODO add logger
		return;
	}

	kNearestNeighbors(chosenChild, bpq, queryPoint);

	if ((!spBPQueueIsFull(bpq)) || (pow((currNodeVal - queryPointAxisCoor),2) < bpqMaxVal )) {
		kNearestNeighbors(otherChild, bpq, queryPoint);
	}


}


void addToCount(SPBPQueue bpq,int * allPicsCount){
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

	/* find last appearance of /  */
	char * tmp = strtok(query, s1);
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
		int* result = (int*) malloc(sizeof(int)* numOfPics);
		if (!result){
			// PRINT ERROR?
		}
		return result;
	}
	else{
		printf("numOfPics is negative\n");
		return NULL;
	}
}

void destroyCount(int * array){// WORKING

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
	if ( !hitsArray ){
		//Allocation error
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
