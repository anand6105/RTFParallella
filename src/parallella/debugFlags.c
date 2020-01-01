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


static unsigned int *btf_trace_buf[1];

static const task_section task_section_list[] = { {.taskId = 1, .base_address = 0x4000 },
                                     {.taskId = 2, .base_address = 0x4020 },
                                     {.taskId = 3, .base_address = 0x4040 },
                                     {.taskId = 4, .base_address = 0x4000 },
                                     {.taskId = 5, .base_address = 0x4020 },
                                     {.taskId = 7, .base_address = 0x4060 },
                                     {.taskId = 10, .base_address = 0x4080 },
                                     {.taskId = 13, .base_address = 0x40A0 },
                                     {.taskId = 16, .base_address = 0x4040 },
                                     {.taskId = 19, .base_address = 0x4060 }};


static unsigned int get_task_base_address(unsigned int taskId);
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

static unsigned int get_task_base_address(unsigned int taskId)
{
    unsigned int index;
    unsigned int entries = sizeof(task_section_list) /sizeof(task_section_list[0]);
    for(index = 1; index <= entries; index++)
    {
        if (taskId == task_section_list[index - 1].taskId)
        {
            return task_section_list[index - 1].base_address;
        }
    }
    return 0x00;
}
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
    *btf_trace_buf[TIME_FLAG] = 0x00;
    
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


void traceTaskEvent(int srcID, int srcInstance, btf_trace_event_type type,
        int taskId, int taskInstance, btf_trace_event_name event_name, int data)
{
    unsigned int base_address = get_task_base_address(taskId);
    if (base_address != 0x00)
    {
        unsigned int *address = base_address;
        *address = *btf_trace_buf[TIME_FLAG];
        address++;
        *address = srcID;
        address++;
        *address = srcInstance;
        address++;
        *address = type;
        address++;
        *address = taskId;
        address++;
        *address = taskInstance;
        address++;
        *address = event_name;
        address++;
        *address = data;
    }
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
