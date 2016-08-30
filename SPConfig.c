/*
 * SPConfig.c
 *
 *  Created on: Jul 30, 2016
 *      Author: adigrabow
 */

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "SPConfig.h"

#ifndef NULL
#define NULL   ((void *) 0)
#endif
#define MAX_STRING_LEN 1024

struct sp_config_t{

	char spImagesDirectory[MAX_STRING_LEN];
	char spImagesPrefix[MAX_STRING_LEN];
	char spImagesSuffix[MAX_STRING_LEN];
	int spNumOfImages;
	int spPCADimension;
	char spPCAFilename[MAX_STRING_LEN];
	int spNumOfFeatures;
	bool spExtractionMode;
	int spNumOfSimilarImages;
	SP_KDTREE_SPLIT_METHOD_TYPE spKDTreeSplitMethod;
	int spKNN;
	bool spMinimalGUI;
	int spLoggerLevel;
//	SP_LOGGER_LEVEL spLoggerLevel;
	char spLoggerFilename[MAX_STRING_LEN];

};


/********************
 * characters MACROS
 ********************/

#define HASH ('#')
#define TAB ('\t')
#define VERTICAL ('\v')
#define NEW_LINE ('\r')
#define WHITESPACE (' ')
#define EQUALS ("=")
#define DECIMAL_POINT ('.')
#define PLUS_SIGN ('+')
#define MINUS_SIGN ('-')

/*********************
 * image suffix values
 *********************/
#define JPG (".jpg")
#define PNG (".png")
#define BMP (".bmp")
#define GIF (".gif")
#define FEAT (".feats")

/**************************
 * Internal status messages
 **************************/

#define INTERNAL_STATUS_SUCCESS ("success")
#define INTERNAL_STATUS_INVALID_CONFIG_LINE ("invalid config line")
#define INTERNAL_STATUS_CONSTRAIT_NOT_MET ("constrait not met")
#define INTERNAL_DEFAULT_STRING_VALUE ("Variable not set yet")

/***********************
 * error messages MACROS
 ***********************/

#define INVALID_CONFIG_LINE_ERROR_MSG ("File: %s\nLine: %d\nMessage: Invalid configuration line\n")
#define CONSTRAIT_NOT_MET_ERROR_MSG ("File: %s\nLine: %d\nMessage: Invalid value - constraint not met\n")
#define PARAMATER_IS_NOT_SET_ERROR_MSG ("File: %s\nLine: %d\nMessage: Parameter %s is not set\n")


/**************************
 * Function Implementation
 * ************************/
/*
int main(){
	SP_CONFIG_MSG msg = SP_CONFIG_SUCCESS;
	SPConfig conf = spConfigCreate("a.txt", &msg);
	printVariableValuesOfConfig(conf);
	return 0;
}*/


