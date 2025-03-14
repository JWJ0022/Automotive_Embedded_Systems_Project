                                                                                              /* Includes */
#include "Task_sch.h"

#include "Driver_CAN/CanHandler.h"
#include "GlobalVar.h"

/* Static variables for tick tracking */
static uint32 next10msTick = 0;
static uint32 next30msTick = 0;
static uint32 next50msTick = 0;
static uint32 next100msTick = 0;
static uint32 next500msTick = 0;

/* STM ISR to update task flags */
IFX_INTERRUPT(stm0ISR, 0, ISR_PRIORITY_STM);

void stm0ISR(void)
{
    uint32 currentTick = IfxStm_getLower(STM_MODULE);

    /* 10ms task */
    if (currentTick >= next10msTick)
    {
        next10msTick += TICKS_10MS;
        handle10msTask();
    }

    /* 30ms task */
    if (currentTick >= next30msTick)
    {
        next30msTick += TICKS_10MS * 3; // 30ms = 10ms * 3
        handle30msTask();
    }

    /* 50ms task */
    if (currentTick >= next50msTick)
    {
        next50msTick += TICKS_10MS * 5; // 50ms = 10ms * 5
        handle50msTask();
    }

    /* 100ms task */
    if (currentTick >= next100msTick)
    {
        next100msTick += TICKS_10MS * 10; // 100ms = 10ms * 10
        handle100msTask();
    }

    if (currentTick >= next500msTick)
       {
        next500msTick += TICKS_10MS * 50; // 500ms = 10ms * 50
           handle500msTask();
       }
    /* Update STM comparator for the next interrupt */
    IfxStm_updateCompare(STM_MODULE, IfxStm_Comparator_0, currentTick + TICKS_10MS);
}

/* Initialize the Task Scheduler */
void initCanTaskScheduler(void)
{
    /* STM configuration */
    IfxStm_CompareConfig stmConfig;
    IfxStm_initCompareConfig(&stmConfig);

    stmConfig.triggerPriority = ISR_PRIORITY_STM;
    stmConfig.typeOfService = IfxSrc_Tos_cpu0;
    stmConfig.ticks = IfxStm_getLower(STM_MODULE) + TICKS_10MS; // Initial comparison value

    IfxStm_initCompare(STM_MODULE, &stmConfig);

    /* Set initial tick values */
    uint32 currentTick = IfxStm_getLower(STM_MODULE);
    next10msTick = currentTick + TICKS_10MS;
    next30msTick = currentTick + (TICKS_10MS * 3);
    next50msTick = currentTick + (TICKS_10MS * 5);
    next100msTick = currentTick + (TICKS_10MS * 10);

    /* Enable interrupts */
    IfxCpu_enableInterrupts();
}

/* 10ms Task Handler */
void handle10msTask(void)
{
//    if (canTaskFlag == L_SET)
//       {
//           processCanTasks();  // CAN 작업 수행
//           canTaskFlag = L_CLEAR; // 플래그 클리어
//       }
}

/* 30ms Task Handler */
void handle30msTask(void)
{

}

/* 50ms Task Handler */
void handle50msTask(void)
{
    //IfxPort_togglePin(&MODULE_P10, 2);
}

/* 100ms Task Handler */
void handle100msTask(void)
{

}

void handle500msTask(void)
{

}
