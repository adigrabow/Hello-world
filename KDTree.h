/*
 * KDTree.h
 *
 *  Created on: 27 αιεμ 2016
 *      Author: Maayan Sivroni
 */
#include "SPConfig.h"
#include "KDArray.h"
#include "SPPoint.h"

typedef struct KDTreeNode* kdTree;
kdTree init(kdArray kdArr, int * index, SP_KDTREE_SPLIT_METHOD_TYPE splitMethod);


int kdTreeGetDimension(kdTree node);
double kdTreeGetVal(kdTree node);
kdTree  kdTreeGetLeft(kdTree node);

kdTree  kdTreeGetRight(kdTree node);

SPPoint kdTreeGetData(kdTree node);
void destroyKdTree(kdTree node);
void printPointIndex(kdArray arr, int numOfPoints);