SPConfig spConfigCreate(const char* filename, SP_CONFIG_MSG* msg){

	int configLineCounter = 0;
	char line[MAX_STRING_LEN];
	int n = MAX_STRING_LEN;
	int valueFlag = 0; /* flag == 1 if word is a value, and 0 if its a variable name */

	if(filename == NULL){
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		return NULL;
	}


	SPConfig config = (SPConfig)malloc(sizeof(struct sp_config_t));//TODO free memory!
	if(config == NULL){
		*msg = SP_CONFIG_ALLOC_FAIL;
		return NULL;
	}

	FILE* configFile = fopen(filename, "r");
	if(configFile == NULL){
		*msg = SP_CONFIG_CANNOT_OPEN_FILE;
		return NULL;
	}
	assignDefaultValues(config);

	while(fgets(line,n,configFile) != NULL){

		char* string;
		const char * delimiter = EQUALS;
		char value[MAX_STRING_LEN];
		char variableName[MAX_STRING_LEN];
		char statusMSG[MAX_STRING_LEN] = INTERNAL_STATUS_SUCCESS;
		configLineCounter++;
		valueFlag = 0;

		if(isLineCommentLine(line))
			continue;
		string = strtok(line, delimiter);

		while(string != NULL ){

			if(valueFlag == 0){
				memset(variableName, 0, sizeof(variableName));
				getCleanWordFromString(string, variableName);
			}

			if(valueFlag == 1){
				memset(value, 0, sizeof(value));
				getCleanWordFromString(string, value);
				assignValueToVariable(config, variableName, value, statusMSG, msg);

				if((strncmp(statusMSG, INTERNAL_STATUS_INVALID_CONFIG_LINE, 19) == 0) ||
						(strncmp(statusMSG, INTERNAL_STATUS_CONSTRAIT_NOT_MET, 17) == 0)){
					free(config);

					fclose(configFile);
					return NULL;
				}
			}
			string = strtok(NULL, delimiter);
			valueFlag = 1;
		}
	}

	if(strncmp(config->spImagesDirectory, INTERNAL_DEFAULT_STRING_VALUE,20) == 0){

		printf(PARAMATER_IS_NOT_SET_ERROR_MSG, __FILE__, configLineCounter, "spImagesDirectory");
		*msg = SP_CONFIG_MISSING_DIR;
		free(config);
		fclose(configFile);
		return NULL;

	}
	if(strncmp(config->spImagesPrefix, INTERNAL_DEFAULT_STRING_VALUE,20) == 0){
		printf(PARAMATER_IS_NOT_SET_ERROR_MSG, __FILE__, configLineCounter, "spImagesPrefix");
		*msg = SP_CONFIG_MISSING_PREFIX;
		free(config);
		fclose(configFile);
		return NULL;
	}
	if(strncmp(config->spImagesSuffix, INTERNAL_DEFAULT_STRING_VALUE,20) == 0){
		printf(PARAMATER_IS_NOT_SET_ERROR_MSG, __FILE__, configLineCounter, "spImagesSuffix");
		*msg = SP_CONFIG_MISSING_PREFIX;
		free(config);
		fclose(configFile);
		return NULL;
	}
	if((config->spNumOfImages == -1)){
		printf(PARAMATER_IS_NOT_SET_ERROR_MSG, __FILE__, configLineCounter, "spNumOfImages" );
		*msg = SP_CONFIG_MISSING_NUM_IMAGES;
		free(config);
		fclose(configFile);
		return NULL;
	}

	fclose(configFile);
	return config;


}


bool spConfigIsExtractionMode(const SPConfig config, SP_CONFIG_MSG* msg){
	assert(msg != NULL);
	if(config == NULL){
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		return false;
	}

	*msg = SP_CONFIG_SUCCESS;
	return config->spExtractionMode;
}

bool spConfigMinimalGui(const SPConfig config, SP_CONFIG_MSG* msg){
	assert(msg != NULL);
	if(config == NULL){
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		return false;
	}
	*msg = SP_CONFIG_SUCCESS;
	return config->spMinimalGUI;

}

int spConfigGetNumOfImages(const SPConfig config, SP_CONFIG_MSG* msg){
	assert(msg != NULL);
	if(config == NULL){
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		return -1;
	}

	*(msg) = SP_CONFIG_SUCCESS;
	return config->spNumOfImages;

}



int spConfigGetNumOfFeatures(const SPConfig config, SP_CONFIG_MSG* msg){
	assert(msg != NULL);
	if(config == NULL){
		*(msg) = SP_CONFIG_INVALID_ARGUMENT;
		return -1;
	}

	*msg = SP_CONFIG_SUCCESS;
	return config->spNumOfFeatures;
}

int spConfigGetPCADim(const SPConfig config, SP_CONFIG_MSG* msg){
	assert(msg != NULL);
	if(config == NULL){
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		return -1;
	}

	*msg = SP_CONFIG_SUCCESS;
	return config->spPCADimension;
}

SP_CONFIG_MSG spConfigGetImagePath(char* imagePath, const SPConfig config,
		int index){
	if(imagePath == NULL || config == NULL){
		return SP_CONFIG_INVALID_ARGUMENT;
	}
	if(index >= config->spNumOfImages){
		return SP_CONFIG_INDEX_OUT_OF_RANGE;
	}
	memset(imagePath, 0, MAX_STRING_LEN);
	char indexAsString[1024] = {0};
	sprintf(indexAsString, "%d",index);
	strcat(imagePath,config->spImagesDirectory);
	strcat(imagePath,config->spImagesPrefix);
	strcat(imagePath,indexAsString);
	strcat(imagePath,config->spImagesSuffix);

	return SP_CONFIG_SUCCESS;

}

