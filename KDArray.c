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
/*getters*/

int getSizeFromKDArray(kdArray arr){
	assert(arr != NULL);
	return arr->size;
}

SPPoint* getPointArrayFromKDArray(kdArray arr){
	if(arr == NULL){
		printf("arr == NULL\n");
	}
	assert(arr != NULL);
	return arr->pointArray;
}

int** getMatrixFromKDArray(kdArray arr){
	assert(arr != NULL);
	return arr->mat;
}

int getDimFromKDArray(kdArray arr){
	assert(arr != NULL);
	return arr->dim;
}


kdArray Init(SPPoint* arr, int size){

	if(arr == NULL){
		//printf("input array is NULL, Exiting...\n");
		return NULL;
	}

	if(size <= 0){
		return NULL;
	}


	/*Copy the array you received in the init function*/
	SPPoint* copiedArr = (SPPoint*)malloc(sizeof(SPPoint)* size);
	if(!copiedArr){
		return NULL;
	}
	memcpy(copiedArr,arr, sizeof(SPPoint)* size);

	SPPoint* tempArray = (SPPoint*)malloc(sizeof(SPPoint)* size); /* for sorting */
	if(!tempArray){
		free(copiedArr);
		return NULL;
	}
	memcpy(tempArray,arr, sizeof(SPPoint)* size);

	kdArray array = (kdArray)malloc(sizeof(struct SPKDArray));
	if(array == NULL){
		free(copiedArr);
		free(tempArray);
		return NULL;
	}

	array->size = size;
	array->pointArray = copiedArr;
	array->dim = spPointGetDimension(copiedArr[0]);

	int** mat = (int**)malloc(sizeof(int*)*(array->dim)); /* Allocate mat */
	if(mat == NULL){
		free(copiedArr);
		free(tempArray);
		free(array);
		return NULL;
		}

	for(int i = 0; i < array->dim; i++){
		mat[i] = (int*)malloc(sizeof(int)*size);
		if(mat[i]==NULL){ /* Allocation error */
			 for (int k=0;k<i;k++){
				free(mat[k]);
				}
			 free(copiedArr);
			 free(tempArray);
			 free(array);
			 free(mat);
			 return NULL;
		}

		//COOR = i;
		//qsort(tempArray, array->size, sizeof(SPPoint), coorCompare);
		/* make sure that "index" field of SPPoint is the same as the order-index of the input array (Maayan knows :) )    */
		/* we changed pointArray - is this OKAY?*/

		//int equal = 0; /*if equal==0 then the points arent the same, 1 means equality */
		//for(int j = 0; j < size; j++){
		//	mat[i][j] = spPointGetIndex(tempArray[j]);
			//TODO this is NOT the correct index! we need the index of the point from  pointArray


		//}


		//int* resultArr = NULL;
		sortedPointsArr sortByIndexArray = (sortedPointsArr) malloc(size * sizeof(sortedPoint));

		if (!sortByIndexArray) {
			//Allocation error
			return NULL;
		}

		/*resultArr = (int*) malloc(size * sizeof(int)); //TODO maybe delete malloc here?
		if (NULL == resultArr) {
		//Allocation error
			free(sortingArr);
			return NULL;
		}*/

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
			//resultArr[j] = sortingArr[j].index;
			mat[i][j] = sortByIndexArray[j].index;
		}

		/* free allocated memory for sorting */
		free(sortByIndexArray);

		//mat[i] =  resultArr; // mat[i]
		}

	array->mat = mat;
	free(tempArray);
	return array;

}

int coorCompare(const void * a, const void* b){//TODO delete

	SPPoint p1 = *(SPPoint*) a;
	SPPoint p2 = *(SPPoint*) b;

/*

	printf("p1[0] = %f\n",spPointGetAxisCoor(p1, 0));
	printf("p2[0] = %f\n",spPointGetAxisCoor(p2, 0));
	printf("p1 dim = %d\n",spPointGetDimension(p1));
	printf("p2 dim = %d\n",spPointGetDimension(p2));*/



	return compareSPPoints(p1,p2,COOR);


}


