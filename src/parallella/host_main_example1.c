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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <e-hal.h> //hardware abstraction library
#include <time.h>   /* Needed for struct timespec */

#include "c2c.h"
#include "debugFlags.h"
#include "shared_comms.h"
#include "host_utils.h"
#include "model_enumerations.h"

#ifdef ENABLE_SHARED_LABEL

unsigned int shared_label_to_read[10];
unsigned int shared_label_core_00[dstr_mem_sec_1_label_count];
unsigned int shared_label_core_10[dstr_mem_sec_1_label_count];
unsigned int shared_label_core[core_count][dstr_mem_sec_1_label_count];
#endif

//#ifdef RFTP_GENERATE_BTF_TRACE

static void construct_btf_trace_header(FILE *stream)
{
    write_btf_trace_header_config(stream);
    write_btf_trace_header_entity_type(stream, TASK_EVENT);
    write_btf_trace_header_entity_type(stream, RUNNABLE_EVENT);
    write_btf_trace_header_entity_type(stream, SIGNAL_EVENT);
    generate_task_entity_table();
    generate_runnable_entity_table();
    generate_signal_entity_table();
    generate_hw_entity_table();
    write_btf_trace_header_entity_table(stream);
    write_btf_trace_header_entity_type_table(stream);
}

//#endif

