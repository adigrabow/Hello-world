/*
 * KDArray.c
 *
 *  Created on: Jul 22, 2016
 *      Author: adigrabow
 */

#include "SPPoint.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "KDArray.h"
#include <string.h>

/******************
 Logger Messages
 *****************/
#define LOGGER_ERROR_KDARRAY_NULL ("Given KDArray is NULL\n")
#define LOGGER_ERROR_KDARRAY_INIT_NEGATIVE_SIZE ("KDArray Init function was called with negative size.\n")
#define LOGGER_ERROR_FAILED_TO_ALLOCATE_MEM_FOR_SPPOINT ("Failed to allocate memory for SPPoint array.\n")
#define LOGGER_ERROR_FAILED_TO_ALLOCATE_MEM_FOR_KDARRAY ("Failed to allocate memory for KDArray.\n")
#define LOGGER_ERROR_FAILED_TO_ALLOCATE_MEM_FOR_MATRIX ("Failed to allocate memory for matrix.\n")
#define LOGGER_ERROR_FAILED_TO_ALLOCATE_MEM_FOR_SORTED_POINT_ARR ("Failed to allocate memory for SortedPointArr.\n")
#define LOGGER_ERROR_FUNCTION_ARGUMENTS_FAILED_TO_MEET_CONSTRAINTS ("Function arguments failed to meet constraints.\n")
#define LOGGER_ERROR_FAILED_TO_ALLOCATE_MEMORY ("Failed to allocate memory.\n")


/****************************
 Help Functions Declarations
 ***************************/
/*
 * This function is called only from 'Split' and since no sorting is needed,
 * it creates a new KDArray in O(n) time.
 *
 * @param	arr				the kdArray
 * @param	X				help array (see more details in PDF)
 * @param	map				map array (see more details in PDF)
 * @param	side			a flag, 0 iff left side and 1 iff right side
 *
 * @return	kdArray			on success the kdarray
 * 								on failure NULL
 ***/
kdArray initFromSplit(kdArray arr, int* X,int * map,int side);


int COOR = 0; /* global? */


struct SPKDArray{
	SPPoint* pointArray;
	int size; /*the number of points int pointArray*/
	int dim; /*num of dimensions of KD-Tree (=num of coor of each point)*/
	int** mat; /* this is the d*size 2d-array that will be sorted by the coor of each row*/
};

//TODO new structs - sortedPoint,
/* In order to sort the points - temp struct */
typedef struct sortedPoint
{
	int index; /* contains the index of the point in pointArray */
	double data; /* contain a single double data[coor] */
}sortedPoint;

typedef struct sortedPoint * sortedPointsArr;
/* in order to create map1, map2 - every point has its original index in P */
typedef struct pointWithPIndex
{
	int index; /* contains the index of the point in pointArray */
	SPPoint point; /* contain a single double data[coor] */
}pointWithPIndex;

typedef struct pointWithPIndex * pointsWithIndexArr;


/********
 Getters
 ********/

int getSizeFromKDArray(kdArray arr){

	if (NULL == arr) {
		spLoggerPrintError(LOGGER_ERROR_KDARRAY_NULL,__FILE__, __func__, __LINE__ );
		return -1;
	}
	return arr->size;
}

SPPoint* getPointArrayFromKDArray(kdArray arr){

	if (NULL == arr) {
		spLoggerPrintError(LOGGER_ERROR_KDARRAY_NULL,__FILE__, __func__, __LINE__ );
		return NULL;
	}
	return arr->pointArray;
}

int** getMatrixFromKDArray(kdArray arr){

	if (NULL == arr) {
		spLoggerPrintError(LOGGER_ERROR_KDARRAY_NULL,__FILE__, __func__, __LINE__ );
		return NULL;
	}
	return arr->mat;
}

int getDimFromKDArray(kdArray arr){

	if (NULL == arr) {
		spLoggerPrintError(LOGGER_ERROR_KDARRAY_NULL,__FILE__, __func__, __LINE__ );
		return -1;
	}
	return arr->dim;
}

/*************************
 Functions Implementation
 ************************/

