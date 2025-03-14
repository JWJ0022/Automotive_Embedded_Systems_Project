#include "CanDefinitions.h"

// CAN 메시지 정의 배열
const CanMessage canMessages_Tx[] = { // 메시지 ID, 길이(DLC), 메세지 이름
    {CAN_TMU_IGNsta_Tx, 1, "TMU_IGNsta"},
    {CAN_TMU_SeatCmd_Tx, 1, "TMU_SeatCmd"},
    {CAN_TMU_SHVCmd_Tx, 1, "TMU_SHVCmd"},
    {CAN_TMU_WindowCmd_Tx, 1, "TMU_WindowCmd"},
    {CAN_TMU_MP3_Tx, 1, "TMU_MP3"}

};

const CanMessage canMessages_Rx[] = {
    {CAN_MSG_ENGINE_Rx, 8, "EngineData_Rx"},
    {CAN_MSG_BRAKE_Rx, 8, "BrakeData_Rx"},
    {CAN_MSG_VEHICLE_Rx, 8, "VehicleData_Rx"}
};

// CAN 신호 정의 배열
const CanSignal canSignals_Tx[] = { //신호 이름, 시작 비트, 길이, Big Endian 여부, Signed 여부
    {"IGN1", 0, 1, false, false},

    {"Seat_RelaxMode", 0, 1, false, false},
    {"Move_FrontSeat", 1, 2, false, false},
    {"Fold_FrontSeat", 3, 2, false, false},
    {"Fold_BackSeat", 5, 2, false, false},

    {"Cush_Heat", 0, 2, false, false},
    {"Back_Heat", 2, 2, false, false},
    {"SHV_VentCmd", 4, 2, false, false},

    {"WindowUp", 0, 1, false, false},
    {"WindowDown", 1, 1, false, false},

    {"MP3_Cmd", 0, 4, false, false},
    {"MP3_Parameter", 4, 8, false, false},
};

const CanSignal canSignals_Rx[] = {
    {"Engine_sig1", 0, 16, false, false},
    {"Engine_sig2", 16, 8, false, false},
    {"Brake_sig1", 0, 12, false, false},
    {"Brake_sig2", 12, 8, false, false},
    {"Vehicle_sig1", 0, 16, false, false},
    {"Vehicle_sig2", 16, 8, false, false}
};

// 글로벌 데이터
CanSignalData_Tx g_canSignalData_Tx = {0};
CanSignalData_Rx g_canSignalData_Rx = {0};