int main(int argc, char *argv[])
{
#ifdef ENABLE_SHARED_LABEL
    int label_enable_count_core0 = 0;
    unsigned labelVisual_perCore[core_count][DSHM_visible_label_count];
    unsigned int prv_val_preCore[core_count][DSHM_visible_label_count];
#endif

#ifdef RFTP_GENERATE_BTF_TRACE
    FILE *fp_to_trace = NULL;
    parse_btf_trace_arguments(argc, argv);
    uint8_t trace_file_path[512] = {0};
    get_btf_trace_file_path(trace_file_path);
    if (strlen(trace_file_path) != 0)
    {
        //TODO: Add function to create file pointer for given file path.
    }
    else
    {
    	fprintf(stderr,"Output redirected to stderr\n");
        fp_to_trace = stderr;
    }
    construct_btf_trace_header(fp_to_trace);
#endif/* End of RFTP_GENERATE_BTF_TRACE*/

#ifdef ENABLE_SHARED_LABEL
    for (int i=0;i<core_count;i++){
        get_visible_label_index(labelVisual_perCore[i],MEM_TYPE_DSHM);
    }
    unsigned labelVisual_SHM[SHM_visible_label_count];
    unsigned int prv_val_SHM[SHM_visible_label_count];
    get_visible_label_index(labelVisual_SHM,MEM_TYPE_SHM);
    for (int i=0;i<core_count;i++){
        printf ("@%d : ",i);
        for (int j=0;j<DSHM_visible_label_count;j++){
            printf("L%d ",labelVisual_perCore[i][j]);
        }
        printf("\n");
    }
    fprintf(stderr,"===========================================================================\n");
    fprintf(stderr,"      |   Tasks being executed  | observed labels values |\n");
    fprintf(stderr,"%6s|%12s|%12s|"," tick ","   Core 1   ","   Core 2   ");
    for (int i=0;i<core_count;i++){
        print_legend_enum(DSHM_visible_label_count,labelVisual_perCore[i],MEM_TYPE_DSHM);
    }
    print_legend_enum(SHM_visible_label_count,labelVisual_SHM,MEM_TYPE_SHM);
    fprintf(stderr,"\n");
    fprintf(stderr,"===========================================================================\n");
#endif
    //counters for row and column, cored id and loop counter
    unsigned   row_loop,col_loop;
    // this will contain the epiphany platform configuration
    e_platform_t epiphany;
    e_epiphany_t dev;
    e_return_stat_t result;
#ifdef ENABLE_SHARED_LABEL
    unsigned int message[9];
    unsigned int message2[9];
#endif
//#ifdef RFTP_GENERATE_BTF_TRACE
    unsigned int core0_btf_trace[BTF_TRACE_BUFFER_SIZE];
    unsigned int core1_btf_trace[BTF_TRACE_BUFFER_SIZE];
//#endif
    int loop;
    int addr;
//#ifdef RFTP_GENERATE_BTF_TRACE
    int btf_trace_addr;
//#endif
    e_mem_t emem;
    e_init(NULL);
    /*
     * reserve shared mem sections
     * one section for each shared label in the model
     * shared mem address space starts at 0x0100_0000
     * and ends at 0x3FFF_FFFF
     *
     */
#ifdef ENABLE_SHARED_LABEL
    e_alloc(&emem, shared_mem_section , sizeof(shared_label_to_read));
#endif
    e_reset_system(); // reset the epiphnay chip
    e_get_platform_info(&epiphany);//gets the configuration info for the parallella platofrm
    //debug flag
    //------------------------------------------------
    //one core within the parallella work group is 1 x 1 i.e single core
    e_open(&dev,0,0,2,1); //2 rows one column rectangle size 2*1
    //reset the group
    e_reset_group(&dev);
    e_return_stat_t result1;
    e_return_stat_t result2;
    //load the group
    result1 =  e_load("core0_main.elf",&dev,0,0,E_FALSE);
    result2 =  e_load("core1_main.elf",&dev,1,0,E_FALSE);
    if (result1 != E_OK || result2 != E_OK){
        fprintf(stderr,"Error Loading the Epiphany Application 1 %i\n", result);
    }
    e_start_group(&dev);
    addr = cnt_address;
#ifdef RFTP_GENERATE_BTF_TRACE
    btf_trace_addr = btf_trace_address;
#endif
    int pollLoopCounter = 0;
#ifdef ENABLE_SHARED_LABEL
    unsigned int chainLatencyEndIndicator = 0;
    unsigned int chainLatencyStartIndicator = 10e6;
    unsigned int lat1 = 0;
    char buffer1[label_str_len];
    array_init(buffer1,label_str_len);
    char buffer2[label_str_len];
    array_init(buffer2,label_str_len);
#endif


    //int prev1,prev2,prev3;
    for (pollLoopCounter=0;pollLoopCounter<=40;pollLoopCounter++){
#ifdef ENABLE_SHARED_LABEL
        message[3] = 0;
        e_read(&dev,0,0,addr, &message, sizeof(message));
#endif
#ifdef RFTP_GENERATE_BTF_TRACE
        e_read(&dev,0,0,btf_trace_addr, &core0_btf_trace, sizeof(core0_btf_trace));
#endif
#ifdef ENABLE_SHARED_LABEL
        e_read(&dev,0,0,dstr_mem_offset_sec_1, &shared_label_core[0], sizeof(shared_label_core_00));
        e_read(&dev,1,0,addr, &message2, sizeof(message2));
#endif
#ifdef RFTP_GENERATE_BTF_TRACE
        e_read(&dev,1,0,btf_trace_addr, &core1_btf_trace, sizeof(core1_btf_trace));
#endif
#ifdef ENABLE_SHARED_LABEL
        e_read(&dev,1,0,dstr_mem_offset_sec_1, &shared_label_core[1], sizeof(shared_label_core_10));
        e_read(&emem,0,0,0x00, &shared_label_to_read, sizeof(unsigned int));

        if (message[8]!= message2[8] ){
            //fprintf(stderr,"NIS->");
        }
        get_task_name(message[6],buffer1);
        get_task_name(message2[6],buffer2);
        fprintf(stderr," %4d | %10s | %10s | ",message[8]+1,buffer1,buffer2);
        for (int i=0;i<core_count;i++){
            user_config_print_values_auto(DSHM_visible_label_count,labelVisual_perCore[i],shared_label_core[i],prv_val_preCore[i]);
        }
        for (int i=0;i<10;i++){
            fprintf(stderr," %3d |",shared_label_to_read[i]);
        }

        //user_config_print_values_auto(SHM_visible_label_count,labelVisual_SHM,shared_label_to_read,prv_val_SHM);
        fprintf(stderr,"\n");
#endif
        fprintf(stderr, "BTF trace Core 0---[Ticks=%d][SourceId=%d][SourceIns=%d][Type=%d] \
        		[Target=%d][TargetIns=%d][Event=%d][Data=%d]\n", core0_btf_trace[0],
        		core0_btf_trace[1], core0_btf_trace[2], core0_btf_trace[3], core0_btf_trace[4],
				core0_btf_trace[5], core0_btf_trace[6], core0_btf_trace[7]);
        fprintf(stderr, "BTF trace Core 1---[Ticks=%d][SourceId=%d][SourceIns=%d][Type=%d] \
        		[Target=%d][TargetIns=%d][Event=%d][Data=%d]\n", core1_btf_trace[0],
        		core1_btf_trace[1], core1_btf_trace[2], core1_btf_trace[3], core1_btf_trace[4],
				core1_btf_trace[5], core1_btf_trace[6], core1_btf_trace[7]);
        nsleep(1);
    }
    fprintf(stderr,"----------------------------------------------\n");
    e_close(&dev);
    e_finalize();
    //----------------------------------------------------------------------------
    //----------------------------------------------------------------------------
    fprintf(stderr,"RFTP demo complete \n ");
    return 0;
}

