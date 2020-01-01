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

//epiphany imports
#include <stdlib.h>

#include "AmaltheaConverter.h"
#include "c2c.h"
#include "debugFlags.h"
#include "shared_comms.h"
#include "taskCode.h"
#include "label_man_core0.h"
#include "e_lib.h"
//freeRTOS imports
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
//utility imports
//unsigned int* global_shm_sec_ptr;

//---------------------------------------------
int main(void) {

#ifdef RFTP_GENERATE_BTF_TRACE
    btf_trace_buf_init();
#endif
#ifdef ENABLE_SHARED_LABEL
	//initialize output buffer for debug messages
	outbuf_init();
	shared_labels_init();
	init_mem_sections();
	shared_labels_init_core();
#endif	
	//create Amalthea task objects
#ifdef RFTP_GENERATE_BTF_TRACE
	AmaltheaTask t5ms =  createAmaltheaTask(handler5ms,cIn5ms,cOut5ms,5,25, 0, 1, 0, 5,2);
	AmaltheaTask t10ms = createAmaltheaTask(handler10ms,cIn10ms,cOut10ms,10,25, 0, 2, 0, 10,3);
	AmaltheaTask t20ms = createAmaltheaTask(handler20ms,cIn20ms,cOut20ms,20,25, 0, 3, 0,20,5);
#else
	AmaltheaTask t5ms =  createAmaltheaTask(handler5ms,cIn5ms,cOut5ms,5, 5,2);
	AmaltheaTask t10ms = createAmaltheaTask(handler10ms,cIn10ms,cOut10ms,10, 10,3);
	AmaltheaTask t20ms = createAmaltheaTask(handler20ms,cIn20ms,cOut20ms,20,20,5);
#endif
	//create RTOS task from templates
	createRTOSTask(&t5ms,3,0);
	createRTOSTask(&t10ms,2,0);
	createRTOSTask(&t20ms,1,0);
	//start RTOS scheduler
	vTaskStartScheduler();
	return EXIT_SUCCESS;
}
//---------------------------------------------
//end of file
