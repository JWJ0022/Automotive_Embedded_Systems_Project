/* Includes */
#include "Task_sch.h"

/* Static variables for tick tracking */
static uint32 next100msTick = 0;

/* STM ISR to update task flags */
IFX_INTERRUPT(stm0ISR, 0, ISR_PRIORITY_STM);

void stm0ISR(void)
{IfxPort_togglePin(&MODULE_P10, 2);
    uint32 currentTick = IfxStm_getLower(STM_MODULE);

    if (currentTick >= next100msTick)
    {
        next100msTick += TICKS_100MS;
        handleCanMessage();
    }

    /* Update STM comparator for the next interrupt */
    IfxStm_updateCompare(STM_MODULE, IfxStm_Comparator_0, currentTick + TICKS_100MS);
}

/* Initialize the CAN Task Scheduler */
void initCanTaskScheduler(void)
{
    /* STM configuration */
    IfxStm_CompareConfig stmConfig;
    IfxStm_initCompareConfig(&stmConfig);

    stmConfig.triggerPriority = ISR_PRIORITY_STM;
    stmConfig.typeOfService = IfxSrc_Tos_cpu0;
    stmConfig.ticks = IfxStm_getLower(STM_MODULE) + TICKS_100MS; // Initial comparison value

    IfxStm_initCompare(STM_MODULE, &stmConfig);

    /* Set initial tick value */
    uint32 currentTick = IfxStm_getLower(STM_MODULE);
    next100msTick = currentTick + TICKS_100MS;

    /* Enable interrupts */
    IfxCpu_enableInterrupts();
}

/* Handle CAN message transmission */
void handleCanMessage(void)
{
    transmitCanMessage(0x100);
    transmitCanMessage(0x300);
    transmitCanMessage(0x301);
    transmitCanMessage(0x302);
    transmitCanMessage(0x303);
}