SP_CONFIG_MSG spConfigGetPCAPath(char* pcaPath, const SPConfig config){
	if(pcaPath == NULL || config == NULL){
		return SP_CONFIG_INVALID_ARGUMENT;
	}

	strcat(pcaPath,config->spImagesDirectory);
	strcat(pcaPath,config->spPCAFilename);
	return SP_CONFIG_SUCCESS;

}

void spConfigDestroy(SPConfig config){
	if(config != NULL){
		free(config);
	}
	return;
}

/******************
 * Getters (that we were not asked to implement)
 * ****************/


char* spConfigGetspImageDirectory(SPConfig config){
	if(config == NULL){
		return NULL;
	}
	return config->spImagesDirectory;

}

char* spConfigGetspImagesPrefix(SPConfig config){
	if(config == NULL){
		return NULL;
	}
	return config->spImagesPrefix;

}

char* spConfigGetspImagesSuffix(SPConfig config){
	if(config == NULL){
		return NULL;
	}
	return config->spImagesSuffix;
}


char* spConfigGetspPCAFilename(SPConfig config){
	if(config == NULL){
		return NULL;
	}
	return config->spPCAFilename;
}

char* spConfigGetspLoggerFilename(SPConfig config){
	if(config == NULL){
		return NULL;
	}
	return config->spLoggerFilename;
}

int spConfigGetspNumOfSimilarImages(SPConfig config){
	if(config == NULL){
		return -1;
	}
	return config->spNumOfSimilarImages;
}

int spConfigGetspKNN(SPConfig config){
	if(config == NULL){
		return -1;
	}
	return config->spKNN;
}
SP_LOGGER_LEVEL spConfigGetspLoggerLevel(SPConfig config){
	if(config == NULL){
		return -1;
	}
	return (SP_LOGGER_LEVEL) (config->spLoggerLevel - 1);
}

SP_KDTREE_SPLIT_METHOD_TYPE spConfigGetspKDTreeSplitMethod(SPConfig config){
	if(config == NULL){
		return INVALID_CONFIG;
	}

	return config->spKDTreeSplitMethod;
}


/******************
 * Help Functions (need to add description in SPConfig.h)
 * ****************/

