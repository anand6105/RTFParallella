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
#if 0
unsigned int *ptick_count = (void *)0x4000;
unsigned int *psource_id = (void *)0x4004;
unsigned int *psource_instance = (void *)0x4008;
unsigned int *ptype_id = (void *)0x400C;
unsigned int *ptarget_id = (void *)0x4010;
unsigned int *ptarget_instance = (void *)0x4014;
unsigned int *pevent_id = (void *)0x4018;
unsigned int *psig_data = (void *)0x4020;

static unsigned int source_id;
static unsigned int source_instance;
static unsigned int target_id;
static unsigned int target_instance;
static unsigned int event_id;
static unsigned int type_id;
static unsigned int sig_data;

#endif

unsigned int *btf_trace_buf[BTF_TRACE_BUFFER_SIZE];



#endif


/*
 * initialize output buffer with the addresses to array elements
 */
void outbuf_init(void ){
#ifdef ENABLE_SHARED_LABEL
    //This will be the same on each core
    //debug interface in shared memory
    outbuf[0] = (unsigned int *) cnt_address;
    outbuf[1] = outbuf[0] + 1;
    outbuf[2] = outbuf[1] + 1;
    outbuf[3] = outbuf[2] + 1;
    outbuf[4] = outbuf[3] + 1;
    outbuf[5] = outbuf[4] + 1;
    outbuf[6] = outbuf[5] + 1;
    outbuf[7] = outbuf[6] + 1;
    outbuf[8] = outbuf[7] + 1;
    outbuf[9] = outbuf[8] + 1;
    //initialize buffer
    int i;
    //timer1init();
    for (i=0;i<9;i++){
        *outbuf[i] = 0;
    }
#endif
}


#ifdef RFTP_GENERATE_BTF_TRACE
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
    btf_trace_buf[1] = btf_trace_buf[0] + 1;
    btf_trace_buf[2] = btf_trace_buf[1] + 1;
    btf_trace_buf[3] = btf_trace_buf[2] + 1;
    btf_trace_buf[4] = btf_trace_buf[3] + 1;
    btf_trace_buf[5] = btf_trace_buf[4] + 1;
    btf_trace_buf[6] = btf_trace_buf[5] + 1;
    btf_trace_buf[7] = btf_trace_buf[6] + 1;
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
#ifdef RFTP_GENERATE_BTF_TRACE 
    *btf_trace_buf[TIME_FLAG] = xTaskGetTickCount();
#endif
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
    *btf_trace_buf[SOURCE_FLAG] = srcID;
    *btf_trace_buf[SOURCE_INSTANCE_FLAG] = srcInstance;
    *btf_trace_buf[EVENT_TYPE_FLAG] = type;
    *btf_trace_buf[TARGET_FLAG] = targetId;
    *btf_trace_buf[TARGET_INSTANCE_FLAG] = targetInstance;
    *btf_trace_buf[EVENT_FLAG] = event_name;
    *btf_trace_buf[DATA_FLAG] = data;
#if 0
    source_id = srcID;
    source_instance = srcInstance;
    type_id = type;
    target_id = targetId;
    target_instance = targetInstance;
    event_id = event_name;
    sig_data = data;
#endif
}


void write_to_btf_trace(void)
{
#if 0
    *psource_id = source_id;
    *psource_instance = source_instance;
    *ptype_id = type_id;
    *ptarget_id = target_id;
    *ptarget_instance = target_instance;
    *pevent_id = event_id;
    *psig_data = sig_data;
#endif
}
#endif
