#ifndef SPBPRIORITYQUEUE_H_
#define SPBPRIORITYQUEUE_H_
#include <stdbool.h>
#include "SPList.h"

/**
 * SP Bounded Priority Queue summary
 *
 * Implements a queue container type.
 * The elements of the queue are type SPListElement and stored in SPList,
 * please refer to SPListElement.h and SPList.h for usage.
 * The elements of the queue are ordered by value
 * (and then by index, in case values are identical).
 * The queue can contain maxSize elements at most, as stated in spBPQueueCreate.
 *
 * The following functions are available:
 *
 *   spBPQueueCreate            - Creates an empty queue with a given maximum capacity
 *   spBPQueueCopy              - Creates a copy of a given queue
 *   spBPQueueDestroy           - Frees all memory allocation associated with the queue
 *   spBPQueueClear             -  Removes all the elements in the queue
 *   spBPQueueSize              - Returns the number of elements in the queue
 *   spBPQueueGetMaxSize        - Returns the maximum capacity of the queue
 *   spBPQueueEnqueue           - Inserts a NEW COPY (must be allocated)
 *   							  element to the queue
 *   spBPQueueDequeue           - Removes the element with the lowest value
 *   spBPQueuePeek  			- Returns a NEW COPY of the element with the lowest value
 *   spBPQueuePeekLast   		- Returns a NEW COPY of the element with the highest value
 *   spBPQueueMaxValue        	- Returns the maximum value in the queue
 *   spBPQueueMinValue			- Returns the minimum value in the queue
 *   spBPQueueIsEmpty           - Returns true if the queue is empty
 *   spBPQueueIsFull            - Returns true if the queue is full
**/
/** type used to define Bounded priority queue **/

typedef struct sp_bp_queue_t* SPBPQueue;


struct sp_bp_queue_t {

	int maxSize;
	SPList list;

};


/** type for error reporting **/
typedef enum sp_bp_queue_msg_t {
	SP_BPQUEUE_OUT_OF_MEMORY,
	SP_BPQUEUE_FULL,
	SP_BPQUEUE_EMPTY,
	SP_BPQUEUE_INVALID_ARGUMENT,
	SP_BPQUEUE_SUCCESS
} SP_BPQUEUE_MSG;


/**
 * Allocates a new queue.
 *
 * This function creates a new empty queue with maxSize capacity.
 * @param maxSize The maximum number of elements in the queue (capacity)
 * @return
 * 	NULL - If maxSize is negative, allocations failed (inner SPList or queue).
 * 	A new Queue in case of success.
 */

SPBPQueue spBPQueueCreate(int maxSize);

/**
 * Creates a copy of the target queue.
 *
 * The new copy will contain all the elements from the source queue in the same
 * order. The function is using spListCopy in order to allocate new list.
 *
 * @param SPBPQueue source - The target queue to copy
 * @return
 * NULL - if a source == NULL or a memory allocation failed.
 * A Queue containing the same elements with same order as queue otherwise.
 */

SPBPQueue spBPQueueCopy(SPBPQueue source);

/**
 * Deallocates an existing queue. Clears all elements by using the spListDestroy,
 * and then by using the free function on the queue.
 *
 * @param source Target queue to be deallocated.
 * If queue is NULL nothing will be done.
 */

void spBPQueueDestroy(SPBPQueue source);

/**
 * Removes all elements from target queue.
 * The elements are deallocated using spListClear.
 * maxSize field will not change.
 *
 * @param source Target queue to remove all element from.
 * If queue is NULL nothing will be done, the queue is cleared otherwise.
 */

void spBPQueueClear(SPBPQueue source);

/**
 * Returns the number of elements in a queue by using spBPQueueSize.
 *
 * @param source The target queue which size is requested.
 * @return
 * -1 if a NULL pointer was sent.
 * Otherwise the number of elements in the list.
 */

int spBPQueueSize(SPBPQueue source);

/**
 * Returns the maximal capacity of the queue.
 *
 * @param source - the target queue for which maxSize is requested.
 * @return
 * -1 if a NULL pointer was sent.
 * Otherwise - the capacity of the queue.
 */

int spBPQueueGetMaxSize(SPBPQueue source);

/**
 * Inserts a SPListElement into a SPBPQueue. If the queue is full the func makes sure
 * the new element's value is smaller than the max value (if not - element will
 * not be inserted)
 * @param source - the queue to which element will be inserted to.
 * @param elemnt - the SPListElement that the user wants to insert to the queue.
 * @return
 * SP_BPQUEUE_INVALID_ARGUMENT if (source ==NULL) || (element == NULL)
 * SP_BPQUEUE_OUT_OF_MEMORY if memory problem occurs
 * SP_BPQUEUE_FULL if the queue is full and the val of element is larger than max's val
 * SP_BPQUEUE_SUCCESS if element is now in queue
 */
SP_BPQUEUE_MSG spBPQueueEnqueue(SPBPQueue source, SPListElement element);

/**
 * Removes the first element of the queue - the element containing the lowest value.
 * First, the current pointer is set the the first element in queue
 * by using spListGetFirst.
 * Second, the current element will be removed by spListRemoveCurrent.
 *
 * @param SPBPQueue source - The queue for which the first element will be removed
 * @return
 * SP_BPQUEUE_INVALID_ARGUMENT if queue is NULL
 * SP_BPQUEUE_EMPTY if queue is empty
 * SP_BPQUEUE_SUCCESS the first element was removed successfully
 */

SP_BPQUEUE_MSG spBPQueueDequeue(SPBPQueue source);

/**
 *returns a new copy of the element with the lowest value from the queue
 *@param SPBPQueue source - the queue for which the element with the lowest value
 *will be returned.
 * @return
 * a copy of the requested SPListElement
 *
 */
SPListElement spBPQueuePeek(SPBPQueue source);

/**
 *  *returns a new copy of the element with the highest value from the queue
 *@param SPBPQueue source - the queue for which the element with the highest value
 *will be returned.
 * @return
 * a copy of the requested SPListElement
 */
SPListElement spBPQueuePeekLast(SPBPQueue source);

/**
 * Returns the minimum value of all elements in a queue.
 * Gets the value of the first element by using spBPQueuePeek.
 *
 * @param source The target queue which the minimum value is requested.
 * @return
 * -1 if a NULL pointer was sent or queue is empty.
 * Otherwise the minimum value of the queue.
 */

double spBPQueueMinValue(SPBPQueue source);

/**
 * Returns the maximum value of all elements in a queue.
 * Gets the value of the last element by using spBPQueuePeekLast.
 *
 * @param source The target queue which the maximum value is requested.
 * @return
 * -1 if a NULL pointer was sent or queue is empty.
 * Otherwise the maximum value of the queue.
 */

double spBPQueueMaxValue(SPBPQueue source);

/**
 * Returns true is the queue is empty by number of elements currently in queue to zero.
 *
 * Asserts:
 * 		source is not NULL
 * @param source The target queue to be checked.
 * @return
 * true if the comparison expression is true, false otherwise.
 */

bool spBPQueueIsEmpty(SPBPQueue source);

/**
 * Returns true is the queue is full by comparing number of elements currently in queue
 * and maxSize (maximum capacity).
 *
 * Asserts:
 * 		source is not NULL
 * @param source The target  queue to be checked.
 * @return
 * true if the comparison expression is true, false otherwise.
 */

bool spBPQueueIsFull(SPBPQueue source);

/**
 * prints the index of each element in the queue by order
 * @param SPBPQueue source - the queue to be printed
 *
 */
void printQueue(SPBPQueue source);

#endif
