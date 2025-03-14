#include "../Driver_CAN/CanDefinitions.h"

const CanMessage canMessages_Rx[] = {
    {CAN_MSG_TMU_MP3_Rx, 8, "TMU_MP3_Rx"},
    {BCM_MSG_FuncSta, 8, "BCM_FuncSta_Rx"},
};

const CanSignal canSignals_Rx[] = {
        {"MP3 Command", 0, 4, false, false},
        {"Mp3 parameter", 4, 8, false, false},
};

// 글로벌 데이터
CanSignalFlags g_canSignalFlags = {0};
CanSignalData_Rx g_canSignalData_Rx = {0};
