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

#include "model_enumerations.h"

#ifdef RFTP_GENERATE_BTF_TRACE
//static uint32_t entity_counter_id;

#endif /* End of RFTP_GENERATE_BTF_TRACE */


char SHM_sec1_enum [SHM_section1_label_count][label_str_len] =
{
  "sh_label_a",
  "sh_label_b",
  "sh_label_c",
  "sh_label_d",
  "sh_label_e",
  "sh_label_f",
  "sh_label_g",
  "sh_label_h",
  "sh_label_i",
  "sh_label_j"
};

char DSHM_sec1_enum [DSHM_section1_label_count][label_str_len] =
{
  "DSH_labelA",
  "DSH_labelB",
  "DSH_labelC",
  "DSH_labelD",
  "DSH_labelE",
  "DSH_labelF",
  "DSH_labelG",
  "DSH_labelH",
  "DSH_labelI",
  "DSH_labelJ"
};

char task_enum [task_count + 1][label_str_len] =
{
  "[idle]",
  "Task5ms0",
  "Task10ms0",
  "Task20ms0",
  "Task10ms1",
  "Task20ms1"
};

//#ifdef RFTP_GENERATE_BTF_TRACE
const uint8_t runnable_enum[][runnable_str_len] =
{
    "cIn5ms",
    "handler5ms",
    "cOut5ms",
    "cIn10ms",
    "handler10ms",
    "cOut10ms",
    "cIn20ms",
    "handler20ms",
    "cOut20ms",
    "cIn10msCore2",
    "handler10msCore2",
    "cOut10msCore2",
    "cIn20msCore2",
    "handler20msCore2",
    "cOut20msCore2"
};

const uint8_t hw_enum[][hw_str_len] = {
    "Core_0",
    "Core_1"
};

//#endif /* End of RFTP_GENERATE_BTF_TRACE */

unsigned DSHM_visible_labels [DSHM_visible_label_count] = {0,1};
unsigned SHM_visible_labels [SHM_visible_label_count] = {0,1};



void get_SHM_label_name (int index,char str[]){
    for (int i=0;i<label_str_len;i++){
        str[i] = SHM_sec1_enum[index][i];
    }
}

void get_DSHM_label_name(int index,char str[]){
    for (int i=0;i<label_str_len;i++){
        str[i] = DSHM_sec1_enum[index][i];
    }
}

void get_task_name(int index,char *str){
    for (int i=0;i<label_str_len;i++){
        str[i] = task_enum[index][i];
    }
}

void get_visible_label_index(unsigned array[],unsigned mem_type){
    if (mem_type==0){
        for (int i=0; i<SHM_visible_label_count;i++){
            array[i] = SHM_visible_labels[i];
        }
    }
    for (int i=0; i<DSHM_visible_label_count;i++){
        array[i] = DSHM_visible_labels[i];
    }
}

#ifdef RFTP_GENERATE_BTF_TRACE

void generate_task_entity_table(void)
{
    store_entity_entry(0, TASK_EVENT, (uint8_t *)task_enum[0]);
    store_entity_entry(1, TASK_EVENT, (uint8_t *)task_enum[1]);
    store_entity_entry(2, TASK_EVENT, (uint8_t *)task_enum[2]);
    store_entity_entry(3, TASK_EVENT, (uint8_t *)task_enum[3]);
    store_entity_entry(4, TASK_EVENT, (uint8_t *)task_enum[4]);
    store_entity_entry(5, TASK_EVENT, (uint8_t *)task_enum[5]);
}

void generate_runnable_entity_table(void)
{
    store_entity_entry(6, RUNNABLE_EVENT, (uint8_t *)runnable_enum[0]);
    store_entity_entry(7, RUNNABLE_EVENT, (uint8_t *)runnable_enum[1]);
    store_entity_entry(8, RUNNABLE_EVENT, (uint8_t *)runnable_enum[2]);
    store_entity_entry(9, RUNNABLE_EVENT, (uint8_t *)runnable_enum[3]);
    store_entity_entry(10, RUNNABLE_EVENT, (uint8_t *)runnable_enum[4]);
    store_entity_entry(11, RUNNABLE_EVENT, (uint8_t *)runnable_enum[5]);
    store_entity_entry(12, RUNNABLE_EVENT, (uint8_t *)runnable_enum[6]);
    store_entity_entry(13, RUNNABLE_EVENT, (uint8_t *)runnable_enum[7]);
    store_entity_entry(14, RUNNABLE_EVENT, (uint8_t *)runnable_enum[8]);
    store_entity_entry(15, RUNNABLE_EVENT, (uint8_t *)runnable_enum[9]);
    store_entity_entry(16, RUNNABLE_EVENT, (uint8_t *)runnable_enum[10]);
    store_entity_entry(17, RUNNABLE_EVENT, (uint8_t *)runnable_enum[11]);
    store_entity_entry(18, RUNNABLE_EVENT, (uint8_t *)runnable_enum[12]);
    store_entity_entry(19, RUNNABLE_EVENT, (uint8_t *)runnable_enum[13]);
    store_entity_entry(20, RUNNABLE_EVENT, (uint8_t *)runnable_enum[14]);
}

void generate_signal_entity_table(void)
{
    store_entity_entry(21, SIGNAL_EVENT, (uint8_t *)SHM_sec1_enum[0]);
    store_entity_entry(22, SIGNAL_EVENT, (uint8_t *)SHM_sec1_enum[1]);
    store_entity_entry(23, SIGNAL_EVENT, (uint8_t *)DSHM_sec1_enum[0]);
    store_entity_entry(24, SIGNAL_EVENT, (uint8_t *)DSHM_sec1_enum[1]);
}

void generate_hw_entity_table(void)
{
    store_entity_entry(25, CORE_EVENT, (uint8_t *)hw_enum[0]);
    store_entity_entry(26, CORE_EVENT, (uint8_t *)hw_enum[1]);
}

#endif /* End of RFTP_GENERATE_BTF_TRACE */




