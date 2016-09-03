/*
 * main_aux.h
 *
 *  Created on: 29 αιεμ 2016
 *      Author: Maayan Sivroni
 */

typedef struct Img {
	int index; /*this will store the index of the image this feature belongs to */
	int hits; /* this will store the number of hits this img had */

}Img;

#include "KDTree.h"
#include "SPConfig.h"
#include "SPBPriorityQueue.h"


#define _MAX 1024
#define CONFIG_DEFAULT ("spcbir.config")
#define ENTERED_CONFIG_FILE_NAME ("-c")
#define ENTER_QUERY ("Please enter an image path:\n")

/********************
 [R] Regular Messages
 ********************/

#define ERROR_DEFAULT_FILE_CONFIG ("The default configuration file 'spcbir.config' couldn't be open.\n")
#define ERROR_INVALID_CONFIG_ARGS ("Invalid command line : use -c <config_filename>\n")
#define ERROR_CANNOT_OPEN_CONFIG_FILE ("The configuration file %s couldn't be open\n")
#define REGULAR_MSG_SPCONFIG_INVALID_ARGUMENT ("The function 'spConfigCreate' received invalid argument.\n")
#define REGULAR_MSG_SPCONFIG_FAILED_TO_ALLOCATE_MEMORY ("The function 'spConfigCreate' failed to allocate memory.\n")
#define REGULAR_MSG_LOGGER_FILE_NAME_IS_NULL ("Logger file name was NULL.\n")
#define REGULAR_MSG_LOGGER_FILE_LEVEL_IS_NEGATIVE ("Logger level was negative.\n")
#define REGULAR_MSG_CANNOT_OPEN_LOGGER_FILE ("Failed to open logger file.\n")
#define REGULAR_MSG_CANNOT_OPEN_LOGGER_FILE ("Failed to open logger file.\n")
#define REGULAR_MSG_SPLOGGER_FAILED_TO_ALLOCATE_MEMORY ("The function 'spLoggerCreate' failed to allocate memory.\n")

/****************
 Logger Messages
 ****************/
#define LOGGER_ERROR_FUNCTION_ARGUMENTS_FAILED_TO_MEET_CONSTRAINTS ("Function arguments failed to meet constraints.\n")
#define LOGGER_ERROR_FAILED_TO_ALLOCATE_MEMORY ("Failed to allocate memory.\n")
#define LOGGER_ERROR_CANNOT_GET_NUM_OF_IMAGES ("Failed to extract numOfImages from SPConfig.\n")
#define LOGGER_ERROR_FAILED_TO_EXTRACT_SPLIT_METHOD_FROM_CONFIG ("Failed to extract splitMethod from SPConfig.\n")
#define LOGGER_ERROR_FAILED_TO_EXTRACT_NUM_OF_SIMILAR_IMGAGES_FROM_CONFIG ("Failed to extract numOfSimilarImages from SPConfig.\n")
#define LOGGER_ERROR_FAILED_TO_EXTRACT_IS_MINIMAL_GUI ("Failed to extract isMinimalGui from SPConfig.\n")
#define LOGGER_ERROR_FAILED_TO_EXTRACT_EXTRACTION_MODE ("Failed to extract extractionMode from SPConfig.\n")
#define LOGGER_ERROR_FAILED_TO_EXTRACT_IMAGE_PATH ("Failed to extract imagePath from SPConfig.\n")
#define LOGGER_ERROR_FAILED_TO_EXTRACT_FEATURE_PATH ("Failed to extract feature path from SPConfig.\n")
#define LOGGER_ERROR_FAILED_TO_INIT_KDARRAY ("Failed to use the KDArray 'init' function.\n")
#define EXIT_FROM_MAIN_MSG ("Exiting program...\n")

#define LOGGER_INFO_LOGGER_WAS_CREATED_SUCCESSFULLY ("Logger was created successfully.\n")
#define LOGGER_INFO_STARTED_PREPROCESSING ("Starting preprocessing.\n")
#define LOGGER_INFO_WORKING_IN_EXTRACTION_MODE ("Working in extraction mode.\n")
#define LOGGER_INFO_WORKING_IN_NON_EXTRACTION_MODE ("Working in non-extraction mode.\n")
#define LOGGER_INFO_FINISHED_PREPROCESSING ("Finished preprocessing.\n")
#define LOGGER_INFO_USING_MINIMAL_GUI ("Using minimal GUI.\n")
#define LOGGER_INFO_NOT_USING_MINIMAL_GUI ("Not using minimal GUI.\n")
#define LOGGER_INFO_DONE_WITH_EXTRACTION_MODE ("Done with extraction mode.\n")
#define LOGGER_INFO_WORKING_ON_NEW_QUERY ("Started main-loop for a new query.\n")

#define LOGGER_WAR_NUM_OF_UPDATED_HITS_SMALLER_THAN_NUM_OF_SIMILAR_IMG ("Number of updated hits is smaller than number of similar images.\n The num of images to be displayed will be smaller than expected.\n")





SPPoint *extractFromFiles(SPConfig config, int * size);
SPBPQueue initBPQ(SPConfig config);
void kNearestNeighbors(kdTree currNode, SPBPQueue bpq, SPPoint queryPoint);
int extractIndexFromQuery(char * query);
int * initCount(int numOfPics);
void destroyCount(int * array);
void addToCount(SPBPQueue bpq,int * allPicsCount);
Img * initImgArray(int * allPicsCount, int numOfPics);
int compareHits (const void * a, const void * b);
