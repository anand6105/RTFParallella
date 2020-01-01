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
#include <getopt.h>
#include <time.h>
#include "trace_utils_BTF.h"
#include <limits.h>
#include <unistd.h>
/*------------------------------DEFINES-------------------------*/
#define BTF_TRACE_ENTITY_TABLE_SIZE                 64
#define CORE_STACK_SIZE                             16
#define TRACE_PATH_SIZE                             512

/*-------------------------GOLBAL VARIABLES--------------------*/
const char *btf_trace_version = "#version 1.0";

/*-------------------------STATIC GOLBAL VARIABLES--------------------*/
static btf_trace_header_config_t btf_header;
static uint8_t output_trace_path[TRACE_PATH_SIZE];
static btf_trace_entity_table entity_table[BTF_TRACE_ENTITY_TABLE_SIZE];
static uint8_t isEntityTypeHeaderWritten = BTF_TRACE_FALSE;
static uint8_t isEntityTableHeaderWritten = BTF_TRACE_FALSE;
static uint8_t isEntityTypTableHeaderWritten = BTF_TRACE_FALSE;
static btf_trace_data core0_trace_data[CORE_STACK_SIZE];
static btf_trace_data core1_trace_data[CORE_STACK_SIZE];
static int8_t core0_stack_top = -1;
static int8_t core1_stack_top = -1;


/*-------------------------CONST VARIABLES---------------------------*/
const uint8_t event_type[][8] = {
        "T",
        "ISR",
        "R",
        "IB",
        "STI",
        "ECU",
        "P",
        "C",
        "SCHED",
        "SIG",
        "SEM",
        "SIM"
};

const uint8_t event_name[][16] = {
        "start",
        "terminate",
        "preempt",
        "suspend",
        "resume",
        "read",
        "write"
};


/*-------------------------STATIC FUNCTIONS----------------------------*/
static void print_usage(void);
static void get_trace_timestamp(uint8_t *buffer);
static int16_t find_first_free_index(void);
static void get_trace_timestamp(uint8_t *buffer);
static uint8_t update_entity_entry(unsigned int id, unsigned int instance, unsigned int event_state);
static void process_btf_trace_data(FILE *stream, btf_trace_data *data, int8_t *top_of_stack, unsigned int *data_buffer);


/* Function to get the first free available index */
static int16_t find_first_free_index(void)
{
    int index = 0;
    for(index = 0; index < BTF_TRACE_ENTITY_TABLE_SIZE; index++)
    {
        if (entity_table[index].is_occupied == 0x00)
        {
            return index;
        }
    }
    return -1;
}

/* Function to get the entity name based on the id passed */
static unsigned char * get_entity_name(unsigned int id)
{
    int index = 0;
    for(index = 0; index < BTF_TRACE_ENTITY_TABLE_SIZE; index++)
    {
        if (entity_table[index].is_occupied == 0x01)
        {
            if (id == entity_table[index].entity_data.entity_id)
            {
                return entity_table[index].entity_data.entity_name;
            }
        }
    }
    return NULL;
}

/* Function to get the current time of creation of btf trace file */
static void get_trace_timestamp(uint8_t *buffer)
{
    time_t timer;
    char date[16] = {0};
    char time_t[16] = {0};
    struct tm* tm_info;
    time(&timer);
    tm_info = localtime(&timer);
    /* The total number of characters to display time is 26 */
    strftime((char *)date, 16, "%Y-%m-%d", tm_info);
    strftime((char *)time_t, 16, "%H:%M:%S", tm_info);
    /* Set the time in ISO 8601 extended specification format */
    snprintf((char *)buffer, 26, "%s%c%s", date, 'T', time_t);
}

/* Function to display to usage of the command line parameters */
static void print_usage(void)
{
    fprintf(stdout, "Usage:\n");
    fprintf(stdout,"\t[-t|--trace-btf]=<Output trace file name.>\n");
    fprintf(stdout,"\t[-m|--model-file]=<Model file name used to generate the trace file.>\n");
    fprintf(stdout,"\t[-s|--scale]=<Timing scale used to generate the trace file.>\n");
    fprintf(stdout,"\t[-d|--device]=<Device target on which the trace file is generated.>\n");
    fprintf(stdout,"\t[-h|--help]=<Print the help message.>\n");
    fflush(stdout);
}

