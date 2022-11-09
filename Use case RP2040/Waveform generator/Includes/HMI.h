#pragma once

#ifndef HMI_HMI_H
#define HMI_HMI_H

#include<stdint.h>
#include <stdio.h>
#include "hardware/pll.h"
#include "hardware/clocks.h"
#include "hardware/structs/pll.h"
#include "hardware/structs/clocks.h"
#include "RotaryEncoder.h"
#include "WaveformGenerator_CH.h"

#define SYS_CLK (float)clock_get_hz(clk_sys)

using namespace pico_WG_CH;
using namespace pico_RotaryEncoder;

namespace pico_HMI{
    class HMI
    {
        private:
            /// \brief Sends single 8bit command to ssd1306 controller
            /// \param command - byte to be sent to controller
            void cmd(uint cmd);
            uint32_t State;
            uint command;

        public:
            HMI(uint32_t State, uint cmd);
            void setcmd(uint cmd);
    };
}

#endif //HMI_HMI_H
