/*
 * @Author: Vamper 3230104810@zju.edu.cn
 * @Date: 2025-01-12 18:36:50
 * @LastEditors: Vamper 3230104810@zju.edu.cn
 * @LastEditTime: 2025-01-12 18:37:57
 * @FilePath: \XiaoKuBao\Instance\Inc\ins_pid.hpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef INSTANCE_INS_PID_HPP_
#define INSTANCE_INS_PID_HPP_

#include "pid.hpp"

namespace hw_pid = hello_world::pid;

hw_pid::MultiNodesPid* CreatePidMotorMove();

#endif // INSTANCE_INS_PID_HPP_