/* Function to update the entity entry table */
static uint8_t update_entity_entry(unsigned int id, unsigned int instance, unsigned int event_state)
{
    int index;
    //Parse the entity table to check for any new event or instance.
    for(index = 0; index < BTF_TRACE_ENTITY_TABLE_SIZE; index++)
    {
        if ((entity_table[index].is_occupied == 0x01) && (id == entity_table[index].entity_data.entity_id))
        {
            if ((entity_table[index].entity_data.instance != instance) ||
                (entity_table[index].entity_data.state != event_state))
                {
                    entity_table[index].entity_data.instance = instance;
                    entity_table[index].entity_data.state = event_state;
                    return BTF_TRACE_TRUE;
                }
        }
    }
    return BTF_TRACE_FALSE;
}

/* Function to push any entity event on core stack */
void push_on_stack(btf_trace_data *data, int8_t *top_of_stack, unsigned int *data_buffer)
{
    int8_t stack_top = *top_of_stack;
    if(stack_top >= CORE_STACK_SIZE - 1)
    {
        fprintf(stdout, "\n\tSTACK is over flow");
        return;
    }
    stack_top++;
    data[stack_top].eventTypeId = data_buffer[EVENT_TYPE_FLAG];
    data[stack_top].srcId = data_buffer[SOURCE_FLAG];
    data[stack_top].srcInstance = data_buffer[SOURCE_INSTANCE_FLAG];
    data[stack_top].taskId = data_buffer[TARGET_FLAG];
    data[stack_top].taskInstance = data_buffer[TARGET_INSTANCE_FLAG];
    data[stack_top].eventState = data_buffer[EVENT_FLAG];
    data[stack_top].data = data_buffer[DATA_FLAG];
    *top_of_stack = stack_top;
}

/* Function to pop any entity event on core stack */
btf_trace_data pop_from_stack(btf_trace_data *data, int8_t *top_of_stack)
{
    btf_trace_data task = {0};
    int8_t stack_top = *top_of_stack;
    if(stack_top <= -1)
    {
        fprintf(stdout, "\n\t Stack is under flow");
        return task;
    }
    else
    {
        task = data[stack_top];
        stack_top--;
        *top_of_stack = stack_top;
        return task;
    }
}

/* Function to find the current active task */
static int find_task_in_execution(btf_trace_data *data, int8_t stack_top, btf_trace_event_type type)
{
    int index = -1;
    int found = -1;
    for(index = 0; index <= stack_top; index++)
    {
        if (data[index].eventTypeId == type && ((data[index].eventState == PROCESS_START)
                || (data[index].eventState == PROCESS_RESUME)))
        {
            found = index;
        }
    }
    return found;
}


/* Function to dump the BTF trace data on output trace file */
static void dump_btf_trace_data(FILE *stream, unsigned int ticks,
                            unsigned int srcId, unsigned int srcInstance,
                            btf_trace_event_type type,
                            unsigned int target, unsigned int targetInstance,
                            btf_trace_event_name event, unsigned int data)
{
    unsigned char * source_name = get_entity_name(srcId);
    unsigned char * target_name = get_entity_name(target);
    const unsigned char *event_type_string = event_type[type];
    const unsigned char *event_name_string = event_name[event];
    if ((source_name != NULL) && (target_name != NULL))
    {
        fprintf(stream,"%d,%s,%d,%s,%s,%d,%s,%d\n", ticks, source_name, srcInstance,
                 event_type_string, target_name, targetInstance, event_name_string, data);
    }
}

