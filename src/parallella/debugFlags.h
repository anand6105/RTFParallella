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

#ifndef SRC_PARALLELLA_DEBUGFLAGS_H_
#define SRC_PARALLELLA_DEBUGFLAGS_H_

#include "trace_utils_BTF.h"

#define cnt_address     0x3000
//#ifdef RFTP_GENERATE_BTF_TRACE
    #define btf_trace_address    0x3200
//#endif

#define TASK1_FLAG             2
#define TASK2_FLAG             0
#define TASK3_FLAG             4
#define TASK4_FLAG             0
#define TASK5_FLAG             4
#define RUNNINGTASK_FLAG       6
#define DEBUG_FLAG             7
#define TICK_FLAG              8

//#define ENABLE_SHARED_LABEL    0x01


/**
 * initialize output buffer in core memory
 *
 * Arguments:
 *
 */
void outbuf_init(void );

/**
 * write the index of the running task to output buffer
 *
 * Arguments:
 * taskNum            :    index of the task
 *
 */
void traceRunningTask(unsigned taskNum);

/**
 * write the task instance (job) to output buffer
 *
 * Arguments:
 * taskNum            :    index of the task
 * currentPasses    :    instance of task (job number)
 *
 */
void traceTaskPasses(unsigned taskNum, int currentPasses);

/**
 * update RTOS tick value in output buffer
 *
 * Arguments:
 *
 */
void updateTick(void);

/**
 * Write a custom value to the output buffer for code coverage debugging
 *
 * Arguments:
 * debugMessage            :    message to be written
 *
 */
void updateDebugFlag(int debugMessage);

#ifdef RFTP_GENERATE_BTF_TRACE
void updateBTFTraceBuffer(int srcID, int srcInstance, btf_trace_event_type type,
        int targetId, int targetInstance, btf_trace_event_name event_name, int data);

void write_to_btf_trace(void);

//void btf_trace_buf_init(void);
#endif


#endif /* SRC_PARALLELLA_DEBUGFLAGS_H_ */
