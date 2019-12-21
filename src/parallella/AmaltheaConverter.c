/*******************************************************************************
 *   Copyright (c) 2019 Dortmund University of Applied Sciences and Arts and others.
 *  
 *   This program and the accompanying materials are made
 *   available under the terms of the Eclipse Public License 2.0
 *   which is available at https://www.eclipse.org/legal/epl-2.0/
 *  
 *   SPDX-License-Identifier: EPL-2.0
 *  
 *   Contributors:
 *        Dortmund University of Applied Sciences and Arts - initial API and implementation
 *******************************************************************************/
#include "AmaltheaConverter.h"

#include "FreeRTOS.h"
#include "task.h"
#include <stdarg.h>
#include "debugFlags.h"
#include "trace_utils_BTF.h"

#ifdef RFTP_GENERATE_BTF_TRACE
AmaltheaTask createAmaltheaTask(void *taskHandler,void *cInHandler,void *cOutHandler,
                    unsigned int period, unsigned int srcId, unsigned int srcInstance,
                    unsigned int taskId, unsigned int taskInstance,
                    unsigned int deadline, unsigned int WCET){
    if (WCET >= period){
        AmaltheaTask retValNull = {0,0, 0,0,0,0,NULL,0,0,0,NULL,NULL};
        return retValNull;
    }else{
        AmaltheaTask retVal = {0,0,srcId, srcInstance, taskId, taskInstance, taskHandler,WCET,deadline,period,cInHandler,cOutHandler};
        return retVal;
    }
}
#else
AmaltheaTask createAmaltheaTask(void *taskHandler,void *cInHandler,void *cOutHandler,
                    unsigned int period, unsigned int deadline, unsigned int WCET){
    if (WCET >= period){
        AmaltheaTask retValNull = {0,0,NULL,0,0,0,NULL,NULL};
        return retValNull;
    }else{
        AmaltheaTask retVal = {0,0, taskHandler,WCET,deadline,period,cInHandler,cOutHandler};
        return retVal;
    }
}
#endif


unsigned int calculateStackSize(int labelBitCount, int labelCount){
    return ((labelBitCount*labelCount)/PLATFORM_WORD_LENGTH);
}

void createRTOSTask(AmaltheaTask* task, int priority, int argCount, ...){
    //cycle through the stack arguments and add the needed numbers to the stack
    int stack_size = 0;
    for (int i=3;i<=argCount+2;i+=2){
        stack_size = calculateStackSize(i,i+1);
    }
    //make sure the stack size is at least big enough to run the task.
    stack_size += configMINIMAL_STACK_SIZE;
    //create the RTOS task with the generalized form
    xTaskCreate(generalizedRTOSTask    ,"Task"    ,stack_size,    &(*task)    ,priority,NULL);
}


#ifdef use_LET_COMM_SEMANTICS
void generalizedRTOSTask(AmaltheaTask task){
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

void generalizedRTOSTask(AmaltheaTask task){
    TickType_t xLastWakeTime = xTaskGetTickCount();
    //task.cInHandler();
    for (;;){
        //execute cIn
#ifdef RFTP_GENERATE_BTF_TRACE
        updateBTFTraceBuffer(task.srcId, task.srcInstance, TASK_EVENT, task.taskId,
                                task.taskInstance, PROCESS_START, 0);
        task.cInHandler(task.taskId, task.taskInstance);
        task.taskHandler(task.taskId, task.taskInstance);
        task.cOutHandler(task.taskId, task.taskInstance);
        updateBTFTraceBuffer(task.srcId, task.srcInstance, TASK_EVENT, task.taskId,
                                task.taskInstance, PROCESS_TERMINATE, 0);
        vTaskDelayUntil( &xLastWakeTime, task.period);
        task.taskInstance++;
#else
        task.cInHandler();
        task.taskHandler();
        task.cOutHandler();
        vTaskDelayUntil( &xLastWakeTime, task.period);
#endif
    }
}

#endif