/* Function to process event on each core */
static void process_btf_trace_data(FILE *stream, btf_trace_data *data, int8_t *top_of_stack, unsigned int *data_buffer)
{
    if (*top_of_stack == -1)
    {
        push_on_stack(data, top_of_stack, data_buffer);
        dump_btf_trace_data(stream, data_buffer[TIME_FLAG], data_buffer[SOURCE_FLAG],
                data_buffer[SOURCE_INSTANCE_FLAG], data_buffer[EVENT_TYPE_FLAG],
                data_buffer[TARGET_FLAG], data_buffer[TARGET_INSTANCE_FLAG],
                data_buffer[EVENT_FLAG], data_buffer[DATA_FLAG]);
    }
    else
    {
        btf_trace_data previousTask;
        if (data_buffer[EVENT_FLAG] == PROCESS_START)
        {
            //Preempt the current running task and suspend the associated runnable
            int task = find_task_in_execution(data, *top_of_stack, TASK_EVENT);
            int runnable = find_task_in_execution(data, *top_of_stack, RUNNABLE_EVENT);
            if((task != -1) && (data[task].taskId != data_buffer[SOURCE_FLAG]))
            {
                // print task to preemption
                data[task].eventState = PROCESS_PREEMPT;
                dump_btf_trace_data(stream, data_buffer[TIME_FLAG], data[task].srcId,
                        data[task].srcInstance, data[task].eventTypeId,
                        data[task].taskId, data[task].taskInstance,
                        data[task].eventState, data[task].data);
            }
            if((runnable != -1) && (data[runnable].srcId != data_buffer[TARGET_FLAG]))
            {
                // print runnable to suspend
                data[runnable].eventState = PROCESS_SUSPEND;
                dump_btf_trace_data(stream, data_buffer[TIME_FLAG], data[runnable].srcId,
                        data[runnable].srcInstance, data[runnable].eventTypeId,
                        data[runnable].taskId, data[runnable].taskInstance,
                        data[runnable].eventState, data[runnable].data);
            }
            //print the task which started and push it on the stack
            push_on_stack(data, top_of_stack, data_buffer);
            dump_btf_trace_data(stream, data_buffer[TIME_FLAG], data_buffer[SOURCE_FLAG],
                    data_buffer[SOURCE_INSTANCE_FLAG], data_buffer[EVENT_TYPE_FLAG],
                    data_buffer[TARGET_FLAG], data_buffer[TARGET_INSTANCE_FLAG],
                    data_buffer[EVENT_FLAG], data_buffer[DATA_FLAG]);
        }
        else if (data_buffer[EVENT_FLAG] == PROCESS_TERMINATE)
        {
            previousTask = data[*top_of_stack];
            if (data_buffer[EVENT_TYPE_FLAG] == RUNNABLE_EVENT)
            {
                if((data_buffer[SOURCE_FLAG] == previousTask.taskId)
                        && (previousTask.eventState == PROCESS_TERMINATE)
                        &&(previousTask.eventTypeId == TASK_EVENT))
                {
                    // The previous event is a TASK and associated to current runnable in terminated state.
                    // pop the terminated task
                    btf_trace_data terminated_task = pop_from_stack(data, top_of_stack);
                    dump_btf_trace_data(stream, data_buffer[TIME_FLAG], data_buffer[SOURCE_FLAG],
                            data_buffer[SOURCE_INSTANCE_FLAG], data_buffer[EVENT_TYPE_FLAG],
                            data_buffer[TARGET_FLAG], data_buffer[TARGET_INSTANCE_FLAG],
                            data_buffer[EVENT_FLAG], data_buffer[DATA_FLAG]);
                    dump_btf_trace_data(stream, data_buffer[TIME_FLAG], terminated_task.srcId, terminated_task.srcInstance,
                            terminated_task.eventTypeId, terminated_task.taskId, terminated_task.taskInstance,
                            terminated_task.eventState, terminated_task.data);
                   //pop the task and runnable start state.
                    pop_from_stack(data, top_of_stack);
                    pop_from_stack(data, top_of_stack);
                    // Resume the previous tasks
                    if (*top_of_stack >= 1)
                    {
                        previousTask = data[*top_of_stack];
                        int8_t prevRunnable = *top_of_stack - 1;
                        btf_trace_data runnableTask = data[prevRunnable];
                        dump_btf_trace_data(stream, data_buffer[TIME_FLAG], previousTask.srcId, previousTask.srcInstance,
                                previousTask.eventTypeId, previousTask.taskId, previousTask.taskInstance,
                                PROCESS_RESUME, previousTask.data);
                        dump_btf_trace_data(stream, data_buffer[TIME_FLAG], runnableTask.srcId, runnableTask.srcInstance,
                                runnableTask.eventTypeId, runnableTask.taskId, runnableTask.taskInstance,
                                PROCESS_RESUME, runnableTask.data);
                        data[*top_of_stack].eventState = PROCESS_RESUME;
                        data[prevRunnable].eventState = PROCESS_RESUME;
                    }
                }
                else
                {
                    // Runnable event has no associated task in terminated state
                    push_on_stack(data, top_of_stack, data_buffer);
                }
            }
            else if(data_buffer[EVENT_TYPE_FLAG] == TASK_EVENT)
            {
                push_on_stack(data, top_of_stack, data_buffer);
            }
        }
        else
        {
            //Do nothing
        }
    }
}

/* Function to get the file name of the trace file along with the
 * absoulte path.
 *
 * Arguments:
 * @out_param trace_file_path  : Pointer to the buffer where the BTF trace file path
 *                                  is stored.
 *
 * Return: void
 */
void get_btf_trace_file_path(char *trace_file_path)
{
    if (trace_file_path == NULL)
    {
        return;
    }
    char lcwd[PATH_MAX-1];

    if (getcwd(lcwd, sizeof(lcwd)) != NULL) {
        fprintf(stderr,"Current working dir: %s\n", lcwd);
    } else {
        perror("getcwd() error");
        return;
    }
    if(0 != access(lcwd, W_OK))
    {
        fprintf(stderr,"You don't have write access to the directory in which you are trying to create the btf file\n");
    }
    sprintf(trace_file_path,"%s" "%c" "%s",lcwd,'/',output_trace_path);
    fprintf(stderr,"trace_file_path = %s\n",trace_file_path);
    fflush(stderr);
}