void assignValueToVariable(SPConfig config, char* variableName,
		char* value, char* statusMSG,  SP_CONFIG_MSG* msg){


	if(strncmp(variableName, "spImagesDirectory", 17) == 0){
		strcpy(config->spImagesDirectory, value);

		/* DF: how much memory do you want to set?  sizeof(statusMSG)?
		 * 		hint - statusMSG is a pointer.						  */
		//TODO changed to sizeof char
		memset(statusMSG, 0, MAX_STRING_LEN);
		strcpy(statusMSG, INTERNAL_STATUS_SUCCESS);
		*msg = SP_CONFIG_SUCCESS;
		return;
	}

	if(strncmp(variableName,"spImagesPrefix",14) == 0){
		strcpy(config->spImagesPrefix, value);
		memset(statusMSG, 0, MAX_STRING_LEN);
		strcpy(statusMSG, INTERNAL_STATUS_SUCCESS);
		*msg = SP_CONFIG_SUCCESS;
		return;
	}

	if(strncmp(variableName,"spImagesSuffix",14) == 0){
		if((strcmp(value, JPG) == 0) || (strcmp(value, PNG) == 0) ||
				(strcmp(value, BMP) == 0) || (strcmp(value, GIF) == 0)){
			strcpy(config->spImagesSuffix,value);
			memset(statusMSG, 0, MAX_STRING_LEN);
			strcpy(statusMSG, INTERNAL_STATUS_SUCCESS);
			*msg = SP_CONFIG_SUCCESS;
			return;
		}

		memset(statusMSG, 0, MAX_STRING_LEN);
		strcpy(statusMSG, INTERNAL_STATUS_CONSTRAIT_NOT_MET);
		printf(CONSTRAIT_NOT_MET_ERROR_MSG, __FILE__, __LINE__ );
		*msg = SP_CONFIG_INVALID_STRING;
		return;
	}

	if(strncmp(variableName,"spNumOfImages",13) == 0){
		if(isNumericValueValid(value)){
			int val = atoi(value);
			if(val > 0){
				config->spNumOfImages = val;
				memset(statusMSG, 0, MAX_STRING_LEN);
				strcpy(statusMSG, INTERNAL_STATUS_SUCCESS);
				*msg = SP_CONFIG_SUCCESS;
				return;
			}
		}

		*msg = SP_CONFIG_INVALID_INTEGER;
		memset(statusMSG, 0, MAX_STRING_LEN);
		strcpy(statusMSG, INTERNAL_STATUS_CONSTRAIT_NOT_MET);
		printf(CONSTRAIT_NOT_MET_ERROR_MSG, __FILE__, __LINE__ );//TODO change line to be in config file
		return;
	}
	if(strncmp(variableName,"spPCADimension", 14) == 0){

		if(isNumericValueValid(value)){
			int val = atoi(value);
			if(10 <= val && val <= 28){
				config->spPCADimension = val;
				memset(statusMSG, 0, MAX_STRING_LEN);
				strcpy(statusMSG, INTERNAL_STATUS_SUCCESS);
				*msg = SP_CONFIG_SUCCESS;
				return;
			}
		}

		*msg = SP_CONFIG_INVALID_INTEGER;
		memset(statusMSG, 0, MAX_STRING_LEN);
		strcpy(statusMSG, INTERNAL_STATUS_CONSTRAIT_NOT_MET);
		printf(CONSTRAIT_NOT_MET_ERROR_MSG, __FILE__, __LINE__ );
		return;
	}

	if(strncmp(variableName,"spPCAFilename",13) == 0){
		strcpy(config->spPCAFilename,value);
		memset(statusMSG, 0, MAX_STRING_LEN);
		strcpy(statusMSG, INTERNAL_STATUS_SUCCESS);
		*msg = SP_CONFIG_SUCCESS;
		return;
	}

	if(strncmp(variableName,"spNumOfFeatures",15) == 0){

		if(isNumericValueValid(value)){
			int val = atoi(value);
			if(val > 0){
				config->spNumOfFeatures = val;
				memset(statusMSG, 0, MAX_STRING_LEN);
				strcpy(statusMSG, INTERNAL_STATUS_SUCCESS);
				*msg = SP_CONFIG_SUCCESS;
				return;
			}
		}

		*msg = SP_CONFIG_INVALID_INTEGER;
		memset(statusMSG, 0, MAX_STRING_LEN);
		strcpy(statusMSG, INTERNAL_STATUS_CONSTRAIT_NOT_MET);
		printf(CONSTRAIT_NOT_MET_ERROR_MSG, __FILE__, __LINE__ );
		return;
	}

	if(strncmp(variableName,"spExtractionMode",16) == 0){
		if(strncmp(value,"true",4)==0){
			config->spExtractionMode = true;
			memset(statusMSG, 0, MAX_STRING_LEN);
			strcpy(statusMSG, INTERNAL_STATUS_SUCCESS);
			*msg = SP_CONFIG_SUCCESS;
			return;
		}else if(strncmp(value,"false",5)==0){
			config->spExtractionMode = false;
			memset(statusMSG, 0, MAX_STRING_LEN);
			strcpy(statusMSG, INTERNAL_STATUS_SUCCESS);
			*msg = SP_CONFIG_SUCCESS;
			return;
		}
		memset(statusMSG, 0, MAX_STRING_LEN);
		strcpy(statusMSG, INTERNAL_STATUS_CONSTRAIT_NOT_MET);
		printf(CONSTRAIT_NOT_MET_ERROR_MSG, __FILE__, __LINE__ );
		*msg = SP_CONFIG_INVALID_STRING;
		return;


	}
	if(strncmp(variableName,"spNumOfSimilarImages",20) == 0){
		int val = atoi(value);
		if(val > 0){
			config->spNumOfSimilarImages = val;
			memset(statusMSG, 0, MAX_STRING_LEN);
			strcpy(statusMSG, INTERNAL_STATUS_SUCCESS);
			*msg = SP_CONFIG_SUCCESS;
			return;
		}
		memset(statusMSG, 0, MAX_STRING_LEN);
		strcpy(statusMSG, INTERNAL_STATUS_CONSTRAIT_NOT_MET);
		printf(CONSTRAIT_NOT_MET_ERROR_MSG, __FILE__, __LINE__ );
		*msg = SP_CONFIG_INVALID_INTEGER;
		return;
	}


	if(strncmp(variableName,"spKDTreeSplitMethod",19) == 0){ //TODO
		if(strncmp(value, "RANDOM",6) == 0){
			config->spKDTreeSplitMethod = RANDOM;
			memset(statusMSG, 0, MAX_STRING_LEN);
			strcpy(statusMSG, INTERNAL_STATUS_SUCCESS);
			*msg = SP_CONFIG_SUCCESS;
			return;
		}
		if(strncmp(value, "MAX_SPREAD",10) == 0){
			config->spKDTreeSplitMethod = MAX_SPREAD;
			memset(statusMSG, 0, MAX_STRING_LEN);
			strcpy(statusMSG, INTERNAL_STATUS_SUCCESS);
			*msg = SP_CONFIG_SUCCESS;
			return;
		}
		if(strncmp(value, "INCREMENTAL",11) == 0){
			config->spKDTreeSplitMethod = INCREMENTAL;
			memset(statusMSG, 0, MAX_STRING_LEN);
			strcpy(statusMSG, INTERNAL_STATUS_SUCCESS);
			*msg = SP_CONFIG_SUCCESS;
			return;
		}

		memset(statusMSG, 0, MAX_STRING_LEN);
		strcpy(statusMSG, INTERNAL_STATUS_CONSTRAIT_NOT_MET);
		printf(CONSTRAIT_NOT_MET_ERROR_MSG, __FILE__, __LINE__ );
		*msg = SP_CONFIG_INVALID_STRING;
		return;

	}

	if(strncmp(variableName,"spKNN",5) == 0){
		int val = atoi(value);
		if(val > 0){
			config->spKNN = val;
			memset(statusMSG, 0, MAX_STRING_LEN);
			strcpy(statusMSG, INTERNAL_STATUS_SUCCESS);
			*msg = SP_CONFIG_SUCCESS;
			return;
		}
		memset(statusMSG, 0, MAX_STRING_LEN);
		strcpy(statusMSG, INTERNAL_STATUS_CONSTRAIT_NOT_MET);
		printf(CONSTRAIT_NOT_MET_ERROR_MSG, __FILE__, __LINE__ );
		*msg = SP_CONFIG_INVALID_INTEGER;
		return;
	}
	if(strncmp(variableName,"spMinimalGUI",12) == 0){
		if(strncmp(value,"true",4) == 0){
			config->spMinimalGUI = true;
			memset(statusMSG, 0, MAX_STRING_LEN);
			strcpy(statusMSG, INTERNAL_STATUS_SUCCESS);
			*msg = SP_CONFIG_SUCCESS;
			return;
		}else if(strncmp(value,"false",5) == 0){
			config->spMinimalGUI = false;
			memset(statusMSG, 0, MAX_STRING_LEN);
			strcpy(statusMSG, INTERNAL_STATUS_SUCCESS);
			*msg = SP_CONFIG_SUCCESS;
			return;
		}
		memset(statusMSG, 0, MAX_STRING_LEN);
		strcpy(statusMSG, INTERNAL_STATUS_CONSTRAIT_NOT_MET);
		printf(CONSTRAIT_NOT_MET_ERROR_MSG, __FILE__, __LINE__ );
		*msg = SP_CONFIG_INVALID_STRING;
		return;

	}
	if(strncmp(variableName,"spLoggerFilename",16) == 0){
		strcpy(config->spLoggerFilename,value);
		memset(statusMSG, 0, MAX_STRING_LEN);
		strcpy(statusMSG, INTERNAL_STATUS_SUCCESS);
		*msg = SP_CONFIG_SUCCESS;
		return;
	}

	if(strncmp(variableName,"spLoggerLevel",13) == 0){
		int val = atoi(value);
		if(val == 1 || val == 2 || val == 3 || val == 4){
			config->spLoggerLevel = val;
			memset(statusMSG, 0, MAX_STRING_LEN);
			strcpy(statusMSG, INTERNAL_STATUS_SUCCESS);
			*msg = SP_CONFIG_SUCCESS;
			return;
		}
		memset(statusMSG, 0, MAX_STRING_LEN);
		strcpy(statusMSG, INTERNAL_STATUS_CONSTRAIT_NOT_MET);
		printf(CONSTRAIT_NOT_MET_ERROR_MSG, __FILE__, __LINE__ );
		*msg = SP_CONFIG_INVALID_INTEGER;
		return;
	}


	/*line is invalid (neither a comment/empty line nor system parameter configuration)*/
	printf(INVALID_CONFIG_LINE_ERROR_MSG, __FILE__, __LINE__);
	memset(statusMSG, 0, MAX_STRING_LEN);
	strcpy(statusMSG, INTERNAL_STATUS_INVALID_CONFIG_LINE);
	*msg = SP_CONFIG_INVALID_STRING;
	return;

	/*
	int spLoggerLevel; default value= 3 {1,2,3,4} 1:error, 2:warning, 3:info, 4:debug
	 */
}

