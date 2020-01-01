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

#include "taskCode.h"

/* define global variables to be
 * accessed through a specific task (context)
 */
//task recurrence counters
int passes1 = 0;
int passes2 = 0;
int passes3 = 0;
int passes4 = 0;
int passes5 = 0;



//define tasks and copy operations here
//-------
#ifdef RFTP_GENERATE_BTF_TRACE
void handler5ms(int srcId, int srcInstance){
#else
void handler5ms(){
#endif
    int localLabel;
    updateDebugFlag(700);
#ifdef RFTP_GENERATE_BTF_TRACE
    traceTaskEvent(srcId, srcInstance, RUNNABLE_EVENT, 7,
            passes1, PROCESS_START, 0);
#endif
    sleepTimerMs(2,1);
#ifdef RFTP_GENERATE_BTF_TRACE
    traceTaskEvent(srcId, srcInstance, RUNNABLE_EVENT, 7,
            passes1, PROCESS_TERMINATE, 0);
#endif
    passes1++;
    traceTaskPasses(1,passes1);
    traceRunningTask(0);
}


#ifdef RFTP_GENERATE_BTF_TRACE
void handler10ms(int srcId, int srcInstance){
#else
void handler10ms(){
#endif
	updateDebugFlag(800);
#ifdef RFTP_GENERATE_BTF_TRACE
    traceTaskEvent(srcId, srcInstance, RUNNABLE_EVENT, 10,
    		passes2, PROCESS_START, 0);
#endif
    sleepTimerMs(3,2);
#ifdef RFTP_GENERATE_BTF_TRACE
    traceTaskEvent(srcId, srcInstance, RUNNABLE_EVENT, 10,
    		passes2, PROCESS_TERMINATE, 0);
#endif
    passes2++;
    traceTaskPasses(2,passes2);
    traceRunningTask(0);

}
#ifdef RFTP_GENERATE_BTF_TRACE
void handler20ms(int srcId, int srcInstance){
#else
void handler20ms(){
#endif
	updateDebugFlag(899);
#ifdef RFTP_GENERATE_BTF_TRACE
    traceTaskEvent(srcId, srcInstance, RUNNABLE_EVENT, 13,
    		passes3, PROCESS_START, 0);
#endif
    sleepTimerMs(5,3);
#ifdef RFTP_GENERATE_BTF_TRACE
    traceTaskEvent(srcId, srcInstance, RUNNABLE_EVENT, 13,
    		passes3, PROCESS_TERMINATE, 0);
#endif
    passes3++;
    traceRunningTask(0);
    traceTaskPasses(3,passes3);

}


#ifdef RFTP_GENERATE_BTF_TRACE
void handler10msCore2(int srcId, int srcInstance){
#else
void handler10msCore2(){
#endif
    updateDebugFlag(899);
#ifdef RFTP_GENERATE_BTF_TRACE
    traceTaskEvent(srcId, srcInstance, RUNNABLE_EVENT, 16,
    		passes4, PROCESS_START, 0);
#endif
    sleepTimerMs(3,4);
#ifdef RFTP_GENERATE_BTF_TRACE
    traceTaskEvent(srcId, srcInstance, RUNNABLE_EVENT, 16,
    		passes4, PROCESS_TERMINATE, 0);
#endif
    passes4++;
    //shared_label1 = shared_label_to_read;
    traceRunningTask(0);
    traceTaskPasses(2,passes4);
}


#ifdef RFTP_GENERATE_BTF_TRACE
void handler20msCore2(int srcId, int srcInstance){
#else
void handler20msCore2(){
#endif
    updateDebugFlag(899);
#ifdef RFTP_GENERATE_BTF_TRACE
    traceTaskEvent(srcId, srcInstance, RUNNABLE_EVENT, 19,
    		passes5, PROCESS_START, 0);
#endif
    sleepTimerMs(7,5);
#ifdef RFTP_GENERATE_BTF_TRACE
    traceTaskEvent(srcId, srcInstance, RUNNABLE_EVENT, 19,
    		passes5, PROCESS_TERMINATE, 0);
#endif
    passes5++;
    traceRunningTask(0);
    traceTaskPasses(3,passes5);
}