kdArray Init(SPPoint* arr, int size){

	if (NULL == arr) {
		spLoggerPrintError(LOGGER_ERROR_KDARRAY_NULL,__FILE__, __func__, __LINE__ );
		return NULL;
	}

	if(size <= 0) {
		spLoggerPrintError(LOGGER_ERROR_KDARRAY_INIT_NEGATIVE_SIZE,__FILE__, __func__, __LINE__ );
		return NULL;
	}


	/*Copy the array you received in the init function*/
	SPPoint* copiedArr = (SPPoint*)malloc(sizeof(SPPoint)* size);

	if (NULL == copiedArr) {
		spLoggerPrintError(LOGGER_ERROR_FAILED_TO_ALLOCATE_MEM_FOR_SPPOINT,__FILE__, __func__, __LINE__ );
		return NULL;
	}

	memcpy(copiedArr,arr, sizeof(SPPoint)* size);

	SPPoint* tempArray = (SPPoint*)malloc(sizeof(SPPoint)* size); /* for sorting */

	if(NULL == tempArray) {
		spLoggerPrintError(LOGGER_ERROR_FAILED_TO_ALLOCATE_MEM_FOR_SPPOINT,__FILE__, __func__, __LINE__ );
		free(copiedArr);
		return NULL;
	}

	memcpy(tempArray,arr, sizeof(SPPoint)* size);

	kdArray array = (kdArray)malloc(sizeof(struct SPKDArray));

	if(NULL == array) {
		spLoggerPrintError(LOGGER_ERROR_FAILED_TO_ALLOCATE_MEM_FOR_KDARRAY,__FILE__, __func__, __LINE__ );
		free(copiedArr);
		free(tempArray);
		return NULL;
	}

	array->size = size;
	array->pointArray = copiedArr;
	array->dim = spPointGetDimension(copiedArr[0]);

	int** mat = (int**)malloc(sizeof(int*)*(array->dim)); /* Allocate mat */
	if(NULL == mat) {
		spLoggerPrintError(LOGGER_ERROR_FAILED_TO_ALLOCATE_MEM_FOR_MATRIX,__FILE__, __func__, __LINE__ );
		free(copiedArr);
		free(tempArray);
		free(array);
		return NULL;
	}

	for(int i = 0; i < array->dim; i++){

		mat[i] = (int*)malloc(sizeof(int)*size);

		if(mat[i]==NULL){
			spLoggerPrintError(LOGGER_ERROR_FAILED_TO_ALLOCATE_MEM_FOR_MATRIX,__FILE__, __func__, __LINE__ );
			for (int k=0;k<i;k++){
				free(mat[k]);
			}
			free(copiedArr);
			free(tempArray);
			free(array);
			free(mat);
			return NULL;
		}

		sortedPointsArr sortByIndexArray = (sortedPointsArr) malloc(size * sizeof(sortedPoint));

		if (NULL == sortByIndexArray) {
			spLoggerPrintError(LOGGER_ERROR_FAILED_TO_ALLOCATE_MEM_FOR_SORTED_POINT_ARR,__FILE__, __func__, __LINE__ );
			free(copiedArr);
			free(tempArray);
			free(array);
			free(mat);
			return NULL;
		}

		/* initialize the sorting array */
		for(int j=0; j < size; j++)
		{
			sortByIndexArray[j].index = j;
			sortByIndexArray[j].data = spPointGetAxisCoor(array->pointArray[j], i);
		}

		/*	sort by data[i] */
		qsort(sortByIndexArray, size, sizeof(sortedPoint), compareSortPoints);

		for (int j=0; j<size; j++)
		{
			mat[i][j] = sortByIndexArray[j].index;
		}

		/* free allocated memory for sorting */
		free(sortByIndexArray);

	}

	array->mat = mat;
	free(tempArray);
	return array;

}


int compareSortPoints(const void * ptr1, const void * ptr2) {

	sortedPoint * a = (sortedPoint *)ptr1;
	sortedPoint * b = (sortedPoint *)ptr2;

	if (a->data > b->data) {
		return 1;
	}
	else if (a->data < b->data) {
		return -1;
	}


	else { /* in case of equality - choose the lower index */
		if (a->index > b->index) {
			return 1;
		}
		else if (a->index < b->index) {
			return -1;
		}

		return 0;
	}
}


