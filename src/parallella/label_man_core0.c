/*******************************************************************************
 *     Copyright (c) 2019 Dortmund University of Applied Sciences and Arts and others.
 *    
 *     This program and the accompanying materials are made
 *     available under the terms of the Eclipse Public License 2.0
 *     which is available at https://www.eclipse.org/legal/epl-2.0/
 *    
 *     SPDX-License-Identifier: EPL-2.0
 *    
 *     Contributors:
 *          Dortmund University of Applied Sciences and Arts - initial API and implementation
 *******************************************************************************/

#include "label_man_core0.h"


//create shm sections' structs
SHM_section sec1 = {0x01001000,10};
DSHM_section sec1_core_00 = {0,0,0x4000,10};
DSHM_section sec1_core_10 = {1,0,0x4000,10};


//pointer declarations for shm sections accessed by this core
unsigned int* sec1_global_pointer;
unsigned int* sec2_global_pointer;

//initialize shm sections and assign pointers

void init_mem_sections(void){
	sec1_global_pointer = shm_section_init(sec1);
}


void init_DSHM_sections(void){
	DSHM_section_init(sec1_core_00);
	DSHM_section_init(sec1_core_10);
}



//extern unsigned int shared_label1;

int label5_10_00;

int label5_10_00_copy1;

int shared_label_1;
int shared_label_2 = 97;
int shared_label_10 = 0;

extern int passes1;
#ifdef RFTP_GENERATE_BTF_TRACE
static int cIn5msInstance;
static int cIn10msInstance;
static int cIn20msInstance;
static int cIn10msCore2Instance;
static int cIn20msCore2Instance;
static int cOut5msInstance;
static int cOut10msInstance;
static int cOut20msInstance;
static int cOut10msCore2Instance;
static int cOut20msCore2Instance;
#endif

