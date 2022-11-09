#pragma once

#ifndef WG_CH_H
#define WG_CH_H

#include <stdint.h>
#include <math.h>
#include "hardware/pio.h"
#include "C:/Users/Miguel Estrada/Desktop/Workshop Intel 2022/Use case RP2040/Waveform generator/build/pio_programs.pio.h"
#include "hardware/dma.h"

#define SINE 0
#define SQR 1
#define SAW 2
#define PI 3.1428592654
#define FACTOR(i,BF_DP) (float)i/BF_DP
#define BUFF_DEPHT 256

namespace pico_WG_CH{
    class WG_CH 
    {
        private:
            int pp;
            int DMA_ch_a;
            int DMA_ch_b;
            uint DREQ_PIO;
            dma_channel_config DMA_ch_a_cnfg;
            dma_channel_config DMA_ch_b_cnfg;
            uint32_t p_index_pin;
            uint32_t p_num_pins;
            uint32_t p_clk_pin;
            void Set_DMA(void);

        public: 
            uint sm;
            uint offset;
            PIO pio;
            WG_CH(PIO pio, const pio_program* prgm, uint32_t SYS_CLK, float freq, uint32_t indx_pin, uint32_t n_pins, uint32_t clk_pin);
            uint32_t id = 0;
            float clk_freq;
            uint32_t buff_depht = BUFF_DEPHT;
            uint8_t buff[BUFF_DEPHT] __attribute__((aligned(BUFF_DEPHT)));
            void Buffer_fill(unsigned int waveform);
    };
};

#endif //WG_CH_H
//WG_CH ch0 = WG_CH(pio0, &ParallelPort_program, SYS_CLK, 100*KHZ, 9, 8, 17);
    //WG_CH ch1 = WG_CH(pio0, &ParallelPort_program, SYS_CLK, 10*KHZ, 0, 8, 8);
    //ch0.Buffer_fill(SINE);
    //ch1.Buffer_fill(SINE);