#include "SPLogger.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#ifndef NULL
#define NULL   ((void *) 0)
#endif

//File open mode
#define SP_LOGGER_OPEN_MODE "w"

// Global variable holding the logger
SPLogger logger = NULL;

struct sp_logger_t {
	FILE* outputChannel; //The logger file
	bool isStdOut; //Indicates if the logger is stdout
	SP_LOGGER_LEVEL level; //Indicates the level
};

SP_LOGGER_MSG spLoggerCreate(const char* filename, SP_LOGGER_LEVEL level) {
	if (logger != NULL) { //Already defined
		return SP_LOGGER_DEFINED;
	}
	logger = (SPLogger) malloc(sizeof(*logger));
	if (logger == NULL) { //Allocation failure
		return SP_LOGGER_OUT_OF_MEMORY;
	}
	logger->level = level; //Set the level of the logger
	if (filename == NULL) { //In case the filename is not set use stdout
		logger->outputChannel = stdout;
		logger->isStdOut = true;
	} else { //Otherwise open the file in write mode
		logger->outputChannel = fopen(filename, SP_LOGGER_OPEN_MODE);
		if (logger->outputChannel == NULL) { //Open failed
			free(logger);
			logger = NULL;
			return SP_LOGGER_CANNOT_OPEN_FILE;
		}
		logger->isStdOut = false;
	}
	return SP_LOGGER_SUCCESS;
}

void spLoggerDestroy() {
	if (!logger) {
		return;
	}
	if (!logger->isStdOut) {//Close file only if not stdout
		fclose(logger->outputChannel);
	}
	free(logger);//free allocation
	logger = NULL;
}

SP_LOGGER_MSG spLoggerPrintError(const char* msg, const char* file, const char* function, const int line){
	/* No need to check logger type because errors are printed at any state*/

	 /* 	---ERROR---
	 * 	- file: <file>
	 *  - function: <function>
	 *  - line: <line>
	 *  - message: <msg>*/

	if (!logger){
		/* logger undefined */
		return SP_LOGGER_UNDIFINED;
	}
	if ( (msg == NULL) || (file == NULL) || (function == NULL) || (line<0) ){
		/*If any of msg or file or function are null or line is negative */
		return SP_LOGGER_INVAlID_ARGUMENT;
	}

	if (logger->isStdOut){
		return spLoggerGlobalPrintStdout(msg,file,function,line, "---ERROR---");
	}
	else{
		return spLoggerGlobalPrintFile(msg,file,function,line, "---ERROR---");
	}
}

SP_LOGGER_MSG spLoggerPrintWarning(const char* msg, const char* file, const char* function, const int line){
	if (logger->level == SP_LOGGER_ERROR_LEVEL)  /*msg will not be printed in ERROR state*/
		return SP_LOGGER_SUCCESS;

	if (!logger){
			/* logger undefined */
			return SP_LOGGER_UNDIFINED;
		}
		if ( (msg == NULL) || (file == NULL) || (function == NULL) || (line<0) ){
			/*If any of msg or file or function are null or line is negative */
			return SP_LOGGER_INVAlID_ARGUMENT;
		}

		if (logger->isStdOut){
			return spLoggerGlobalPrintStdout(msg,file,function,line, "---WARNING---");
		}
		else{
			return spLoggerGlobalPrintFile(msg,file,function,line, "---WARNING---");
		}

}


SP_LOGGER_MSG spLoggerPrintDebug(const char* msg, const char* file, const char* function, const int line){
	if (logger->level == SP_LOGGER_ERROR_LEVEL || logger->level == SP_LOGGER_WARNING_ERROR_LEVEL ||
			logger->level == SP_LOGGER_INFO_WARNING_ERROR_LEVEL )
		/*msg will not be printed in ERROR,warning,info state*/
			return SP_LOGGER_SUCCESS;

	if (!logger){
				/* logger undefined */
				return SP_LOGGER_UNDIFINED;
			}
			if ( (msg == NULL) || (file == NULL) || (function == NULL) || (line<0) ){
				/*If any of msg or file or function are null or line is negative */
				return SP_LOGGER_INVAlID_ARGUMENT;
			}

			if (logger->isStdOut){
				return spLoggerGlobalPrintStdout(msg,file,function,line, "---DEBUG---");
			}
			else{
				return spLoggerGlobalPrintFile(msg,file,function,line, "---DEBUG---");
			}


}