kdArray * Split(kdArray kdArr, int coor) { /* coor = number of dimension that we split by */

	if ((NULL == kdArr) || (coor < 0)) { /* check for invalid arguments */
		spLoggerPrintError(LOGGER_ERROR_FUNCTION_ARGUMENTS_FAILED_TO_MEET_CONSTRAINTS,__FILE__, __func__, __LINE__ );
		return NULL;
	}

	SPPoint point = getPointArrayFromKDArray(kdArr)[0];

	SPPoint* P = (SPPoint*)malloc(sizeof(SPPoint)* getSizeFromKDArray(kdArr));
	if(NULL == P) {
		spLoggerPrintError(LOGGER_ERROR_FAILED_TO_ALLOCATE_MEM_FOR_SPPOINT,__FILE__, __func__, __LINE__ );
		return NULL;
	}

	memcpy(P,getPointArrayFromKDArray(kdArr), sizeof(SPPoint)* getSizeFromKDArray(kdArr));

	kdArray * result = (kdArray*) malloc(sizeof(struct SPKDArray) * 2); /* result array will include left and right array */
	if (NULL == result) {
		spLoggerPrintError(LOGGER_ERROR_FAILED_TO_ALLOCATE_MEM_FOR_KDARRAY,__FILE__, __func__, __LINE__ );
		free(P);
		return NULL;
	}

	int size = getSizeFromKDArray(kdArr);
	int halfIndex = (size + 1) / 2; /* half is index n/2 rounded up */
	int flag = 0; /* for creating maps : 0=not found in pi, else 1 */
	//TODO for all the arrays: maybe use malloc?

	int * X = (int *) malloc(sizeof(int)*size);
	if (NULL == X) {
		spLoggerPrintError(LOGGER_ERROR_FAILED_TO_ALLOCATE_MEMORY,__FILE__, __func__, __LINE__ );
		free(P);
		free(result);
		return NULL;
	}

	int * map1 = (int *) malloc(sizeof(int)*size);
	if (NULL == map1) {
		spLoggerPrintError(LOGGER_ERROR_FAILED_TO_ALLOCATE_MEMORY,__FILE__, __func__, __LINE__ );
		free(P);
		free(X);
		free(result);
		return NULL;
	}

	int * map2 = (int *) malloc(sizeof(int)*size);
	if (NULL == map2) {
		spLoggerPrintError(LOGGER_ERROR_FAILED_TO_ALLOCATE_MEMORY,__FILE__, __func__, __LINE__ );
		free(P);
		free(X);
		free(result);
		free(map1);
		return NULL;
	}

	SPPoint * P1 = (SPPoint*)malloc(halfIndex * sizeof(SPPoint));
	if(NULL == P1) {
		spLoggerPrintError(LOGGER_ERROR_FAILED_TO_ALLOCATE_MEMORY,__FILE__, __func__, __LINE__ );
		free(P);
		free(X);
		free(result);
		free(map1);
		free(map2);
		return NULL;
	}

	SPPoint * P2 = (SPPoint*)malloc((size - halfIndex) * sizeof(SPPoint));
	if (NULL == P2) {
		spLoggerPrintError(LOGGER_ERROR_FAILED_TO_ALLOCATE_MEMORY,__FILE__, __func__, __LINE__ );
		free(P);
		free(X);
		free(result);
		free(map1);
		free(map2);
		free(P1);
		return NULL;
	}
	kdArray Left;
	kdArray Right;
	int i;

	/* if (side = 0) the we build the left half, (side = 1) for the right side. Used in initFromSplit(X,map-i,side) */
	int side;
	for (int i=0; i<size; i++){ /* Build X array */ /* use getMatrix instead of kdArr->mat */
		if (i< halfIndex){
			/* elements in Left */
			X[(getMatrixFromKDArray(kdArr))[coor][i]] = 0;
		}
		else{ /* elements in Right */
			X[(getMatrixFromKDArray(kdArr))[coor][i]] = 1 ;
		}
	}

	i=0;
	int k = 0;
	for (int j = 0; j < size; j++){
		if (X[j] == 0){
			P1[i] = P[j];
			i++;

		}else{
			P2[k] = P[j];
			k++;
		}
	}

	// test to new p1, p2
	///////////////////////////////
	pointsWithIndexArr  P1test = (pointsWithIndexArr)malloc(halfIndex * sizeof(pointWithPIndex));
	if (!P1test){
		//Alloc error
		return NULL;
	}
	pointsWithIndexArr P2test = (pointsWithIndexArr)malloc((size - halfIndex) * sizeof(pointWithPIndex));
	if (!P2test){
		//Alloc error
		free(P1test);
		return NULL;
	}
	i=0;
	k = 0;
	for (int j = 0; j < size; j++){
		if (X[j] == 0){
			P1test[i].point = spPointCopy(P[j]);
			P1test[i].index = j; /*index in P - main array */
			i++;
		}else{
			P2test[k].point = spPointCopy(P[j]);
			P2test[k].index = j;
			k++;
		}
	}


	/* Build map1, map2 - arrays including indexes of points if point is in map-i and (-1) otherwise */
	//TODO new
	//TODO we need a new struct that will include the point and its index in P !! */
	pointsWithIndexArr pointsWithIndexArray = (pointsWithIndexArr) malloc(size* sizeof(pointWithPIndex));
	if (!pointsWithIndexArray){
		// Allocation error
		return NULL;
	}
	for(i=0;i<size;i++){
		pointsWithIndexArray[i].index = i;
		pointsWithIndexArray[i].point = spPointCopy(P[i]);
	}
	//TODO now we need to  build P1 P2 using the new struct
	/* Build map1 */
	for (i=0; i<size;i++){ /* init default values */
		map1[i] = -1;
	}
	for (i=0; i<halfIndex;i++){
		//map1[ spPointGetIndex(P1[i])] = i;
		map1[ P1test[i].index] = i;
	}

	/* Build map2 */
	for (i=0; i<size;i++){ /* init default values */
		map2[i] = -1;
	}
	for (i=0; i<(size - halfIndex);i++){
		//map2[ spPointGetIndex(P2[i])] = i;
		map2[ P2test[i].index] = i;
	}

	/* free pointsWithIndexArray */
	for (i=0;i<size;i++){
		free(pointsWithIndexArray[i].point);

	}
	free(pointsWithIndexArray);

	/* free p1test */
	for (i=0;i<halfIndex;i++){
		free(P1test[i].point);

	}
	free(P1test);

	/* free p2test */
	for (i=0;i<(size - halfIndex);i++){
		free(P2test[i].point);

	}
	free(P2test);


	/* Build left and right */
	side = 0; /* indicator for left side in init function */
	Left = initFromSplit(kdArr,X,map1,side); /* create Left */
	side = 1; /* indicator for right side in init function */
	Right = initFromSplit(kdArr,X,map2,side); /* create Right */

	/* free allocations */
	free(X);
	free(map1);
	free(map2);
	free(P);

	/* update result */
	result[0] = Left;
	result[1] = Right;
	return result;

}

