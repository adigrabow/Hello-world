/*
 * KDTree.c
 *
 *  Created on: 27  2016
 *      Author: Maayan Sivroni
 */

#include "KDArray.h"
#include "KDTree.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <string.h>
#include <time.h>

/* Note: the name we need to define is KDTreeNode. Maybe the typedef is wrong?
 * spKDTreeSplitMethod should be part of the config struct.
 * Create a destroy function for the tree.
 * */

/******************
 Logger Messages
 *****************/
#define LOGGER_ERROR_FAILED_TO_ALLOCATE_MEM_FOR_SORTED_POINT_ARR ("Failed to allocate memory for SortedPointArr.\n")
#define LOGGER_ERROR_FUNCTION_ARGUMENTS_FAILED_TO_MEET_CONSTRAINTS ("Function arguments failed to meet constraints.\n")
#define LOGGER_ERROR_FAILED_TO_ALLOCATE_MEMORY ("Failed to allocate memory.\n")
#define LOGGER_ERROR_FUNCTION_ARGUMENT_IS_EMPTY ("Function received empty parameter.\n")



struct KDTreeNode{
	int dim; /* The splitting dimension */
	double val; /* The median value of the splitting dimension */
	struct KDTreeNode* left; /* Pointer to the left subtree */
	struct KDTreeNode* right; /* Pointer to the right subtree */
	SPPoint data; /* Pointer to a point (only if the current node is a leaf) otherwise this field value is NULL */
};


/*************************
 Functions Implementation
 ************************/

kdTree init(kdArray kdArr, int * prevIndex, SP_KDTREE_SPLIT_METHOD_TYPE splitMethod){ /* prevIndex = pointer to the i-split dimension */

	/* if array size is 1 then create node and return it */
	if (getSizeFromKDArray(kdArr) == 1){
		kdTree node = (kdTree)malloc(sizeof(struct KDTreeNode));

		if(node == NULL) {
			spLoggerPrintError(LOGGER_ERROR_FAILED_TO_ALLOCATE_MEMORY,__FILE__, __func__, __LINE__ );
			return NULL;
		}

		node->dim = -1;
		node->val = -1.0;
		node->left = NULL;
		node->right = NULL;
		SPPoint point = spPointCopy((getPointArrayFromKDArray(kdArr))[0]);
		node->data = point;
		return node;
	}
	double * spreadArray = (double *) malloc (sizeof(double)*getDimFromKDArray(kdArr)); /* will contain all spreads for MAX_SPREAD mode */

	if (NULL == spreadArray) {
		spLoggerPrintError(LOGGER_ERROR_FAILED_TO_ALLOCATE_MEMORY,__FILE__, __func__, __LINE__ );
		return NULL;
	}

	int i;
	double max= 0.0; /* for MAX_SPREAD */
	int index; /* the index of the dimension to split by */
	kdArray * twoKdArrays;
	double * tmpDataArray; /* In order to free the copied array we need to save it in a variable - for node->val */

	/* if array size is bigger than 1 then create tree recursively */
	/* I assume that the following parameter from config - Use a getter after it is build */

	if (splitMethod == MAX_SPREAD){

		/* create spreadArray - calculate spread for each dimension and choose the maximum */
		for (i=0; i< getDimFromKDArray(kdArr); i++){

			double max_coor, min_coor;
			double * tmpDataArray1;
			double * tmpDataArray2;
			tmpDataArray1 = spPointGetData(getPointArrayFromKDArray(kdArr)[ getMatrixFromKDArray(kdArr)[i][getSizeFromKDArray(kdArr) -1] ]);
			max_coor = tmpDataArray1[i];
			free(tmpDataArray1);
			tmpDataArray2 = spPointGetData(getPointArrayFromKDArray(kdArr)[ getMatrixFromKDArray(kdArr)[i][0] ]);
			min_coor = tmpDataArray2[i];
			free(tmpDataArray2);
			spreadArray[i] = max_coor-min_coor;

			if (spreadArray[i] >= max){  /*find max spread from array*/
				max = spreadArray[i];

			}
		}
		/*	 find the index of max - the first index to be found*/
		for (i=0; i< getDimFromKDArray(kdArr); i++){
			if (spreadArray[i] == max){
				index = i;
				break;
			}
		}

	}

	if (splitMethod == RANDOM){

		index =(int)  0 + rand() / (RAND_MAX / (getDimFromKDArray(kdArr)-1 - 0 + 1) + 1);

	}

	if (splitMethod == INCREMENTAL){ /* INCREMENTAL */

		index = (*prevIndex +1) % getDimFromKDArray(kdArr);
	}

	twoKdArrays = Split(kdArr, index);

	// create tree recursively
	kdTree node = (kdTree)malloc(sizeof(struct KDTreeNode));
	if(node == NULL) {
		spLoggerPrintError(LOGGER_ERROR_FAILED_TO_ALLOCATE_MEMORY,__FILE__, __func__, __LINE__ );
		free(spreadArray);
		return NULL;
	}

	node->dim = index;
	SPPoint point = (getPointArrayFromKDArray(twoKdArrays[0])) [ getMatrixFromKDArray(twoKdArrays[0])[index][getSizeFromKDArray(twoKdArrays[0]) -1] ];
	tmpDataArray = spPointGetData(point);
	node->val = tmpDataArray[index];
	free(tmpDataArray);
	// last point in left array
	kdArray leftArr = twoKdArrays[0];
	kdArray rightArr = twoKdArrays[1];
	kdTree left = init(leftArr, &index, splitMethod);
	kdTree right = init(rightArr, &index,  splitMethod);
	node->left = left;
	node->right = right;
	node->data = NULL; // pointArray is an array that is a pointer to point

	return node;

}

