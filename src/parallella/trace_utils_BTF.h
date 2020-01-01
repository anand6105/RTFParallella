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

#ifndef SRC_PARALLELLA_TRACE_UTILS_BTF_H_
#define SRC_PARALLELLA_TRACE_UTILS_BTF_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define RFTP_GENERATE_BTF_TRACE 0x01

#define BTF_TRACE_TRUE     0x01
#define BTF_TRACE_FALSE    0x00


/* BTF trace flags */
#define BTF_TRACE_BUFFER_SIZE           8
#define TIME_FLAG                       0
#define SOURCE_FLAG                     1
#define SOURCE_INSTANCE_FLAG            2
#define EVENT_TYPE_FLAG                 3
#define TARGET_FLAG                     4
#define TARGET_INSTANCE_FLAG            5
#define EVENT_FLAG                      6
#define DATA_FLAG                       7

/* Enum for event type ID */
typedef enum btf_trace_event_type_t
{
    TASK_EVENT,
    INT_SERVICE_ROUTINE_EVENT,
    RUNNABLE_EVENT,
    INS_BLOCK_EVENT,
    STIMULUS_EVENT,
    ECU_EVENT,
    PROCESSOR_EVENT,
    CORE_EVENT,
    SCHEDULER_EVENT,
    SIGNAL_EVENT,
    SEMAPHORE_EVENT,
    SIMULATION_EVENT
} btf_trace_event_type;

/* Enum for event name */
typedef enum btf_trace_event_name_t
{
    INIT = -1,
    PROCESS_START,
    PROCESS_TERMINATE,
    PROCESS_PREEMPT,
    PROCESS_SUSPEND,
    PROCESS_RESUME,
    SIGNAL_READ,
    SIGNAL_WRITE
} btf_trace_event_name;


/**
 * Structure to hold BTF Header
 */
typedef struct btf_trace_header_config_t
{
    uint8_t creator[64];          /* Target device on which the trace is generated */
    uint8_t modelfile[512];       /* Model file used to generate the trace */
    uint8_t timescale[4];         /* Time scale e.g ns, ms, us, s..*/
} btf_trace_header_config_t;

/* BTF structure for storing the entity configuration */
typedef struct btf_trace_entity_entry_t
{
    uint16_t entity_id;                 /* Entity ID to get the entity name */
    int16_t instance;                   /* Current instance of the entity */
    btf_trace_event_name state;         /* Current state of the entity */
    btf_trace_event_type entity_type;   /* Entity type to get the source*/
    uint8_t entity_name[64];            /* Entity name */

} btf_trace_entity_entry;

/* Structure to hold BTF trace data in task stack fro processing */
typedef struct btf_trace_data_t
{
    int32_t ticks;                     /* Not used currently */
    int32_t srcId;                     /* Source Id */
    int32_t srcInstance;               /* Instance of the source */
    int32_t eventTypeId;               /* Type of event Runnable , Task etc.. */
    int32_t taskId;                    /* Task Id */
    int32_t taskInstance;              /* Instance of the task */
    int32_t eventState;                /* State of the event */
    int32_t data;                      /* Notes */
} btf_trace_data;

/* Entity table structure */
typedef struct btf_trace_entity_table_t
{
    uint16_t is_occupied;                 /* If 0, entry is available else not available */
    btf_trace_entity_entry entity_data;   /* Entity details */
} btf_trace_entity_table;

/* Function to get the trace file path from current working directory */
void get_btf_trace_file_path(char *trace_file_path);

/* Function to parse the command line arguments for generating the BTF trace file. */
void parse_btf_trace_arguments(int argc, char **argv);

/* Function to write the trace header config */
void write_btf_trace_header_config(FILE *stream);

/* Function to write entity type to trace file */
void write_btf_trace_header_entity_type(FILE *stream, btf_trace_event_type type);

/* Function to write the entity table in the header section */
void write_btf_trace_header_entity_table(FILE *stream);

/* Function to write the entity type table in the BTF header section */
void write_btf_trace_header_entity_type_table(FILE *stream);

/* Function to store the entry for all the entities */
void store_entity_entry(uint16_t typeId, btf_trace_event_type type, uint8_t *name);

/* Function to write the data section of the BTF */
void write_btf_trace_data(FILE *stream, uint8_t core_id, unsigned int * data_buffer);

#endif /* SRC_PARALLELLA_TRACE_UTILS_BTF_H_ */
