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

#include "ParallellaUtils.h"
#include "debugFlags.h"
#include "RTFParallellaConfig.h"


void sleepTimerMs(int ticks, int taskNum){
    updateDebugFlag(ticks * 10);
    int clock_cycle = (configCPU_CLOCK_HZ / execution_time_scale);
    int tick_count;
    int ts = get_time_scale_factor();
    int no_of_ticks = ts * ticks;
    int is_signal_sent = 0;
    for (tick_count = 0;tick_count < no_of_ticks; tick_count++)
    {
        if(is_signal_sent == 0){
            signalHost();
            is_signal_sent = 1;
        }
        taskENTER_CRITICAL();
        e_wait(E_CTIMER_0,clock_cycle);
        taskEXIT_CRITICAL();
    }
}