bool isLineValid(char* line){
	int i = 0;
	while(*(line+i) != '\0'){
		if(*(line+i) == '#'){ /* # in the middle of the line is not valid!*/
			return false;
		}
	}
	return true;
}

void getCleanWordFromString(char* string, char* word){
	//	printf("getCleanWordFromString: string=%s, word=%s\n",string,word);
	int firstLetterIndex = 0;
	int lastLetterIndex = 0;
	for(int i = 0; i < strlen(string); i++ ){
		if((*(string + i) != WHITESPACE) && (*(string + i) != TAB)){
			firstLetterIndex = i;
			break;
		}
	}
	//	printf("firstLetterIndex = %d\n",firstLetterIndex);
	//	printf("strlen(string)=%d\n",strlen(string));

	lastLetterIndex = strlen(string) - 1;
	for(int t = firstLetterIndex; t < strlen(string); t++){
		if((*(string + t) == WHITESPACE) || (*(string + t) == TAB)){
			//			printf("*(string + t) = %c\n",*(string + t));
			//if((*(string + t)) == WHITESPACE){
			lastLetterIndex = t;
			break;
		}
	}
	//	lastLetterIndex--;
	//	printf("lastLetterIndex = %d\n",lastLetterIndex);

	//printf("getCleanWordFromString: firstLetterIndex = %d\n",firstLetterIndex);
	int newWordLengthe = lastLetterIndex - firstLetterIndex + 1;
	//	printf("newWordLengthe = %d\n",newWordLengthe);
	for(int j = 0; j < newWordLengthe; j++){
		if(*(string + firstLetterIndex) != '\n'){
			*(word + j) = *(string + firstLetterIndex);
			firstLetterIndex++;
		}

	}

}



