/*
 * @Author: Vamper 3230104810@zju.edu.cn
 * @Date: 2025-01-11 20:37:39
 * @LastEditors: Vamper 3230104810@zju.edu.cn
 * @LastEditTime: 2025-01-14 15:27:31
 * @FilePath: \XiaoKuBao\Instance\Src\ins_comm.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
/** 
 *******************************************************************************
 * @file      : ins_comm.cpp
 * @brief     : 
 * @history   :
 *  Version     Date            Author          Note
 *  V0.9.0      yyyy-mm-dd      <author>        1. <note>
 *******************************************************************************
 * @attention :
 *******************************************************************************
 *  Copyright (c) 2024 Hello World Team, Zhejiang University.
 *  All Rights Reserved.
 *******************************************************************************
 */
/* Includes ------------------------------------------------------------------*/

#include "ins_comm.hpp"
#include "DT7.hpp"
#include "can.h"
#include "usart.h"

/* Private macro -------------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/

typedef hello_world::comm::CanRxMgr CanRxMgr;
typedef hello_world::comm::CanTxMgr CanTxMgr;
typedef hello_world::comm::UartRxMgr UartRxMgr;

/* Private constants ---------------------------------------------------------*/

const size_t kRxRcBufferSize = hello_world::remote_control::DT7::kRcRxDataLen_ + 1;
const size_t kRxRfrBufferSize = 64;

/* Private variables ---------------------------------------------------------*/

static bool is_can_1_rx_mgr_inited = false;
static CanRxMgr can_1_rx_mgr = CanRxMgr();

static bool is_can1_tx_mgr_inited = false;
static CanTxMgr can_1_tx_mgr = CanTxMgr();

static bool is_rc_rx_mgr_inited = false;
static UartRxMgr rc_rx_mgr = UartRxMgr();

/* External variables --------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Exported function definitions ---------------------------------------------*/

CanRxMgr* CreateCan1RxMgr(void)
{
  if (!is_can_1_rx_mgr_inited) {
    can_1_rx_mgr.init(&hcan1, CanRxMgr::RxType::kFifo0);
    can_1_rx_mgr.clearReceiver();
    is_can_1_rx_mgr_inited = true;
  }
  return &can_1_rx_mgr;
};

CanTxMgr* CreateCan1TxMgr(void)
{
  if (!is_can1_tx_mgr_inited) {
    can_1_tx_mgr.init(&hcan1);
    can_1_tx_mgr.clearTransmitter();
    is_can1_tx_mgr_inited = true;
  }
  return &can_1_tx_mgr;
};


UartRxMgr* CreateRcRxMgr(void)
{
  if (!is_rc_rx_mgr_inited) {
    rc_rx_mgr.init(&huart3, UartRxMgr::EofType::kIdle, kRxRcBufferSize, kRxRcBufferSize - 1);
    rc_rx_mgr.clearReceiver();
    is_rc_rx_mgr_inited = true;
  }
  return &rc_rx_mgr;
};

/* Private function definitions ----------------------------------------------*/