SP_LOGGER_MSG spLoggerPrintInfo(const char* msg){

	if (logger->level == SP_LOGGER_ERROR_LEVEL || logger->level == SP_LOGGER_WARNING_ERROR_LEVEL)
			/*msg will not be printed in ERROR,warning state*/
				return SP_LOGGER_SUCCESS;


	if (!logger){
				/* logger undefined */
				return SP_LOGGER_UNDIFINED;
			}
	if ( (msg == NULL) ){
				/*If any of msg or file or function are null or line is negative */
			return SP_LOGGER_INVAlID_ARGUMENT;
	}

	if (logger->isStdOut){
		if (printf("---INFO---\n")<0)
			return SP_LOGGER_WRITE_FAIL;
		if (printf("- message: %s\n",msg)<0)
			return SP_LOGGER_WRITE_FAIL;
		return SP_LOGGER_SUCCESS;

				}
	else{
		if (fprintf(logger->outputChannel,"---INFO---\n")<0)
			return SP_LOGGER_WRITE_FAIL;
		if (fprintf(logger->outputChannel,"- message: %s\n",msg)<0)
			return SP_LOGGER_WRITE_FAIL;
		return SP_LOGGER_SUCCESS;

				}
}

SP_LOGGER_MSG spLoggerPrintMsg(const char* msg){
				if (!logger){
					/* logger undefined */
					return SP_LOGGER_UNDIFINED;
				}
				if ( (msg == NULL) ){
					/*If any of msg or file or function are null or line is negative */
					return SP_LOGGER_INVAlID_ARGUMENT;
				}

				if (logger->isStdOut){
					if (printf("%s\n",msg)<0)
						return SP_LOGGER_WRITE_FAIL;
				}
				else{
					if (fprintf(logger->outputChannel,"%s\n",msg)<0)
						return SP_LOGGER_WRITE_FAIL;
				}
				return SP_LOGGER_SUCCESS;
}


SP_LOGGER_MSG spLoggerGlobalPrintStdout(const char* msg, const char* file, const char* function,
		const int line, const char*  typeOfMsg){


	if (printf("%s\n",typeOfMsg)<0) /* write headline */
				return SP_LOGGER_WRITE_FAIL;

	if (printf("- file: %s\n",file)<0)
		return SP_LOGGER_WRITE_FAIL;
	if (printf("- function: %s\n",function)<0)
		return SP_LOGGER_WRITE_FAIL;
	if (printf("- line: %d\n",line)<0)
		return SP_LOGGER_WRITE_FAIL;
	if (printf("- message: %s\n",msg)<0)
		return SP_LOGGER_WRITE_FAIL;

	return SP_LOGGER_SUCCESS;
}

SP_LOGGER_MSG spLoggerGlobalPrintFile(const char* msg, const char* file, const char* function,
		const int line, const char*  typeOfMsg){

	FILE* outputFile = logger->outputChannel;
	if (fprintf(outputFile,"%s\n",typeOfMsg)<0) /* write headline */
				return SP_LOGGER_WRITE_FAIL;

	if (fprintf(outputFile,"- file: %s\n",file)<0)
		return SP_LOGGER_WRITE_FAIL;
	if (fprintf(outputFile,"- function: %s\n",function)<0)
		return SP_LOGGER_WRITE_FAIL;
	if (fprintf(outputFile,"- line: %d\n",line)<0)
		return SP_LOGGER_WRITE_FAIL;
	if (fprintf(outputFile,"- message: %s\n",msg)<0)
		return SP_LOGGER_WRITE_FAIL;

	return SP_LOGGER_SUCCESS;
}