/**
 * Parse the command line arguments for generating the BTF trace file
 *
 * Arguments:
 * @in_param argc  : The count for the number of arguments passed
 * @in_param argv  : Pointer to the list of arguments
 *
 * Return: void
 */
void  parse_btf_trace_arguments(int argc, char **argv)
{
    int opt= 0;
    int is_time_scale_provided = BTF_TRACE_FALSE;

    //Specifying the expected options
    //The two options l and b expect numbers as argument
    static const struct option long_options[] = {
        {"trace-btf", required_argument, NULL, 't' },
        {"model-file", required_argument, NULL,  'm' },
        {"scale", optional_argument, NULL, 's'},
        {"device", required_argument, NULL, 'd'},
        {"help", no_argument, NULL, 'h'},
        {NULL, 0, NULL, 0}
    };

    int long_index =0;
    while ((opt = getopt_long(argc, argv,"t:m:s:d:",
                   long_options, &long_index )) != -1) {
        switch (opt) {
             case 't' :
                 strncpy((char *)output_trace_path, (const char *)optarg, sizeof(output_trace_path));
                 break;
             case 'm' :
                 strncpy((char *)btf_header.modelfile, (const char *)optarg, sizeof(btf_header.modelfile));
                 break;
             case 'd' :
                 strncpy((char *)btf_header.creator, (const char *)optarg, sizeof(btf_header.creator));
                 break;
             case 's' :
                 strncpy((char *)btf_header.timescale, (const char *)optarg, sizeof(btf_header.timescale));
                 is_time_scale_provided = BTF_TRACE_TRUE;
                 break;
             case 'h' :
                 print_usage();
                 exit(EXIT_SUCCESS);
             case '?' :
             default  :
                 print_usage();
                 exit(EXIT_FAILURE);
        }
    }
    /* Set the default time scale as ns */
    if (is_time_scale_provided == BTF_TRACE_FALSE)
    {
        strncpy((char *)btf_header.timescale, (const char *)"ns", sizeof(btf_header.timescale));
    }

}



/**
 * Store the entity metadata which can be used to generate the entity
 * type and entity type table.
 *
 * Arguments:
 * @in_param typeId  : Unique entity type ID
 * @in_param type    : Entity type..e.g TASK, RUNNABLE etc..
 * @in_param name    : Entity name
 *
 * Return: void
 */
void store_entity_entry(uint16_t typeId, btf_trace_event_type type, uint8_t *name)
{
    int16_t index = 0;
    index = find_first_free_index();
    if (index >= 0)
    {
        entity_table[index].entity_data.entity_id = typeId;
        entity_table[index].entity_data.instance = -1;
        entity_table[index].entity_data.state = INIT;
        entity_table[index].entity_data.entity_type = type;
        strcpy((char *)entity_table[index].entity_data.entity_name, (const char *)name);
        entity_table[index].is_occupied = 0x01;
    }
}


/**
 * Function to write BTF header data to the trace file.
 * It writes the version, creator, input model file,
 * timescale and timestamp section of the header file.
 *
 * Arguments:
 * @in_param stream  : File pointer to the stream where the data has to be
 *                     written.
 *
 * Return: void
 */
void write_btf_trace_header_config(FILE *stream)
{
    if (stream == NULL)
    {
        return;
    }
    /* 32 bytes is enough to store the timescale format */
    uint8_t time_string[32] = {0};
    fprintf(stream, "%s\n", btf_trace_version);
    fprintf(stream, "#creator %s\n", btf_header.creator);
    get_trace_timestamp(time_string);
    fprintf(stream, "#creationdate %s\n", time_string);
    fprintf(stream, "#inputFile %s\n", btf_header.modelfile);
    fprintf(stream, "#timescale %s\n", btf_header.timescale);
    fflush(stream);
}

/**
 * Function to write entity type in BTF header data
 *
 * Arguments:
 * @in_param stream  : File pointer to the stream where the data has to be
 *                     written.
 * @in_param type    : Type of the entity i.e. TASK, RUNNABLE, STIMULUS etc..
 *
 * Return: void
 */
