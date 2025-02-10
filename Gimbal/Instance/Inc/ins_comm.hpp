/*
 * @Author: Vamper 3230104810@zju.edu.cn
 * @Date: 2025-01-11 20:37:55
 * @LastEditors: Vamper 3230104810@zju.edu.cn
 * @LastEditTime: 2025-01-12 19:54:00
 * @FilePath: \XiaoKuBao\Instance\Inc\ins_comm.hpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef INSTANCE_INS_COMM_HPP_
#define INSTANCE_INS_COMM_HPP_

/* Includes ------------------------------------------------------------------*/

#include "can_rx_mgr.hpp"
#include "can_tx_mgr.hpp"
#include "uart_rx_mgr.hpp"
#include "uart_tx_mgr.hpp"

/* Exported macro ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported function prototypes ----------------------------------------------*/
namespace hw_comm = hello_world::comm;
hello_world::comm::CanRxMgr* CreateCan1RxMgr(void);
hello_world::comm::CanTxMgr* CreateCan1TxMgr(void);
hello_world::comm::UartRxMgr* CreateRcRxMgr(void);

#endif /* INSTANCE_INS_COMM_HPP_ */
