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

#include "c2c.h"

#include "e-lib.h"

dstr_mem_sec_1_label_type *outbuf_dstr_shared[10];
//e_mem_t emem_dst[16];
int core_write_mutex=0;

void shared_labels_init_core(){
	//shared buffer in core memory
	outbuf_dstr_shared[0] = (dstr_mem_sec_1_label_type *) dstr_mem_offset_sec_1;
	outbuf_dstr_shared[1] = outbuf_dstr_shared[0] + 1;
    outbuf_dstr_shared[2] = outbuf_dstr_shared[1] + 1;
    outbuf_dstr_shared[3] = outbuf_dstr_shared[2] + 1;
    outbuf_dstr_shared[4] = outbuf_dstr_shared[3] + 1;
    outbuf_dstr_shared[5] = outbuf_dstr_shared[4] + 1;
    outbuf_dstr_shared[6] = outbuf_dstr_shared[5] + 1;
    outbuf_dstr_shared[7] = outbuf_dstr_shared[6] + 1;
    outbuf_dstr_shared[8] = outbuf_dstr_shared[7] + 1;
    outbuf_dstr_shared[9] = outbuf_dstr_shared[8] + 1;
	//initialize buffer
	int i;
	for (i=0;i<9;i++){
		*outbuf_dstr_shared[i] =0;
	}
}

uint8_t shared_label_write_core	(unsigned row,unsigned col,int label_indx,int payload){
	unsigned int *addr;
	unsigned int* addr_base;
	addr_base = (unsigned int *)get_base_address_core(row,col);
	addr = (unsigned int*) ((unsigned ) addr_base | (unsigned)outbuf_dstr_shared[label_indx]);
	*addr = payload;
}


void DSHM_section_init(DSHM_section sec){
	unsigned int *addr;
	unsigned int* addr_base;
	addr_base = (unsigned int *)get_base_address_core(sec.row,sec.col);
	addr = (unsigned int*) ((unsigned ) addr_base | (unsigned)sec.base_addr);
	for (int i = 0;i<sec.label_count;i++){
		addr[i] = 256;
	}
}


uint8_t write_DSHM_section	(DSHM_section sec,int label_indx,int payload){
	unsigned int *addr;
	unsigned int* addr_base;
	addr_base = (unsigned int *)get_base_address_core(sec.row,sec.col);
	addr = (unsigned int*) ((unsigned ) addr_base | (unsigned)sec.base_addr);
	addr[label_indx] = payload;
}

unsigned int read_DSHM_section (DSHM_section sec, int label_indx){
	unsigned int *addr;
	unsigned int* addr_base;
	addr_base = (unsigned int *)get_base_address_core(sec.row,sec.col);
	addr = (unsigned int*) ((unsigned ) addr_base | (unsigned)sec.base_addr);
	return addr[label_indx];
}





unsigned int shared_label_read_core (unsigned row, unsigned col, int label_indx){
	unsigned int *addr;
	unsigned int* addr_base;
	addr_base = (unsigned int *)get_base_address_core(row,col);
	addr = (unsigned int*) ((unsigned ) addr_base | (unsigned)outbuf_dstr_shared[label_indx]);
	return addr;
}


unsigned int get_base_address_core(int row, int col){
	if(row ==0 ){
		if (col == 0){
			return 0x80800000;
		} else if (col ==1){
			return 0x80900000;
		}else if (col == 2){
			return 0x80A00000;
		}else if (col == 3){
			return 0x80B00000;
		}
	} else if(row ==1 ){
		if (col == 0){
			return 0x84800000;
		} else if (col ==1){
			return 0x84900000;
		}else if (col == 2){
			return 0x84A00000;
		}else if (col == 3){
			return 0x84B00000;
		}
	} else if(row ==2 ){
		if (col == 0){
			return 0x88800000;
		} else if (col ==1){
			return 0x88900000;
		}else if (col == 2){
			return 0x88A00000;
		}else if (col == 3){
			return 0x88B00000;
		}
	} else if(row ==3 ){
		if (col == 0){
			return 0x8C800000;
		} else if (col ==1){
			return 0x8C900000;
		}else if (col == 2){
			return 0x8CA00000;
		}else if (col == 3){
			return 0x8CB00000;
		}
	}
}
