/*
 * SPBPriorityQueue.c
 *
 *  Created on: May 20, 2016
 *      Author: adigrabow
 */
#include "SPBPriorityQueue.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

SPBPQueue spBPQueueCreate(int maxSize){
	if (maxSize <= 0)
		return NULL;

	SPBPQueue queue = (SPBPQueue) malloc(sizeof( struct sp_bp_queue_t));
	if (queue == NULL) /* allocation error */
		return NULL;

	queue->list = spListCreate();
	if (queue->list == NULL){/* allocation error */
		free(queue);
		return NULL;
	}

	queue->maxSize = maxSize;
	return queue;
}


SPBPQueue spBPQueueCopy(SPBPQueue source){

	if(source == NULL){
		return NULL;
	}

	SPBPQueue target = (SPBPQueue) malloc(sizeof( struct sp_bp_queue_t));
	if (target == NULL) /* allocation error */
		return NULL;

	target->maxSize = source->maxSize;
	target->list = spListCopy(source->list); /* allocation inside spListCopy */
	if (target->list == NULL){
		free(target);
		return NULL;
	}
	return target;


}


void spBPQueueDestroy(SPBPQueue source){

	if(source == NULL){
		return ;
	}

	spListDestroy(source->list);
	free(source);

}

void spBPQueueClear(SPBPQueue source){
	if(source == NULL){
		return ;
	}
	spListClear(source->list);

}


int spBPQueueSize(SPBPQueue source){
	if(source == NULL){
		return -1;
	}
	else{
		return spListGetSize(source->list);
	}
}

int spBPQueueGetMaxSize(SPBPQueue source){
	if(source == NULL){
		return -1;
	}
	else {return source->maxSize;}
}
/*we assume the element with the lowest value is the first one on the list*/
SPListElement spBPQueuePeek(SPBPQueue source){
	if(source == NULL){
		return NULL;
	}

	return spListElementCopy(spListGetFirst(source->list));
}
/*we assume the element with the highest value is the last on on the list*/

SPListElement spBPQueuePeekLast(SPBPQueue source){
	if(source == NULL){
		return NULL;
	}
	return spListElementCopy(spListGetLast(source->list));

}

double spBPQueueMinValue(SPBPQueue source){
	if((source == NULL) || (spBPQueueSize(source) == 0)){
		return -1.0;
	}
	double value = 0.0;
	SPListElement elem = spBPQueuePeek(source);
	value = spListElementGetValue(elem);

	spListElementDestroy(elem);
	return (value);

}

double spBPQueueMaxValue(SPBPQueue source){
	if((source == NULL) || (spBPQueueSize(source) == 0)){
		return -1.0;
	}
	double value = 0.0;

	SPListElement elem = spBPQueuePeekLast(source);
	value = spListElementGetValue(elem);
	spListElementDestroy(elem);
	return (value);

}

bool spBPQueueIsEmpty(SPBPQueue source){
	assert(source != NULL);
	if (spListGetSize(source->list) == 0)
		return true;
	return false;
}

bool spBPQueueIsFull(SPBPQueue source){
	assert(source != NULL);

	if (spBPQueueSize(source) == source->maxSize){
		return true;
	}
	return false;
}




SP_BPQUEUE_MSG spBPQueueEnqueue(SPBPQueue source, SPListElement element){
	SP_LIST_MSG msg = SP_BPQUEUE_INVALID_ARGUMENT;

	 /*check for invalid arguments*/
	if ( (source ==NULL) || (element == NULL) ){
		return SP_BPQUEUE_INVALID_ARGUMENT;
	}

	SPListElement current = spListGetFirst(source->list);  /*current is HEAD*/


	if (spBPQueueIsEmpty(source)){  /*if queue is empty - use function insert first*/
		msg = spListInsertFirst(source->list, element);
		if (msg == SP_LIST_SUCCESS){
			return SP_BPQUEUE_SUCCESS;
		}

		if (msg == SP_LIST_OUT_OF_MEMORY){
			return SP_BPQUEUE_OUT_OF_MEMORY;
		}

	}

	if (spBPQueueIsFull(source)){
		current = spListGetLast(source->list);
		if ((spListElementGetValue(current) < spListElementGetValue(element)) ||
				((spListElementGetValue(current)== spListElementGetValue(element))
						&& (spListElementGetIndex(current) < spListElementGetIndex(element))) ){
			return SP_BPQUEUE_FULL;
		}
		else {
			spListRemoveCurrent(source->list);
		}
	}
	current = spListGetFirst(source->list);

	while((current != NULL) ){
		/*if we want to insert the elem before the current node*/
		if (spListElementGetValue(current) > spListElementGetValue(element)){
			msg = spListInsertBeforeCurrent(source->list, element);
		}

	/*	if the values are equal we insert the one with the smaller index first*/
		if (spListElementGetValue(current) == spListElementGetValue(element)){
			if(spListElementGetIndex(current) > spListElementGetIndex(element)){
				msg = spListInsertBeforeCurrent(source->list, element);
		}
		}



		if (msg == SP_LIST_SUCCESS){
			return SP_BPQUEUE_SUCCESS;
		}
		if (msg == SP_LIST_OUT_OF_MEMORY){
		//	printf("SP_BPQUEUE_OUT_OF_MEMORY");
			return SP_BPQUEUE_OUT_OF_MEMORY;

		}

		/*forward the pointer*/
		current = spListGetNext(source->list);

	}
	if (spBPQueueIsEmpty(source)){  /*if queue is empty - use function insert first*/
		msg = spListInsertFirst(source->list, element);
		if (msg == SP_LIST_SUCCESS)
			return SP_BPQUEUE_SUCCESS;
		if (msg == SP_LIST_OUT_OF_MEMORY)
			return SP_BPQUEUE_OUT_OF_MEMORY;
	}
	current = spListGetLast(source->list);
	spListInsertAfterCurrent(source->list, element);
	return SP_BPQUEUE_SUCCESS;
}

SP_BPQUEUE_MSG spBPQueueDequeue(SPBPQueue source){
	/* check for invalid arguments */
	if (source ==NULL)
		return SP_BPQUEUE_INVALID_ARGUMENT;

	/* the list is empty */
	if(spListGetSize(source->list) == 0)
		return SP_BPQUEUE_EMPTY;


	spListGetFirst(source->list);
	spListRemoveCurrent(source->list);

	return SP_BPQUEUE_SUCCESS;


}


void printQueue(SPBPQueue source){ /* I added the header to queue.h */
	int size = spBPQueueSize(source);
	SPList list = source->list;
	SPListElement current = spListGetFirst(list);
	printf("Q: ");
	for (int i = 0; i < size; i++){
		printf("%d, ",spListElementGetIndex(current));
		current = spListGetNext(list);
	}

}
