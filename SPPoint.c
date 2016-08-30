/*
 * SPPoint.c
 *
 *  Created on: May 13, 2016
 *      Author: adigrabow
 */

#include "SPPoint.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <string.h>


struct sp_point_t{
	double * data;
	int dim;
	int index;
};

SPPoint spPointCreate(double* data, int dim, int index){
	if(data == NULL || dim <= 0 || index < 0){
		return NULL;
	}
	SPPoint point = (SPPoint) malloc(sizeof(struct sp_point_t)); /*allocate space for the new point*/
	if(point == NULL)
		return NULL;

	double* dataCopy = (double*) malloc(sizeof(double) * dim);
	memcpy(dataCopy, data, sizeof(double) * dim); /*copies info from 'data' to 'dataCopy'*/
	point->data = dataCopy;
	point->dim = dim;
	point->index = index;
	return point;

}

SPPoint spPointCopy(SPPoint source){
	if (NULL == source) {
		return NULL;
	}
	assert(source != NULL);
	SPPoint target = malloc(sizeof(struct sp_point_t));
	if(target == NULL)
		return NULL;

	target = spPointCreate(source->data, source->dim, source->index);
	return target;

}


void spPointDestroy(SPPoint point){
	if (NULL == point) {
		return;
	}

	free(point->data);
	free(point);
}

int spPointGetDimension(SPPoint point){
	if (NULL == point) {
		return -1;
	}
	return point->dim;
}

int spPointGetIndex(SPPoint point){
	if (NULL == point){
		return -1;
	}
	return point->index;
}

double spPointGetAxisCoor(SPPoint point, int axis){
	if(point == NULL){
		return -1;
	}
	if(axis >= point->dim){
		return -1;
	}

	return point->data[axis];
}


double * spPointGetData(SPPoint point){
	if (NULL == point) {
		return NULL;
	}
	double* dataCopy = (double*) malloc(sizeof(double) * spPointGetDimension(point));
	memcpy(dataCopy, point->data, sizeof(double) *  spPointGetDimension(point));
	return dataCopy;
}

double spPointL2SquaredDistance(SPPoint p, SPPoint q){
	if (NULL == p || NULL == q || spPointGetDimension(p) != spPointGetDimension(q)) {
		return -1.0;
	}
	assert(p != NULL && q != NULL && p->dim == q->dim);
	double sum = 0;
	int i;
	for(i = 0; i < p->dim; i++){
		sum += pow((p->data[i] - q->data[i]),2);
	}
	return sum;

}
void printPoint(SPPoint p){
	if (!p){
			return;
		}
	int dim = p->dim;
	for(int i = 0; i < dim; i++){
		printf("%f, ",spPointGetAxisCoor(p,i));
	}
}

