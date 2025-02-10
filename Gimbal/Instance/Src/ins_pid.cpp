/*
 * @Author: Vamper 3230104810@zju.edu.cn
 * @Date: 2025-01-12 18:36:58
 * @LastEditors: Vamper 3230104810@zju.edu.cn
 * @LastEditTime: 2025-01-15 16:26:07
 * @FilePath: \XiaoKuBao\Instance\Src\ins_pid.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "ins_pid.hpp"
#include "motor.hpp"

const float kMaxPidOutRoll = 20.0f;  ///< 3508电流控制的最大输出
const hw_pid::OutLimit kOutLimitRoll = hw_pid::OutLimit(true, -kMaxPidOutRoll, kMaxPidOutRoll);

const hw_pid::MultiNodesPid::ParamsList kPidParamsRoll = {
    {
     .auto_reset = true,
     .kp = 5.0f,
     .ki = 0.0f,
     .kd = 0.0f,
     .setpoint_ramping = hw_pid::SetpointRamping(false, -0.1, 0.1, 0.1),
     .inte_anti_windup = hw_pid::InteAntiWindup(true, -1.0f, 1.0f),
     .diff_previous = hw_pid::DiffPrevious(false, 0.01f),
     .out_limit = kOutLimitRoll,
     },
};

const hw_pid::MultiNodesPid::Type kPidTypeCascade = hw_pid::MultiNodesPid::Type::kCascade;

hw_pid::MultiNodesPid unique_pid_wheel_Roll(kPidTypeCascade, kOutLimitRoll, kPidParamsRoll);

hw_pid::MultiNodesPid* CreatePidMotorRoll() { return &unique_pid_wheel_Roll; };