int compareSPPoints(SPPoint p1, SPPoint p2 , int coordinate){ //TODO delete
//	printf("entered compareSPPoints\n");
//	printf("coordinate = %d\n", coordinate);
	double num1 = spPointGetAxisCoor(p1,coordinate);
	double num2 = spPointGetAxisCoor(p2,coordinate);


	//printf("compareSPPoints result is %f\n", num1 - num2);
	if(num1 > num2){
		return 1;
	}
	if(num1 < num2){
		return -1;
	}
	return 0;


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




/* important comments:
 * Add documentation in header file.
 * init:
 * 	2. Check invalid arguments at first.
 * 	3. Create destroy function.
 * 	4. I included point.c because eclipse didn't recognize the struct.
 * Split:
 * 	1. Check types: requested SPKDArray and not kdArray... maybe we should change typedef?
 * 	2. Use getters (and create them for each field in struct)
 *
 */

kdArray * Split(kdArray kdArr, int coor) { /* coor = number of dimension that we split by */
	if ((!kdArr) || (coor < 0)) { /* check for invalid arguments */
		return NULL;
	}

	SPPoint point = getPointArrayFromKDArray(kdArr)[0];

	SPPoint* P = (SPPoint*)malloc(sizeof(SPPoint)* getSizeFromKDArray(kdArr));
	if(!P){
		return NULL;
	}
	memcpy(P,getPointArrayFromKDArray(kdArr), sizeof(SPPoint)* getSizeFromKDArray(kdArr));

	kdArray * result = (kdArray*) malloc(sizeof(struct SPKDArray) * 2); /* result array will include left and right array */
	if (!result) { /* Allocation error */
		return NULL;
	}


	int size = getSizeFromKDArray(kdArr);
	int halfIndex = (size + 1) / 2; /* half is index n/2 rounded up */
	int flag = 0; /* for creating maps : 0=not found in pi, else 1 */
	//TODO for all the arrays: maybe use malloc?


	int * X = (int *) malloc(sizeof(int)*size);
	if (!X) { /* Allocation error */
		free(result);
		return NULL;
	}

	int * map1 = (int *) malloc(sizeof(int)*size);
	if (!map1) { /* Allocation error */
		free(X);
		free(result);
		return NULL;
	}

	int * map2 = (int *) malloc(sizeof(int)*size);
	if (!map2) { /* Allocation error */
		free(map1);
		free(X);
		free(result);
		return NULL;
	}

	SPPoint * P1 = (SPPoint*)malloc(halfIndex * sizeof(SPPoint));
	SPPoint * P2 = (SPPoint*)malloc((size - halfIndex) * sizeof(SPPoint));
	kdArray Left;
	kdArray Right;
	int i;

	/* if (side = 0) the we build the left half, (side = 1) for the right side. Used in initFromSplit(X,map-i,side) */
	int side;
	for (int i=0; i<size; i++){ /* Build X array */ /* use getMatrix instead of kdArr->mat */
		if (i< halfIndex){
			/* elements in Left */
			X[(getMatrixFromKDArray(kdArr))[coor][i]] = 0;

			/*X[ (kdArr->mat[coor][i]) ] = 0 ;*/
		}
		else{ /* elements in Right */
			X[(getMatrixFromKDArray(kdArr))[coor][i]] = 1 ;
		}
	}

	/*printing X*/
//	printf("X: ");
//	for(int k = 0; k < size; k++){
//		printf("%d, ",X[k]);
//	}
//	printf("\n");


	//for (int i=0; i<halfIndex; i++){ /* size of P1 is halfIndex */
	//P1[i] = kdArr->pointArray(kdArr->mat[coor][i]);

	/* Build P1, P2  - arrays including left and right, elements are points */

	for (int j=0; j< size; j++){
		/*	P1[i] = kdArr->pointArray[j];*/
//		printf("split: pointArray[%d] = %d\n", j, spPointGetIndex(P[j]));

	}


	i=0;
	int k = 0;
	for (int j = 0; j < size; j++){
		if (X[j] == 0){
		/*	P1[i] = kdArr->pointArray[j];*/
//			printf("pointArray[%d] = %d\n", j, spPointGetIndex((getPointArrayFromKDArray(kdArr))[j]));
		    P1[i] = P[j];
		//	P1[i] = getPointArrayFromKDArray(kdArr)[j];
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

	///////////////////////////////

	/*printing P1*/
	///printf("P1: ");
	//for(int k = 0; k < 3; k++){
	//	printf("%d, ",spPointGetIndex(P1[k]));
	//}
	//printf("\n");


	/*printing P2*/
	//printf("P2: ");
	//for(int k = 0; k < 2; k++){
	//	printf("%d, ",spPointGetIndex(P2[k]));
	//}
	//printf("\n");


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

	/*for (int i=0; i<size; i++){  map1
		flag = 0;  at the beginning of iteration - point not found yet
		for (int j=0; j < halfIndex; j++){  iterate in p1
			if (spPointGetIndex(kdArr->pointArray[i]) == spPointGetIndex(P1[j])){
				flag = 1;
				map1[i] = j;
			}
		}

		if (flag == 0){  not found in p-i
			map1[i] = -1;
		}

	}
//	printf("map1: ");
////	for(int m = 0; m < size; m++){
//		printf("%d, ",map1[m]);
//	}
//	printf("\n");


	for (int i=0; i<size; i++){  map2
		flag = 0;  at the beginning of iteration - point not found yet
		for (int j=0; j < (size-halfIndex); j++){  iterate in p2
			if (spPointGetIndex(kdArr->pointArray[i]) == spPointGetIndex(P2[j])){
				flag = 1;
				map2[i] = j;
			}
		}
		if (flag == 0){  not found in p-i
			map2[i] = -1;
		}

	}*/

//	printf("map2: ");
//	for(int m = 0; m < size; m++){
//		printf("%d, ",map2[m]);
//	}
//	printf("\n");

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
	int i=0;

	/* Calculate size of kdArray */
	int size;
	if (side ==0) /* Left side size */
		size = (arr->size + 1) / 2;
	else /* Right side size */
		size = arr->size - ((arr->size + 1) / 2);

	/*Copy the point-array we received in the init function*/
	SPPoint* copiedPointArr = (SPPoint*)malloc(sizeof(SPPoint)* arr->size); /* copy original point array */
	memcpy(copiedPointArr,arr->pointArray, sizeof(SPPoint)* arr->size);

	kdArray array = (kdArray)malloc(sizeof(struct SPKDArray)); /* output array */
	if(!array){ /* Allocation error */
		free(copiedPointArr);
		return NULL;
	}

	array->size = size;

	SPPoint* pointArr = (SPPoint*)malloc(sizeof(SPPoint)* size); /* allocate new point array */
	if(!pointArr){ /*Allocation error */
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
	if(mat == NULL){
		free(copiedPointArr);
		free(pointArr);
		free(array);
		return NULL;
	}

	for(int i = 0; i < array->dim; i++){
		mat[i] = (int*)malloc(sizeof(int)*size);
		if(mat[i]==NULL){
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

	for(int i = 0; i < arr->dim; i++){ //maybe arr->dim ?
		free(arr->mat[i]);
	}
	free(arr->mat);
	free(arr->pointArray);
	free(arr);
}



