/*
 * @Author: Vamper 3230104810@zju.edu.cn
 * @Date: 2025-01-11 19:38:29
 * @LastEditors: Vamper 3230104810@zju.edu.cn
 * @LastEditTime: 2025-01-17 15:48:33
 * @FilePath: \XiaoKuBao\Task\Src\main_task.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "main_task.hpp"
#include "iwdg.h"
#include "tim.h"
#include "can.h"
#include "ins_all.hpp"
//wait

 hw_motor::Motor* Move_motor_ptr = CreateMotorMove();
 hw_rc::DT7* rc_ptr = CreateRemoteControl();
 hw_pid::MultiNodesPid* pid_ptr = CreatePidMotorMove();
 hw_comm::CanRxMgr* can_rx_mgr_ptr = CreateCan1RxMgr();
 hw_comm::CanTxMgr* can_tx_mgr_ptr = CreateCan1TxMgr();
 hw_comm::UartRxMgr* uart_rx_mgr_ptr = CreateRcRxMgr();

float spd_ref_=0.0f;
float spd_fdb_=0.0f;    
float cur_ref_=0.0f;       
float cur_fdb_=0.0f; 
float reduce=2.4f;

uint32_t tick = 0;
uint32_t stuck_tick = 0;

bool dirction=true;

enum MoveMode_ {
  kMoveMode_Auto = 1u,
  kMoveMode_Normal,
  kMoveMode_Off,
};

uint8_t Move_mode = kMoveMode_Off;

void RefreshIwdg(void) { HAL_IWDG_Refresh(&hiwdg); }

void MainTaskInit(){
 can_rx_mgr_ptr->addReceiver(Move_motor_ptr);
 can_tx_mgr_ptr->addTransmitter(Move_motor_ptr);
 can_rx_mgr_ptr->filterInit();
 can_rx_mgr_ptr->startReceive();
 HAL_CAN_Start(&hcan1);
 rc_ptr->registerUpdateCallback(RefreshIwdg);

 uart_rx_mgr_ptr->addReceiver(rc_ptr);
 uart_rx_mgr_ptr->startReceive();
}

void RcSetMode() 
{
  if (rc_ptr->rc_l_switch() != hw_rc::SwitchState::kUp) {
    switch (rc_ptr->rc_l_switch()) {
      case hw_rc::SwitchState::kDown:
        Move_mode = kMoveMode_Auto;
        break;
      case hw_rc::SwitchState::kMid:
        Move_mode = kMoveMode_Normal;
        break;
      default:
        Move_mode = kMoveMode_Off;
        break;
    }
  } else {
    Move_mode = kMoveMode_Off;
  }
}

void DirUpdate() 
{
 if(cur_ref_>=19.0f||cur_ref_<=-19.0f){
   stuck_tick++;
 }else{
    stuck_tick=0;
 }

 if(stuck_tick>100){
   dirction=(!dirction);
   stuck_tick=0;
 }

}

void MotorUpdate() //BLY
{
  HW_ASSERT(Move_motor_ptr != nullptr, "ptr to feed motor is nullptr", Move_motor_ptr);
  if(Move_mode == kMoveMode_Off)
  {
    spd_ref_=0.0f;
  }

  else if(Move_mode == kMoveMode_Normal)
  {
    if(rc_ptr->rc_rh()<0.2f&&rc_ptr->rc_rh()>-0.2f){
      spd_ref_=0.0f;
  }else if(rc_ptr->rc_rh()>0.2f){
    spd_ref_=5.0f*reduce*((rc_ptr->rc_rh()-0.2f)/0.8f);
  }else if(rc_ptr->rc_rh()<-0.2f)
  {
    spd_ref_=5.0f*reduce*((rc_ptr->rc_rh()+0.2f)/0.8f);
  }
  }

  else if(Move_mode == kMoveMode_Auto)
  {
    if(dirction){
      spd_ref_=3.0f*reduce;
    }else {
      spd_ref_=-3.0f*reduce;
    }
  }

  spd_fdb_ = Move_motor_ptr->vel();
}

void calcMotorInput() 
{
  HW_ASSERT(Move_motor_ptr != nullptr, "ptr to Move motor is nullptr", Move_motor_ptr);
  HW_ASSERT(pid_ptr != nullptr, "ptr to Move normal pid is nullptr", pid_ptr);

 if (Move_mode == kMoveMode_Off) {
    cur_ref_ = 0.0f;
    spd_ref_ = 0.0f;
    float ref = spd_ref_;
    float fdb[1] = {spd_fdb_};
    pid_ptr->calc(&ref, fdb, nullptr, &cur_ref_);//BLY
  } else {
    float ref = spd_ref_;
    float fdb[1] = {spd_fdb_};
    pid_ptr->calc(&ref, fdb, nullptr, &cur_ref_);
  }
}

void setCommData()
{
  HW_ASSERT(Move_motor_ptr != nullptr, "pointer to Move Motor is nullptr", Move_motor_ptr);

  if ((!Move_motor_ptr ->isOffline())&& Move_mode != kMoveMode_Off) {
    Move_motor_ptr->setInput(cur_ref_);
  } else {
    Move_motor_ptr->setInput(cur_ref_);//BLY
  }
}

void SendMsg()
{
  can_tx_mgr_ptr->setTransmitterNeedToTransmit(Move_motor_ptr);
  can_tx_mgr_ptr->startTransmit();
}

void MainTask()
{
    RcSetMode();
    DirUpdate();
    MotorUpdate();
    calcMotorInput();
    setCommData();
    SendMsg();
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim)
{
  if (htim->Instance == TIM6) {
    tick++;
    MainTask();
  }
}

/* 接收回调函数 */
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef* hcan) { can_rx_mgr_ptr->rxFifoMsgPendingCallback(hcan); }

/* 发送回调函数 */
void HAL_CAN_TxMailbox0CompleteCallback(CAN_HandleTypeDef* hcan) { can_tx_mgr_ptr->txMailboxCompleteCallback(hcan); }

/* 错误回调函数 */
void HAL_CAN_ErrorCallback(CAN_HandleTypeDef* hcan)
{
  can_tx_mgr_ptr->errorCallback(hcan);
  can_rx_mgr_ptr->errorCallback(hcan);
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef* huart, uint16_t Size) { uart_rx_mgr_ptr->rxEventCallback(huart, Size); }
/* 错误回调 */
void HAL_UART_ErrorCallback(UART_HandleTypeDef* huart) { uart_rx_mgr_ptr->errorCallback(huart); }

//Wait to be moved to comm_task