kdArray initFromSplit(kdArray arr, int* X,int * map,int side){

	int i = 0;

	/* Calculate size of kdArray */
	int size;
	if (side == 0) /* Left side size */
		size = (arr->size + 1) / 2;
	else /* Right side size */
		size = arr->size - ((arr->size + 1) / 2);

	/*Copy the point-array we received in the init function*/
	SPPoint* copiedPointArr = (SPPoint*)malloc(sizeof(SPPoint)* arr->size); /* copy original point array */

	if (NULL == copiedPointArr) {
		spLoggerPrintError(LOGGER_ERROR_FAILED_TO_ALLOCATE_MEM_FOR_SPPOINT,__FILE__, __func__, __LINE__ );
		return NULL;
	}

	memcpy(copiedPointArr,arr->pointArray, sizeof(SPPoint)* arr->size);
	kdArray array = (kdArray)malloc(sizeof(struct SPKDArray)); /* output array */

	if(NULL == array) {
		spLoggerPrintError(LOGGER_ERROR_FAILED_TO_ALLOCATE_MEM_FOR_KDARRAY,__FILE__, __func__, __LINE__ );
		free(copiedPointArr);
		return NULL;
	}

	array->size = size;
	SPPoint* pointArr = (SPPoint*)malloc(sizeof(SPPoint)* size); /* allocate new point array */

	if(NULL == pointArr) {
		spLoggerPrintError(LOGGER_ERROR_FAILED_TO_ALLOCATE_MEM_FOR_SPPOINT,__FILE__, __func__, __LINE__ );
		free(copiedPointArr);
		free(array);
		return NULL;
	}

	/* for each index in X - if (X[i] == side ) then pointArr[ map[i] ] =  copiedPointArr[i] */
	for (i = 0;i < (arr->size);i++){
		if (X[i] == side){
			pointArr[map[i]] = copiedPointArr[i];
		}
	}

	array->pointArray = pointArr;
	array->dim = spPointGetDimension(copiedPointArr[0]);
	int** mat = (int**)malloc(sizeof(int*)*(array->dim));

	if(NULL == mat) {
		spLoggerPrintError(LOGGER_ERROR_FAILED_TO_ALLOCATE_MEM_FOR_MATRIX,__FILE__, __func__, __LINE__ );
		free(copiedPointArr);
		free(pointArr);
		free(array);
		return NULL;
	}

	for(int i = 0; i < array->dim; i++){
		mat[i] = (int*)malloc(sizeof(int)*size);
		if(mat[i]==NULL){
			spLoggerPrintError(LOGGER_ERROR_FAILED_TO_ALLOCATE_MEM_FOR_MATRIX,__FILE__, __func__, __LINE__ );
			for (int k=0;k<i;k++){
				free(mat[k]);
			}
			free(copiedPointArr);
			free(pointArr);
			free(array);
			free(mat);
			return NULL;
		}
	}

	int row = -1, col = 0;
	for(int i = 0; i < arr->dim ; i++){
		col = 0;
		row++;
		for(int j = 0; j < arr->size ; j++){
			/*build right side*/
			if((X[arr->mat[i][j]] == 1) && (side == 1)){
				mat[row][col] = map[arr->mat[i][j]];
				col++;

			}
			/*build left side*/
			if(X[arr->mat[i][j]] == 0 && (side == 0)){
				mat[row][col] = map[arr->mat[i][j]];
				col++;
			}
		}
	}


	/* maybe now we can use free(copiedPointArr) ?? */

	array->mat = mat;
	return array;

}


void destroyKdArray(kdArray arr){

	if (NULL == arr) {
		return;
	}

	for(int i = 0; i < arr->dim; i++){ //maybe arr->dim ?
		free(arr->mat[i]);
	}
	free(arr->mat);
	free(arr->pointArray);
	free(arr);
}