//-------
#ifdef RFTP_GENERATE_BTF_TRACE
void cIn5ms(int srcId, int srcInstance){
    updateBTFTraceBuffer(srcId, srcInstance, RUNNABLE_EVENT, 6,
    		cIn5msInstance, PROCESS_START, 0);
#else
void cIn5ms(){
#endif
	label5_10_00_copy1 = label5_10_00;
	shared_label_2 = 97 + passes1;
#ifdef RFTP_GENERATE_BTF_TRACE
    updateBTFTraceBuffer(srcId, srcInstance, RUNNABLE_EVENT, 6,
    		cIn5msInstance, PROCESS_TERMINATE, 0);
    cIn5msInstance++;
#endif
}


#ifdef RFTP_GENERATE_BTF_TRACE
void cIn10ms(int srcId, int srcInstance){
    updateBTFTraceBuffer(srcId, srcInstance, RUNNABLE_EVENT, 9,
    		cIn10msInstance, PROCESS_START, 0);
    updateBTFTraceBuffer(srcId, srcInstance, RUNNABLE_EVENT, 9,
    		cIn10msInstance, PROCESS_TERMINATE, 0);
    cIn10msInstance++;
}
#else
void cIn10ms(){

}
#endif


#ifdef RFTP_GENERATE_BTF_TRACE
void cIn20ms(int srcId, int srcInstance){
    updateBTFTraceBuffer(srcId, srcInstance, RUNNABLE_EVENT, 12,
    		cIn20msInstance, PROCESS_START, 0);
    updateBTFTraceBuffer(srcId, srcInstance, RUNNABLE_EVENT, 12,
    		cIn20msInstance, PROCESS_TERMINATE, 0);
    cIn20msInstance++;
}
#else
void cIn20ms(){

}
#endif


#ifdef RFTP_GENERATE_BTF_TRACE
void cIn10msCore2(int srcId, int srcInstance){
    updateBTFTraceBuffer(srcId, srcInstance, RUNNABLE_EVENT, 15,
    		cIn10msCore2Instance, PROCESS_START, 0);
#else
void cIn10msCore2(){
#endif
#ifdef ENABLE_SHARED_LABEL
	shared_label_1 = read_DSHM_section(sec1_core_00,0);
	//shared_label_1 = shared_label_read_core(1,0,0);
#endif

#ifdef RFTP_GENERATE_BTF_TRACE
    updateBTFTraceBuffer(srcId, srcInstance, RUNNABLE_EVENT, 15,
    		cIn10msCore2Instance, PROCESS_TERMINATE, 0);
    cIn10msCore2Instance++;
#endif
}

#ifdef RFTP_GENERATE_BTF_TRACE
void cIn20msCore2(int srcId, int srcInstance){
    updateBTFTraceBuffer(srcId, srcInstance, RUNNABLE_EVENT, 18,
    		cIn20msCore2Instance, PROCESS_START, 0);
#else
void cIn20msCore2(){
#endif

#ifdef ENABLE_SHARED_LABEL
	shared_label_10 = read_shm_section(sec1_global_pointer,0);
#endif

#ifdef RFTP_GENERATE_BTF_TRACE
    updateBTFTraceBuffer(srcId, srcInstance, RUNNABLE_EVENT, 18,
    		cIn20msCore2Instance, PROCESS_TERMINATE, 0);
    cIn20msCore2Instance++;
#endif
}

//-------
#ifdef RFTP_GENERATE_BTF_TRACE
void cOut5ms(int srcId, int srcInstance){
    updateBTFTraceBuffer(srcId, srcInstance, RUNNABLE_EVENT, 8,
    		cOut5msInstance, PROCESS_START, 0);
#else
void cOut5ms(){
#endif
	label5_10_00 = label5_10_00_copy1;
	write_DSHM_section(sec1_core_10,0,shared_label_2);
#ifdef RFTP_GENERATE_BTF_TRACE
    updateBTFTraceBuffer(1, 0, RUNNABLE_EVENT, 8,
    		0, PROCESS_TERMINATE, 0);
    cOut5msInstance++;
#endif
	//shared_label_write_core(1,0,0,shared_label_2);
}

#ifdef RFTP_GENERATE_BTF_TRACE
void cOut10ms(int srcId, int srcInstance){
    updateBTFTraceBuffer(srcId, srcInstance, RUNNABLE_EVENT, 11,
    		cOut10msInstance, PROCESS_START, 0);
#else
void cOut10ms(){
#endif
	write_shm_section(sec1_global_pointer,0,2);
	//shared_label_write(0,2);
#ifdef RFTP_GENERATE_BTF_TRACE
    updateBTFTraceBuffer(2, 0, RUNNABLE_EVENT, 11,
    		0, PROCESS_TERMINATE, 0);
    cOut10msInstance++;
#endif
}


#ifdef RFTP_GENERATE_BTF_TRACE
void cOut20ms(int srcId, int srcInstance){
    updateBTFTraceBuffer(srcId, srcInstance, RUNNABLE_EVENT, 14,
    		cOut20msInstance, PROCESS_START, 0);
    updateBTFTraceBuffer(srcId, srcInstance, RUNNABLE_EVENT, 14,
    		cOut20msInstance, PROCESS_TERMINATE, 0);
    cOut20msInstance++;
}
#else
void cOut20ms(){

}
#endif

#ifdef RFTP_GENERATE_BTF_TRACE
void cOut10msCore2(int srcId, int srcInstance){
    updateBTFTraceBuffer(srcId, srcInstance, RUNNABLE_EVENT, 17,
    		cOut10msCore2Instance, PROCESS_START, 0);
#else
void cOut10msCore2(){
#endif
	shared_label_1++;
#ifdef ENABLE_SHARED_LABEL
	write_shm_section(sec1_global_pointer,1,shared_label_1);
	//shared_label_write(1,shared_label_1);
#endif

#ifdef RFTP_GENERATE_BTF_TRACE
    updateBTFTraceBuffer(srcId, srcInstance, RUNNABLE_EVENT, 17,
    		cOut10msCore2Instance, PROCESS_TERMINATE, 0);
    cOut10msCore2Instance++;
#endif
}

#ifdef RFTP_GENERATE_BTF_TRACE
void cOut20msCore2(int srcId, int srcInstance){
    updateBTFTraceBuffer(srcId, srcInstance, RUNNABLE_EVENT, 20,
    		cOut20msCore2Instance, PROCESS_START, 0);
#else
    void cOut20msCore2(){
#endif

#ifdef ENABLE_SHARED_LABEL
	write_shm_section(sec1_global_pointer,3,shared_label_10 + 1);
	//shared_label_write(3,shared_label_10 + 1);
#endif

#ifdef RFTP_GENERATE_BTF_TRACE
    updateBTFTraceBuffer(srcId, srcInstance, RUNNABLE_EVENT, 20,
    		cOut20msCore2Instance, PROCESS_TERMINATE, 0);
    cOut20msCore2Instance++;
#endif
}