/********
 Getters
 ********/

int kdTreeGetDimension(kdTree node){
	if ( NULL == node) {
		spLoggerPrintError(LOGGER_ERROR_FUNCTION_ARGUMENT_IS_EMPTY,__FILE__, __func__, __LINE__ );
		return -1;
	}
	return node->dim;
}

double kdTreeGetVal(kdTree node){

	if (NULL == node) {
		spLoggerPrintError(LOGGER_ERROR_FUNCTION_ARGUMENT_IS_EMPTY,__FILE__, __func__, __LINE__ );
		return -1.0;
	}
	return node->val;
}

kdTree  kdTreeGetLeft(kdTree node){

	if (NULL == node){
		spLoggerPrintError(LOGGER_ERROR_FUNCTION_ARGUMENT_IS_EMPTY,__FILE__, __func__, __LINE__ );
		return NULL;
	}
	return node->left;
}

kdTree  kdTreeGetRight(kdTree node){

	if (NULL == node) {
		spLoggerPrintError(LOGGER_ERROR_FUNCTION_ARGUMENT_IS_EMPTY,__FILE__, __func__, __LINE__ );
		return NULL;
	}
	return node->right;
}

SPPoint kdTreeGetData(kdTree node){

	if (NULL == node){
		spLoggerPrintError(LOGGER_ERROR_FUNCTION_ARGUMENT_IS_EMPTY,__FILE__, __func__, __LINE__ );
		return NULL;
	}
	if (NULL == node->data) {
		spLoggerPrintError(LOGGER_ERROR_FUNCTION_ARGUMENTS_FAILED_TO_MEET_CONSTRAINTS,__FILE__, __func__, __LINE__ );
		return NULL;
	}
	SPPoint dataCopy = spPointCopy(node->data);
	return dataCopy;
}

void destroyKdTree(kdTree node){

	if (NULL == node) {
		spLoggerPrintError(LOGGER_ERROR_FUNCTION_ARGUMENT_IS_EMPTY,__FILE__, __func__, __LINE__ );
		return;
	}

	if( kdTreeGetDimension(node) != -1 ){ // if the node is NOT a leaf
		destroyKdTree(node->left);
		destroyKdTree(node->right);
		free( node );
	}
	else{
		spPointDestroy(node->data);
		free(node);
	}
}


void printPointIndex(kdArray arr, int numOfPoints){
	if (NULL == arr){
		printf("array is null");
		return;
	}
	printf("indexes of SPPoint array: ");
	for(int i = 0; i < numOfPoints; i++){
		printf("%d, ",spPointGetIndex((getPointArrayFromKDArray(arr))[i]));
	}
	printf("\n");

}
