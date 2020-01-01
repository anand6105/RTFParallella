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

#include "shared_comms.h"
#include "debugFlags.h"
#include "e-lib.h"


typedef unsigned int e_label_size; // one word = 4 bytes = unsigned int

unsigned int *outbuf_shared[SHM_DEFINED_SPACE]; //array for global section access API

//The current SHM access API
void* shm_section_init (SHM_section sec){
	unsigned size_in_bytes = 0;
	typedef unsigned int* cast_type;
	if (sec.sec_type == UINT_8){
		typedef uint8_t* cast_type;
		size_in_bytes = sec.label_count * sizeof(uint8_t);
	}else if (sec.sec_type == UINT_16){
		typedef uint16_t* cast_type;
		size_in_bytes = sec.label_count * sizeof(uint16_t);
	}else if (sec.sec_type == UINT_32){
		typedef uint32_t* cast_type;
		size_in_bytes = sec.label_count * sizeof(uint32_t);
	}else{
		size_in_bytes = sec.label_count * sizeof(unsigned int);
	}
	//check if section fits in shm_dram section of the memory
	if ((sec.base_addr | size_in_bytes)<0x01000000 || (sec.base_addr | size_in_bytes)>=0x02000000){
		//will cause segmentation fault in Epi!
		updateDebugFlag(404);
		return NULL;
	}
	cast_type retval;
	retval = (cast_type ) (0x8e000000 | sec.base_addr);// assign to  the ABSOLUTE address of memory section
	int i = 0;
	for (i = 0;i<sec.label_count;i++){
		retval[i] = 256;
	}
	return retval;
}

int read_shm_section ( unsigned int* x, unsigned indx){
	return (int) x[indx];
}

void write_shm_section (unsigned int* x, unsigned indx, int payload){
	x[indx] = payload;
}

/**
 * TODO use e_write/ e_read functions and DMA to constrain contention from different cores.
 */
unsigned int shm_section_init_read (SHM_section sec,int index){
	typedef unsigned int cast_type;
	if (sec.sec_type == UINT_8){
		typedef uint8_t cast_type;
	}else if (sec.sec_type == UINT_16){
		typedef uint16_t cast_type;
	}else if (sec.sec_type == UINT_32){
		typedef uint32_t cast_type;
	}
	unsigned int *retval;
	e_write((void*)&e_emem_config, retval, 0, 0, (void *)0x01000000, sizeof(unsigned int));
	return (unsigned int) retval;
}



/**
 * obsolete utility functions
 * specific for global section access.
 *
 */


void shared_labels_init(){
	outbuf_shared[0] = (unsigned int *) (0x8e000000 | shared_mem_section);
	for (int j = 1;j<SHM_DEFINED_SPACE;j++){
		outbuf_shared[j] = outbuf_shared[j-1] + 1;
	}
	//initialize buffer
	int i;
	//timer1init();
	for (i=0;i<SHM_DEFINED_SPACE;i++){
		*outbuf_shared[i] =0x10000;
	}
}

uint8_t shared_label_write(int label_indx,int payload){
	uint8_t retval= 0;
	*outbuf_shared[label_indx] = payload;
	/*if (payload == *outbuf_shared[label_indx]){
		retval = 1;
	}*/
	return retval;
}


unsigned int shared_label_read(int label_indx){
	return *outbuf_shared[label_indx];
}


//-------------------end of file-------------------------//
