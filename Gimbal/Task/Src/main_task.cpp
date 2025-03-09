/*
 * @Author: Vamper 3230104810@zju.edu.cn
 * @Date: 2025-01-11 19:38:29
 * @LastEditors: Vamper 3230104810@zju.edu.cn
 * @LastEditTime: 2025-01-18 01:27:02
 * @FilePath: \Gimbal\Task\Src\main_task.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置
 * 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */

#include "main_task.hpp"
#include "can.h"
#include "ins_all.hpp"
#include "iwdg.h"
#include "tim.h"
// TODO tree need to be added

hw_motor::Motor *Roll_motor_ptr = CreateMotorRoll();
hw_rc::DT7 *rc_ptr = CreateRemoteControl();
hw_pid::MultiNodesPid *pid_ptr = CreatePidMotorRoll();
hw_comm::CanRxMgr *can_rx_mgr_ptr = CreateCan1RxMgr();
hw_comm::CanTxMgr *can_tx_mgr_ptr = CreateCan1TxMgr();
hw_comm::UartRxMgr *uart_rx_mgr_ptr = CreateRcRxMgr();

float spd_ref_ = 0.0f;
float spd_fdb_ = 0.0f;
float cur_ref_ = 0.0f;
float cur_fdb_ = 0.0f;
float reduce = 2.4f;
int roll_dir = 0;

uint32_t tick = 0;
uint32_t rolling_tick = 0;

enum RollMode_ {
  kRollMode_Fast = 1u,
  kRollMode_Mid,
  kRollMode_Slow,
  kRollMode_Off,
};

uint8_t roll_mode = kRollMode_Off;

void RefreshIwdg(void) { HAL_IWDG_Refresh(&hiwdg); }

void MainTaskInit() {
  can_rx_mgr_ptr->addReceiver(Roll_motor_ptr);
  can_tx_mgr_ptr->addTransmitter(Roll_motor_ptr);
  can_rx_mgr_ptr->filterInit();
  can_rx_mgr_ptr->startReceive();
  HAL_CAN_Start(&hcan1);
  rc_ptr->registerUpdateCallback(RefreshIwdg);

  uart_rx_mgr_ptr->addReceiver(rc_ptr);
  uart_rx_mgr_ptr->startReceive();
}

void RcSetMode() {
  if ((rc_ptr->rc_l_switch() == hw_rc::SwitchState::kDown) ||
      (rc_ptr->rc_l_switch() == hw_rc::SwitchState::kMid)) {
    switch (rc_ptr->rc_r_switch()) {
    case hw_rc::SwitchState::kUp:
      roll_mode = kRollMode_Fast;
      break;
    case hw_rc::SwitchState::kMid:
      roll_mode = kRollMode_Mid;
      break;
    case hw_rc::SwitchState::kDown:
      roll_mode = kRollMode_Slow;
      break;
    default:
      roll_mode = kRollMode_Off;
      break;
    }
  } else {
    roll_mode = kRollMode_Off;
  }
  if (rc_ptr->rc_wheel() > 0) {
    rolling_tick = 100;
    roll_dir = 1;
  } else if (rc_ptr->rc_wheel() < 0) {
    rolling_tick = 100;
    roll_dir = -1;
  }
}

void MotorUpdate() {
  HW_ASSERT(Roll_motor_ptr != nullptr, "ptr to feed motor is nullptr",
            Roll_motor_ptr);
  switch (roll_mode) {
  case kRollMode_Fast:
    spd_ref_ = 6.0f * reduce;
    break;
  case kRollMode_Mid:
    spd_ref_ = 3.1415926f * reduce;
    break;
  case kRollMode_Slow:
    spd_ref_ = 3.1415926f * reduce;
    break;
  default:
    spd_ref_ = 0.0f;
    break;
  }
  // spd_fdb_=10.0f;
  spd_fdb_ = Roll_motor_ptr->vel();
}

void calcMotorInput() {
  HW_ASSERT(Roll_motor_ptr != nullptr, "ptr to roll motor is nullptr",
            Roll_motor_ptr);
  HW_ASSERT(pid_ptr != nullptr, "ptr to roll normal pid is nullptr", pid_ptr);

  if (roll_mode == kRollMode_Off) {
    cur_ref_ = 0.0f;
    spd_ref_ = 0.0f;
    float ref = spd_ref_;
    float fdb[1] = {spd_fdb_}; // BLY
    if (spd_fdb_ > 3.0f * reduce) {
      pid_ptr->calc(&ref, fdb, nullptr, &cur_ref_);
    } else {
      cur_ref_ = 0.0f;
    }
  } else if (roll_mode == kRollMode_Slow) {
    if (rolling_tick > 0) {
      float ref = roll_dir * spd_ref_;
      float fdb[1] = {spd_fdb_};
      pid_ptr->calc(&ref, fdb, nullptr, &cur_ref_);
      rolling_tick--;
    } else {
      spd_ref_ = 0.0f;
      float ref = spd_ref_;
      float fdb[1] = {spd_fdb_};
      pid_ptr->calc(&ref, fdb, nullptr, &cur_ref_);
    }
  } else {
    float ref = spd_ref_;
    float fdb[1] = {spd_fdb_};
    pid_ptr->calc(&ref, fdb, nullptr, &cur_ref_);
  }
}

void setCommData() {
  HW_ASSERT(Roll_motor_ptr != nullptr, "pointer to roll Motor is nullptr",
            Roll_motor_ptr);

  if ((!Roll_motor_ptr->isOffline()) && roll_mode != kRollMode_Off) {
    Roll_motor_ptr->setInput(cur_ref_);
  } else {
    Roll_motor_ptr->setInput(cur_ref_); // BLY
  }
}

void SendMsg() {
  can_tx_mgr_ptr->setTransmitterNeedToTransmit(Roll_motor_ptr);
  can_tx_mgr_ptr->startTransmit();
}

void MainTask() {
  RcSetMode();
  MotorUpdate();
  calcMotorInput();
  setCommData();
  SendMsg();
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
  if (htim->Instance == TIM6) {
    tick++;
    MainTask();
  }
}

/* 接收回调函数 */
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) {
  can_rx_mgr_ptr->rxFifoMsgPendingCallback(hcan);
}

/* 发送回调函数 */
void HAL_CAN_TxMailbox0CompleteCallback(CAN_HandleTypeDef *hcan) {
  can_tx_mgr_ptr->txMailboxCompleteCallback(hcan);
}

/* 错误回调函数 */
void HAL_CAN_ErrorCallback(CAN_HandleTypeDef *hcan) {
  can_tx_mgr_ptr->errorCallback(hcan);
  can_rx_mgr_ptr->errorCallback(hcan);
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size) {
  uart_rx_mgr_ptr->rxEventCallback(huart, Size);
}
/* 错误回调 */
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart) {
  uart_rx_mgr_ptr->errorCallback(huart);
}

// Wait to be moved to comm_task