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

#ifndef SRC_PARALLELLA_SHARED_COMMS_H_
#define SRC_PARALLELLA_SHARED_COMMS_H_

#include <stdlib.h>
#include <stdint.h>


#define shared_mem_section	0x01001000

#define shared_mem_section_2	0x01001000

#define shared_mem_section1_label_count 10

#define SHM_DEFINED_SPACE	256 //number of words in the shared memory section

/**
 * allowable sizes of labels to be used i shared memory
 *
 * If the label is of larger size declare it as multiple labels
 *
 * TODO
 * add support for larger labels as combination of smaller ones
 */
typedef enum {
    UINT_8,
    UINT_16,
    UINT_32
} TYPE;


/**
 * defines a shared memory section
 *
 * Fields:
 * base_addr: address of the first label in the section
 * label_count: number of labels in the section
 * sec_type: data type of the section (size of labels in the section)
 *
 */
struct{
	unsigned int 	base_addr;
	unsigned 		label_count;
	TYPE			sec_type;
}typedef SHM_section;



/**
 * Initiate the shared label section, this function will assign addresses to labels in a section,
 * and initialize those labels to 0
 *
 * Arguments:
 *
 */
void shared_labels_init();

/**
 * write a value to a label in a shared memory(DRAM) section
 *
 * Arguments:
 * label_indx	:	index of the target shared label
 * payload		:	value to write
 *
 */
uint8_t shared_label_write	(int label_indx,int payload);

/**
 * read a value of a label in a shared memory(DRAM) section
 *
 * Arguments:
 * label_indx	:	index of the target shared label
 *
 * Return:
 * value of requested shared memory label
 */
unsigned int shared_label_read(int label_indx);



/**
 * Initiate the shared label section, this function will assign addresses to labels in a section,
 * and initialize those labels to the value of 256
 * If the requested section does not fit in the system's shared_dram, a null
 * pointer will be returned
 *
 * Arguments:
 * sec	:	structure of type SHM_section containing
 * 			details of the the shared memory section to be initiated
 *
 * Return:
 * pointer to the initiated shared memory label
 *
 *
 */
void* shm_section_init (SHM_section sec);

/**
 * Read data from a specific label in a shared memory section
 * This function will read one full label but the result will
 * be cast into int (4 bytes on this platform)
 *
 * Segmentation fault will occur for addresses outside the shared_dram section of the system
 * check RTFP documentation for details.
 *
 * Arguments:
 * x	:	pointer to the section to be read
 * indx	:	index of requested label
 *
 * Return:
 * value of requested label in a shared memory section
 *
 *
 */
int read_shm_section ( unsigned int* x, unsigned indx);

/**
 * Write data to a specific label in a shared memory section
 * This function will write one full label but the value will
 * be given as int (4 bytes on this platform) to avoid overflow issues
 *
 * Segmentation fault will occur for addresses outside the shared_dram section of the system
 * check RTFP documentation for details.
 *
 * Arguments:
 * x		:	pointer to the section to be written to
 * indx		:	index of requested label
 * payload	:	value to be written (will be cast into data type of target label)
 *
 */
void write_shm_section (unsigned int* x, unsigned indx, int payload);

/**
 * TODO
 * expand this function to use DMA engine of the Epi chip
 */
unsigned int shm_section_init_read (SHM_section sec,int index);


#endif /* SRC_PARALLELLA_SHARED_COMMS_H_ */
