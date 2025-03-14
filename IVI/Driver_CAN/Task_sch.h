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

/* Define macros for task intervals */
#define STM_MODULE         &MODULE_STM0
#define ISR_PRIORITY_STM   10
#define TICKS_10MS         (IfxStm_getTicksFromMilliseconds(STM_MODULE, 10))

/* Function prototypes */
void initCanTaskScheduler(void);
void stm0ISR(void);
void handle10msTask(void);
void handle30msTask(void);
void handle50msTask(void);
void handle100msTask(void);
void handle500msTask(void);

#endif /* TASK_SCH_H_ */