void write_btf_trace_header_entity_type(FILE *stream, btf_trace_event_type type)
{
    if (stream == NULL)
    {
        return;
    }
    if (isEntityTypeHeaderWritten == BTF_TRACE_FALSE)
    {
        fprintf(stream, "#entityType\n");
        isEntityTypeHeaderWritten = BTF_TRACE_TRUE;
    }
    fprintf(stream, "#-%d %s\n", type, event_type[type]);
    fflush(stream);
}


/**
 * Function to write entity type in BTF header data
 *
 * Arguments:
 * @in_param stream  : File pointer to the stream where the data has to be
 *                     written.
 * @in_param type    : Type of the entity i.e. TASK, RUNNABLE, STIMULUS etc..
 *
 * Return: void
 */
void write_btf_trace_header_entity_type_table(FILE *stream)
{
    if (stream == NULL)
    {
        return;
    }
    if (isEntityTypTableHeaderWritten == BTF_TRACE_FALSE)
    {
        fprintf(stream, "#entityTypeTable\n");
        isEntityTypTableHeaderWritten = BTF_TRACE_TRUE;
    }
    int index = 0;
    for(index = 0; index < BTF_TRACE_ENTITY_TABLE_SIZE; index++)
    {
        if (entity_table[index].is_occupied == 0x01)
        {
            int type_index = entity_table[index].entity_data.entity_type;
            fprintf(stream, "#-%s %s\n", event_type[type_index],
                            entity_table[index].entity_data.entity_name);
            fflush(stream);
        }
    }
}


/**
 * Function to write entity type in BTF header data
 *
 * Arguments:
 * @in_param stream  : File pointer to the stream where the data has to be
 *                     written.
 * @in_param type    : Type of the entity i.e. TASK, RUNNABLE, STIMULUS etc..
 *
 * Return: void
 */
void write_btf_trace_header_entity_table(FILE *stream)
{
    if (stream == NULL)
    {
        return;
    }
    if (isEntityTableHeaderWritten == BTF_TRACE_FALSE)
    {
        fprintf(stream, "#entityTable\n");
        isEntityTableHeaderWritten = BTF_TRACE_TRUE;
    }
    int index = 0;
    for(index = 0; index < BTF_TRACE_ENTITY_TABLE_SIZE; index++)
    {
        if (entity_table[index].is_occupied == 0x01)
        {
            fprintf(stream, "#-%d %s\n", entity_table[index].entity_data.entity_id,
                    entity_table[index].entity_data.entity_name);
            fflush(stream);
        }
    }
}

/**
 * Function to write the data section of the BTF
 *
 * Arguments:
 * @in_param stream        : File pointer to the stream where the data has to be
 *                            written.
 * @in_param core_id       : Core ID on which the task operations are performed
 * @in_param data_buffer   : Data buffer containing the BTF trace information.
 *
 * Return: void
 */
void write_btf_trace_data(FILE *stream, uint8_t core_id, unsigned int * data_buffer)
{
    if (stream == NULL || (data_buffer == NULL))
    {
        return;
    }
    unsigned char * source_name = get_entity_name(data_buffer[SOURCE_FLAG]);

    // Do not print idle tasks.
    if (strcmp((const char *)source_name,"[idle]") == 0)
    {
        return;
    }
    uint8_t new_entry = update_entity_entry(data_buffer[TARGET_FLAG],
                            data_buffer[TARGET_INSTANCE_FLAG], data_buffer[EVENT_FLAG]);
    if (new_entry == BTF_TRACE_FALSE)
    {
        return;
    }
    if (core_id == 1)
    {
        process_btf_trace_data(stream, core1_trace_data, &core1_stack_top, data_buffer);
    }
    else if (core_id == 0)
    {
        #if 0
        fprintf(stream,"Input ---%d,%d,%d,%d,%d,%d,%d\n", data_buffer[TIME_FLAG], data_buffer[SOURCE_FLAG],
                data_buffer[SOURCE_INSTANCE_FLAG], data_buffer[EVENT_TYPE_FLAG], data_buffer[TARGET_FLAG],
                data_buffer[TARGET_INSTANCE_FLAG], data_buffer[EVENT_FLAG]);
        fflush(stream);
        #endif
        process_btf_trace_data(stream, core0_trace_data, &core0_stack_top, data_buffer);
        #if 0
        int index = 0;
        if (core0_stack_top >= 0)
        {
            fprintf(stream, "stack size=%d\n", core0_stack_top);
            for(index = core0_stack_top; index >= 0; index--)
            {
                fprintf(stream,"Src=%d Target=%d Event=%d\n",core0_trace_data[index].srcId,
                        core0_trace_data[index].taskId, core0_trace_data[index].eventState);
                fflush(stream);
            }
        }
        #endif
    }
    else
    {
        // Do nothing
    }

}
