/*
 * kdArray_unit_test.c
 *
 *  Created on: Jul 29, 2016
 *      Author: adigrabow
 */

#include "SPPoint.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "KDArray.h"

 /*new test file*/

void MemCpyTest();
void printMatrix(int** mat, int numOfPoints, int dim);
void printKDArray(kdArray arr, int size);
void simpleInitTest();
void splitTest1();
void splitTest2();

/*
int main(){

	//simpleInitTest();
	//splitTest1();
	splitTest2();

	printf("finished main!\n");
	return 0;
}*/

void simpleInitTest(){

	double data1[2] = {1.0,2.0};
	double data2[2]= {123.0,70.0};
	double data3[2]= {2.0,7.0};
	double data4[2]= {9.0,11.0};
	double data5[2]= {3.0,4.0};


	SPPoint p1 = spPointCreate(data1, 2, 0);
	SPPoint p2 = spPointCreate(data2, 2, 1);
	SPPoint p3 = spPointCreate(data3, 2, 2);
	SPPoint p4 = spPointCreate(data4, 2, 3);
	SPPoint p5 = spPointCreate(data5, 2, 4);


	SPPoint SPPointArr[5] = {p1,p2,p3,p4,p5};
	kdArray resultArray = Init(SPPointArr , 5);

	printMatrix(getMatrixFromKDArray(resultArray), 5, 2);

	spPointDestroy(p1);
	spPointDestroy(p2);
	spPointDestroy(p3);
	spPointDestroy(p4);
	spPointDestroy(p5);

	destroyKdArray(resultArray);


	return;
}


void MemCpyTest(){

	double data1[4] = {1.1,2.2,3.3,4.4};
	double data2[4]= {5.5,6.6,7.7,8.8};
	double data3[4]= {9.9,10.10,11.11,12.12};

	SPPoint p1 = spPointCreate(data1, 4, 1);
	SPPoint p2 = spPointCreate(data2, 4, 2);
	SPPoint p3 = spPointCreate(data3, 4, 3);

	SPPoint SPPointArr[3] = {p1,p1,p3};

	for(int i = 0; i < 3; i++){
		printf("p%d[0] = %f\n",i,spPointGetAxisCoor(SPPointArr[i], 0));
	}

	SPPoint* copiedArr = (SPPoint*)malloc(sizeof(SPPointArr));
	memcpy(copiedArr,SPPointArr, sizeof(SPPointArr));

	for(int i = 0; i < 3; i++){
		printf("p%d[0] = %f\n",i,spPointGetAxisCoor(copiedArr[i], 0));
	}


	spPointDestroy(p1);
	spPointDestroy(p2);
	spPointDestroy(p3);
	free(copiedArr);
	free(SPPointArr);

	return;
}


void printMatrix(int** mat, int numOfPoints, int dim){
	for(int i = 0; i < dim ; i++){
		printf("row %d: ",i);
		for(int j = 0 ; j < numOfPoints; j++){
			printf("%d,",mat[i][j]);
		}
		printf("\n");
	}
}
/*this is the example we were given*/
void splitTest1(){
	double data1[2] = {1.0,2.0};
	double data2[2]= {123.0,70.0};
	double data3[2]= {2.0,7.0};
	double data4[2]= {9.0,11.0};
	double data5[2]= {3.0,4.0};


	SPPoint p1 = spPointCreate(data1, 2, 0);
	SPPoint p2 = spPointCreate(data2, 2, 1);
	SPPoint p3 = spPointCreate(data3, 2, 2);
	SPPoint p4 = spPointCreate(data4, 2, 3);
	SPPoint p5 = spPointCreate(data5, 2, 4);


	SPPoint SPPointArr[5] = {p1,p2,p3,p4,p5};
	kdArray resultArray = Init(SPPointArr , 5);

	kdArray * TwoKDArrays = Split(resultArray, 0);

	printMatrix(getMatrixFromKDArray(TwoKDArrays[0]), 3, 2);
	printMatrix(getMatrixFromKDArray(TwoKDArrays[1]), 2, 2);


	spPointDestroy(p1);
	spPointDestroy(p2);
	spPointDestroy(p3);
	spPointDestroy(p4);
	spPointDestroy(p5);

	destroyKdArray(TwoKDArrays[0]);
	destroyKdArray(TwoKDArrays[1]);

	return;
}


void splitTest2(){
	double data1[3] = {1.0,5.0,10.0};
	double data2[3]= {2.0,4.0,20.0};
	double data3[3]= {3.0,3.0,30.0};
	double data4[3]= {4.0,2.0,40.0};
	double data5[3]= {5.0,1.0,50.0};
	double data6[3]= {6.0,0.5,60.0};



	SPPoint p1 = spPointCreate(data1, 3, 0);
	SPPoint p2 = spPointCreate(data2, 3, 1);
	SPPoint p3 = spPointCreate(data3, 3, 2);
	SPPoint p4 = spPointCreate(data4, 3, 3);
	SPPoint p5 = spPointCreate(data5, 3, 4);
	SPPoint p6 = spPointCreate(data6, 3, 5);


	SPPoint SPPointArr[6] = {p1,p2,p3,p4,p5,p6};
	kdArray resultArray = Init(SPPointArr , 6);

	kdArray * TwoKDArrays = Split(resultArray, 0);
	printf("left kdArray matrix: \n");
	printMatrix(getMatrixFromKDArray(TwoKDArrays[0]), 3, 3);
	printf("right kdArray matrix: \n");
	printMatrix(getMatrixFromKDArray(TwoKDArrays[1]), 3, 3);
	printPointIndex(TwoKDArrays[0],3);
	printPointIndex(TwoKDArrays[1],3);

	spPointDestroy(p1);
	spPointDestroy(p2);
	spPointDestroy(p3);
	spPointDestroy(p4);
	spPointDestroy(p5);
	spPointDestroy(p6);

	destroyKdArray(TwoKDArrays[0]);
	destroyKdArray(TwoKDArrays[1]);

	return;

}
void printKDArray(kdArray arr, int size){
	printMatrix(getMatrixFromKDArray(arr), size, getDimFromKDArray(arr));
	return;
}



