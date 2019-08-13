/*
 * AmaltheaConverter.c
 *
 *  Created on: Jun 23, 2019
 *      Author: bigm
 */


#include "AmaltheaConverter.h"

#include "debugFlags.h"
#include "FreeRTOS.h"
#include "task.h"


AmaltheaTask createAmaltheaTask(void *taskHandler,void *cInHandler,void *cOutHandler,unsigned int period,unsigned int deadline, unsigned int WCET){
	if (WCET >= period){
		AmaltheaTask retValNull = {0,0,NULL,0,0,0,NULL,NULL};
		return retValNull;
	}else{
		AmaltheaTask retVal = {0,0,taskHandler,WCET,deadline,period,cInHandler,cOutHandler};
		return retVal;
	}
}

unsigned int calculateStackSize(int labelBitCount, int labelCount){
	return 20 + ((labelBitCount*labelBitCount)/PLATFORM_WORD_LENGTH);
}


#ifdef use_LET_COMM_SEMANTICS
void generalizedRTOSTak(AmaltheaTask task){
	TickType_t xLastWakeTime = xTaskGetTickCount();
	//task.cInHandler();
	for (;;){
		//execute cIn
		task.cInHandler();
		task.taskHandler();
		vTaskDelayUntil( &xLastWakeTime, task.period);
		task.cOutHandler();
	}
}
#else

void generalizedRTOSTak(AmaltheaTask task){
	TickType_t xLastWakeTime = xTaskGetTickCount();
	//task.cInHandler();
	for (;;){
		//execute cIn
		task.cInHandler();
		task.taskHandler();
		task.cOutHandler();
		vTaskDelayUntil( &xLastWakeTime, task.period);
	}
}

#endif











