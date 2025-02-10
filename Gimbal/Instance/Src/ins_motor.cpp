/*
 * @Author: Vamper 3230104810@zju.edu.cn
 * @Date: 2025-01-12 16:01:01
 * @LastEditors: Vamper 3230104810@zju.edu.cn
 * @LastEditTime: 2025-01-12 18:17:37
 * @FilePath: \XiaoKuBao\Instance\Src\ins_motor.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "ins_motor.hpp"

const hw_motor::OptionalParams kMotorParamsRoll = {//wait to be modified BLY
  .input_type = hw_motor::InputType::kCurr,
  .angle_range = hw_motor::AngleRange::kNegInfToPosInf,
  .dir = hw_motor::kDirFwd,
  .remove_build_in_reducer = false,
  .angle_offset = 0,
  .ex_redu_rat = 1,
  .max_raw_input_lim = std::numeric_limits<float>::max(),
  .max_torq_input_lim = std::numeric_limits<float>::max(),
  .max_curr_input_lim = std::numeric_limits<float>::max(),
};

hw_motor::M3508 unique_motor_Roll = hw_motor::M3508(1u, kMotorParamsRoll);
hw_motor::Motor* CreateMotorRoll() { return &unique_motor_Roll; };