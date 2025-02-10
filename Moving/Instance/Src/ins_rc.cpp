/*
 * @Author: Vamper 3230104810@zju.edu.cn
 * @Date: 2025-01-12 20:02:43
 * @LastEditors: Vamper 3230104810@zju.edu.cn
 * @LastEditTime: 2025-01-12 20:05:37
 * @FilePath: \XiaoKuBao\Instance\Src\ins_rc.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "ins_rc.hpp"

hw_rc::DT7 unique_rc = hw_rc::DT7();

hw_rc::DT7* CreateRemoteControl() { return &unique_rc; };

