#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "pio_programs.pio.h"

#include <math.h>

#include <stdio.h>
#include "hardware/pll.h"
#include "hardware/clocks.h"
#include "hardware/structs/pll.h"
#include "hardware/structs/clocks.h"

#include "hardware/dma.h"

#define SYS_CLK (float)clock_get_hz(clk_sys)

#define BUFF_LENGHT 256
#define WAVEFORM 0
#define WAVE_FREQ 15000

#define PI 3.1428592654

int main() 
{
    stdio_init_all();

//|Arbitrary Waveform Generator|

    // Buffer array attributed with an alignment for a memory allocation boundary according to the buffer size
    uint8_t buff[BUFF_LENGHT] __attribute__((aligned(BUFF_LENGHT)));

	int sine = WAVEFORM;
    switch(sine)
    {	

	// for sine wave
		case 0 :
			for (int i = 0; i < BUFF_LENGHT; ++i) 
            {
				float factor=(float)i/BUFF_LENGHT;                                      //convert integer division to floating point
				buff[i] = 128 + (sin((factor)*2*PI)*127);                               //Loads the AWG Buffer table with values of the sine wave
			}
			break;		

	//For square wave
		case 1:
			for (int i = 0; i < BUFF_LENGHT; ++i) 
            {
				if(i <= BUFF_LENGHT/2)buff[i] = 0;                                      //Loads a reset[0] state to AWG Buffer
                else buff[i] = 255;                                                     //Loads a set[255] state to AWG Buffer
			}
			break;

    //For Arbitrary Waveform Generator
        case 2 :
			for (int i = 0; i < BUFF_LENGHT; ++i) 
            {
				float factor=(float)i/BUFF_LENGHT;                                      //convert integer division to floating point
				// put the AWG formula here:
				buff[i] = 128 + (sin((factor)*2*PI)*57) - sin((factor)*72*PI)*70;       //Loads the AWG Buffer table with values of the arbitrary wave
			}
			break;	

        default:
            break;
	}

//|Programmable Input Output Subsystem|

    uint sm_clk_freq = SYS_CLK;                                                         //Maximum waveform frequency
    //uint sm_clk_freq = WAVE_FREQ*BUFF_LENGHT/2;                                       //state machine clock frequency
    uint32_t p_index_pin = 9;                                                           //index of the 8-bit parallel port [16-9]
    uint32_t p_num_pins = 8;                                                            //number of parallel pins
    uint32_t p_clk_pin = 17;                                                            //clk pin
    PIO pio_wg = pio0;                                  
    uint DREQ_PIO = 0;
    
    uint sm_wg = pio_claim_unused_sm(pio_wg, true);                                     // Get a free state mahcine from PIO 0

    if(pio_wg == pio0)                                                                  // Determine DREQ_TX according to PIO and State Machine
    {
        switch (sm_wg)
        {
            case 0:DREQ_PIO = DREQ_PIO0_TX0;break;
            case 1:DREQ_PIO = DREQ_PIO0_TX1;break;
            case 2:DREQ_PIO = DREQ_PIO0_TX2;break;
            case 3:DREQ_PIO = DREQ_PIO0_TX3;break;
            default:break;
        }
        
    } 
    else
    {
        switch (sm_wg)
        {
            case 0:DREQ_PIO = DREQ_PIO1_TX0;break;
            case 1:DREQ_PIO = DREQ_PIO1_TX1;break;
            case 2:DREQ_PIO = DREQ_PIO1_TX2;break;
            case 3:DREQ_PIO = DREQ_PIO1_TX3;break;
            default:break;
        }
    }

    uint offset_wg = pio_add_program(pio_wg, &ParallelPort_program);                    // Add PIO program to PIO instruction memory, return offset of the program

    ParallelPort_program_init(pio_wg, sm_wg, offset_wg, p_index_pin, p_num_pins, p_clk_pin, sm_clk_freq, SYS_CLK);
    
//|Direct Memory Access|

    int DMA_ch_a = dma_claim_unused_channel(true);                                      // Request free DMA channel
    int DMA_ch_b = dma_claim_unused_channel(true);                                      // Request free DMA channel

    dma_channel_config DMA_ch_a_cnfg = dma_channel_get_default_config(DMA_ch_a);        // Configure a default DMA channel
    channel_config_set_chain_to(&DMA_ch_a_cnfg, DMA_ch_b);                              // Set DMA chain from CHA -> CHB
    channel_config_set_dreq(&DMA_ch_a_cnfg, DREQ_PIO);                                  // Set DREQ signal from PIO
    channel_config_set_ring(&DMA_ch_a_cnfg, false, (uint)log2(BUFF_LENGHT));            // Set the ring buffer configuration, false, and set ring lenght according to buffer size

    dma_channel_config DMA_ch_b_cnfg = dma_channel_get_default_config(DMA_ch_b);        // Configure a default DMA channel
    channel_config_set_chain_to(&DMA_ch_b_cnfg, DMA_ch_a);                              // Set DMA chain from CHB -> CHA
    channel_config_set_dreq(&DMA_ch_b_cnfg, DREQ_PIO);                                  // Set DREQ signal from PIO
    channel_config_set_ring(&DMA_ch_b_cnfg, false, (uint)log2(BUFF_LENGHT));            // Set the ring buffer configuration, false, and set ring lenght according to buffer size

    dma_channel_configure(
        DMA_ch_a,                                                                       // DMA Channel
        &DMA_ch_a_cnfg,                                                                 // DMA Configuration
        &pio_wg->txf[sm_wg],                                                            // set DMA output pointer to the State Machine TX FIFO
        buff,                                                                           // Assign DMA data buffer
        BUFF_LENGHT,                                                                    // set DMA buffer lenght
        false                                                                           //
    );

    dma_channel_configure(
        DMA_ch_b,                                                                       // DMA Channel
        &DMA_ch_b_cnfg,                                                                 // DMA Configuration
        &pio_wg->txf[sm_wg],                                                            // set DMA output pointer to the State Machine TX FIFO
        buff,                                                                           // Assign DMA data buffer
        BUFF_LENGHT,                                                                    // set DMA buffer lenght
        false                                                                           //
    );
    
    dma_start_channel_mask(1u << DMA_ch_a);                                             // Begin DMA chain by starting the first channel

//|Arm core|

    for(int i = 0; i <= 8; i++)
    {
        gpio_init(i);                                                                   //Initialize pin
        gpio_set_dir(i, true);                                                          //Set pin as output
    }

    while (true) 
    {
        //Fastest CPU Data thoughput
        for (int i = 0; i < BUFF_LENGHT; i++)
        {
            gpio_put_masked(0b111111111, buff[i] | (0<<8));                             //Pins [7-0](parallel data) output a byte and pin [8](clk) is reset in parallel.                       
            gpio_put(8,true);                                                           //Pin [8](clk) is set
        } 
    }
}