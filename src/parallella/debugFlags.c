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

#include "debugFlags.h"

#include "FreeRTOS.h"
#include "task.h"

#ifdef ENABLE_SHARED_LABEL
unsigned int *outbuf[10];
#endif

#ifdef RFTP_GENERATE_BTF_TRACE
volatile unsigned int *ptick_count = (void *)0x3200;
volatile unsigned int *psource_id = (void *)0x3204;
volatile unsigned int *psource_instance = (void *)0x3208;
volatile unsigned int *ptarget_id = (void *)0x320C;
volatile unsigned int *ptarget_instance = (void *)0x3210;
volatile unsigned int *pevent_id = (void *)0x3214;
volatile unsigned int *ptype_id = (void *)0x3218;
volatile unsigned int *psig_data = (void *)0x3220;

static unsigned int source_id;
static unsigned int source_instance;
static unsigned int target_id;
static unsigned int target_instance;
static unsigned int event_id;
static unsigned int type_id;
static unsigned int sig_data;

#endif


/*
 * initialize output buffer with the addresses to array elements
 */
void outbuf_init(void ){
#ifdef ENABLE_SHARED_LABEL
    //This will be the same on each core
    //debug interface in shared memory
    outbuf[0] = (unsigned int *) cnt_address;
    //initialize buffer
    int i;
    //timer1init();
    for (i=0;i<9;i++){
        *outbuf[i] = 0;
    }
#endif
}


#ifdef RFTP_GENERATE_BTF_TRACE_
/**
 * Function to initialize the BTF trace buffer.
 *
 * Arguments:
 * void
 *
 * Return: void
 */
void btf_trace_buf_init(void)
{
    int index;
    btf_trace_buf[0] = (unsigned int *) btf_trace_address;
    //initialize buffer
    for (index = 0;index < BTF_TRACE_BUFFER_SIZE; index++){
        *btf_trace_buf[index] = 0x00;
    }
}

#endif

void traceRunningTask(unsigned taskNum){
#ifdef ENABLE_SHARED_LABEL
    *outbuf[RUNNINGTASK_FLAG] = taskNum;
#endif
}

void traceTaskPasses(unsigned taskNum, int currentPasses){
#ifdef ENABLE_SHARED_LABEL
    if (taskNum == 1){
        *outbuf[TASK1_FLAG] = currentPasses;
    }else if (taskNum == 2){
        *outbuf[TASK2_FLAG] = currentPasses;
    }else if (taskNum == 3){
        *outbuf[TASK3_FLAG] = currentPasses;
    }
#endif
}

void updateTick(void){
#ifdef ENABLE_SHARED_LABEL
    *outbuf[TICK_FLAG] = xTaskGetTickCount();
#endif
    *ptick_count = xTaskGetTickCount();
}

void updateDebugFlag(int debugMessage){
#ifdef ENABLE_SHARED_LABEL
    *outbuf[DEBUG_FLAG] = debugMessage;
#endif
}

#ifdef RFTP_GENERATE_BTF_TRACE
void updateBTFTraceBuffer(int srcID, int srcInstance, btf_trace_event_type type,
        int targetId, int targetInstance, btf_trace_event_name event_name, int data)
{
    //*btf_trace_buf[TIME_FLAG] = xTaskGetTickCount();
    source_id = srcID;
    source_instance = srcInstance;
    type_id = type;
    target_id = targetId;
    target_instance = targetInstance;
    event_id = event_name;
    sig_data = data;
}


void write_to_btf_trace(void)
{
    *psource_id = source_id;
    *psource_instance = source_instance;
    *ptype_id = type_id;
    *ptarget_id = target_id;
    *ptarget_instance = target_instance;
    *pevent_id = event_id;
    *psig_data = sig_data;

}
#endif
