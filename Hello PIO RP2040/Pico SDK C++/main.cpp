#include "pico/stdlib.h"
#include "pio_programs.pio.h"
#include "hardware/pio.h"

#include "hardware/clocks.h"
#include "hardware/structs/clocks.h"

#define SYS_CLK (float)clock_get_hz(clk_sys)

#define USING_PIO 1

int main() 
{
    stdio_init_all();
    
//|Programmable Input Output Subsystem|

    PIO Npio = pio0;

    uint sm = pio_claim_unused_sm(Npio, true);                  // Get a free state mahcine from PIO
    uint offset = pio_add_program(Npio, &ParallelPort_program); // Add PIO program to PIO instruction memory, return offset of the program

    // Add program to PIO instruction set and sm variables
    ParallelPort_program_init(                                  // Initialize PIO program 
        Npio,                                                   // PIO number
        sm,                                                     // State machine number
        offset,                                                 // PIO program offset
        9,                                                      // Index Output pin
        8,                                                      // Number of Output pins
        17,                                                     // Index Sideset pin
        10*MHZ,                                                 // State machine clock frequency
        SYS_CLK                                                 // RP2040 System clock
    ); 

//|Arm core|

    #ifndef USING_PIO
    for(int i = 0; i <= 8; i++)
    {
        gpio_init(i);                                           //Initialize pin
        gpio_set_dir(i, true);                                  //Set pin as output
    }
    #endif

    while (true) 
    {
        // PIO
        #ifdef USING_PIO
        for(int i = 0; i <= 255; i++)
        {
            pio_sm_put(Npio, sm, i);                            //Send data to state machine FIFO
        }
        #endif
        
        // CPU
        #ifndef USING_PIO
        //fast bitbang
        for (int i = 0; i < 255; i++)
        {
            gpio_put_masked(0b111111111, i | (0<<8));           //Pins [7-0](parallel data) output a byte and pin [8](clk) is reset in parallel.                       
            gpio_put(8,true);                                   //Pin [8](clk) is set
        } 

        //slow bitbang
        /*
        for (int i = 0; i < 255; i++)
        {
            gpio_put_masked(0b111111111);                       //Pins [7-0](parallel data) output a byte in parallel.   
            gpio_put(8,false);                                  //Pin [8](clk) is reset                    
            gpio_put(8,true);                                   //Pin [8](clk) is set
        } 
        */
        #endif
    }
}