bool isStringValid(char* string){
	if(string == NULL){
		return false;
	}
	int stringLength = strlen(string);

	int i = 0;

	while(*(string+i) != '\0'){
		if(*(string+i) == HASH || (*(string+i) == WHITESPACE) || (*(string + i) == TAB) ||
				(*(string + i) == VERTICAL) || (*(string + i) == NEW_LINE)){ /* #,'\t','\r'.. in the middle of the line is not valid!*/
			return false;
		}
		i++;
	}
	return true;
}


bool isLineCommentLine(char* line){
	char c = ' ';
	int i = 0;
	while(c == WHITESPACE || c == TAB ){
		c = *(line + i);
		i++;
	}
	if(c == HASH){
		return true;
	}
	return false;
}

bool isNumericValueValid(char* number){
	char c = *number;
	int i = 0;

	while(c != '\0'){
		if(c == DECIMAL_POINT){
			return false;
		}
		c = *(number + i);
		i++;
	}

	return true;
}
void assignDefaultValues(SPConfig config){

	strcpy(config->spImagesDirectory, INTERNAL_DEFAULT_STRING_VALUE);
	strcpy(config->spImagesPrefix, INTERNAL_DEFAULT_STRING_VALUE);
	strcpy(config->spImagesSuffix, INTERNAL_DEFAULT_STRING_VALUE);
	config->spNumOfImages = -1;

	config->spPCADimension = 20;
	strcpy(config->spPCAFilename, "pca.yml");
	config->spNumOfFeatures = 100;
	config->spExtractionMode = true;
	config->spMinimalGUI = false;
	config->spNumOfSimilarImages = 1;
	config->spKNN = 1;
	config->spKDTreeSplitMethod = MAX_SPREAD;
	config->spLoggerLevel = 3;
	strcpy(config->spLoggerFilename, "stdout");

	return;
}

