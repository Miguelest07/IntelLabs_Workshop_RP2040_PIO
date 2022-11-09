#pragma once

#ifndef ROTARY_ENCODER_H
#define ROTARY_ENCODER_H

#include <stdint.h>
#include "hardware/pio.h"
#include "C:/Users/Miguel Estrada/Desktop/Workshop Intel 2022/Use case RP2040/Waveform generator/build/pio_programs.pio.h"

namespace pico_RotaryEncoder{
    class RotaryEncoder
    {
        private:
            PIO pio;
            int old_value;
            int max_step_rate;
            uint button;
            uint pin;

        public:
            uint offset;
            int delta = 0;
            int new_value;
            int reset_offset = 0;
            int step = 1;
            uint sm;
            void setStep(int step);
            int getCount(int step);
            int getCount(void);
            bool getbutton(void);
            int getDelta(void);
            void resetCount(void);
            RotaryEncoder(PIO pio, const pio_program* prgm, uint button, uint pinAB, int max_step_rate, uint32_t SYS_CLK);
    };
}

#endif
