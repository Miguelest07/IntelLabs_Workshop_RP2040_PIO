#include "HMI.h"

namespace pico_HMI{

    HMI::HMI(uint32_t State, uint cmd)
    {
        this->command = cmd;
        this->State = State;
    }

    void HMI::setcmd(uint cmd)
    {
        this->cmd(cmd);
    }

    void HMI::cmd(uint cmd)
    {
        this->command = cmd;
    }
}