void printVariableValuesOfConfig(SPConfig config){
	SP_CONFIG_MSG msg = SP_CONFIG_SUCCESS;
	char imagePath[1024];
	char pcaPath[1024];

	printf("***********printing config values***********\n");
	if(config == NULL){
		printf("config is NULL\n");
		return;
	}

	SP_CONFIG_MSG getImagePathMSG = spConfigGetImagePath(imagePath, config, 1);
	SP_CONFIG_MSG getPCAPathMSG = spConfigGetPCAPath(pcaPath, config);

	printf("spImagesDirectory = %s\n",spConfigGetspImageDirectory(config));
	printf("spImagesPrefix = %s\n",spConfigGetspImagesPrefix(config));
	printf("spImagesSuffix = %s\n",spConfigGetspImagesSuffix(config));
	printf("spNumOfImages = %d\n",spConfigGetNumOfImages(config, &msg)); //X
	printf("spPCADimension = %d\n",spConfigGetPCADim(config, &msg)); //V
	printf("spPCAFilename = %s\n",spConfigGetspPCAFilename(config)); //V
	printf("spNumOfFeatures = %d\n",spConfigGetNumOfFeatures(config, &msg)); //V
	printf("spNumOfSimilarImages = %d\n",spConfigGetspNumOfSimilarImages(config)); //V
	printf("spKNN = %d\n",spConfigGetspKNN(config)); //V
	printf("spLoggerLevel = %d\n",spConfigGetspLoggerLevel(config)); //V
	printf("spLoggerFilename = %s\n",spConfigGetspLoggerFilename(config)); //V


	if(spConfigGetspKDTreeSplitMethod(config) == MAX_SPREAD){
		printf("spKDTreeSplitMethod = MAX_SPREAD\n");
	}else if(spConfigGetspKDTreeSplitMethod(config) == RANDOM){
		printf("spKDTreeSplitMethod = RANDOM\n");
	}else if(spConfigGetspKDTreeSplitMethod(config) == INCREMENTAL){
		printf("spKDTreeSplitMethod = INCREMENTAL\n");
	}else {
		printf("spConfigGetspKDTreeSplitMethod not working\n");
	}


	if(spConfigIsExtractionMode(config,&msg) == true){
		printf("spExtractionMode = %s\n","true");
	}else if(spConfigIsExtractionMode(config,&msg) == false){
		printf("spExtractionMode = %s\n","false");
	}else{
		printf("spConfigIsExtractionMode not working\n");
	}

	if(spConfigMinimalGui(config,&msg) == true){
		printf("spMinimalGUI = %s\n","true");
	}else if(spConfigMinimalGui(config,&msg) == false){
		printf("spMinimalGUI = %s\n","false");
	}else{
		printf("spConfigMinimalGui not working\n");
	}

}

/* Added by Maayan */
/* =============== */
SP_CONFIG_MSG spConfigGetImagePathFeat(char* imagePath, const SPConfig config,
		int index){
	if(imagePath == NULL || config == NULL){
		return SP_CONFIG_INVALID_ARGUMENT;
	}
	if(index >= config->spNumOfImages){
		return SP_CONFIG_INDEX_OUT_OF_RANGE;
	}
	memset(imagePath, 0, MAX_STRING_LEN);
	char indexAsString[1024] = {0};
	sprintf(indexAsString, "%d",index);
	strcat(imagePath,config->spImagesDirectory);
	strcat(imagePath,config->spImagesPrefix);
	strcat(imagePath,indexAsString);
	strcat(imagePath,FEAT);

	//printf("imagePath = %s\n",imagePath);
	return SP_CONFIG_SUCCESS;

}

SPConfig spConfigAlternativeCreate(){
	SPConfig config = (SPConfig)malloc(sizeof(struct sp_config_t));
	if(config == NULL){
		printf("allocation error\n");
		return NULL;
	}
	strcpy(config->spImagesDirectory, "./images/");
	strcpy(config->spImagesPrefix, "img");
	strcpy(config->spImagesSuffix, ".png");
	config->spNumOfImages = 17;
	//config->spNumOfImages = 3;
	//config->spPCADimension = 28;
	config->spPCADimension = 2;
	strcpy(config->spPCAFilename, "adigrabow.yml");
	//config->spNumOfFeatures = 20;
	config->spNumOfFeatures = 2;
	//config->spExtractionMode = true;
	config->spExtractionMode = false;
	config->spNumOfSimilarImages = 2;
	config->spKDTreeSplitMethod = MAX_SPREAD;
	config->spKNN = 3;
	config->spMinimalGUI = false;
	config->spLoggerLevel = 2;
	strcpy(config->spLoggerFilename, "stdout");

	return config;
}
