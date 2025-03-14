#ifndef TASK_SCH_H_
#define TASK_SCH_H_

#include <stdint.h>
#include "IfxStm.h"
#include "IfxCpu.h"
#include "IfxScuWdt.h"
#include "IfxPort_regdef.h"
#include "Ifx_Types.h"
#include "IfxAsclin_Asc.h"
#include "CanHandler.h"

/* Define macros for flag states */
#define L_SET   1U
#define L_CLEAR 0U

/* STM settings */
#define STM_MODULE         &MODULE_STM0
#define ISR_PRIORITY_STM   10
#define TICKS_100MS        (IfxStm_getTicksFromMilliseconds(STM_MODULE, 100))

void initCanTaskScheduler(void);
void stm0ISR(void);
void handleCanMessage(void);

#endif
