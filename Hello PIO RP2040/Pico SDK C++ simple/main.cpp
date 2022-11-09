#include "pico/stdlib.h"
#include "pio_programs.pio.h"
#include "hardware/pio.h"

#include "hardware/clocks.h"
#include "hardware/structs/clocks.h"

#define SYS_CLK (float)clock_get_hz(clk_sys)

int main() 
{
    stdio_init_all();

    // Select PIO
    PIO Npio = pio0;

    // Get a free state mahcine from PIO
    uint sm = pio_claim_unused_sm(Npio, true); 

    //Get pio program offset in memory
    uint offset = pio_add_program(Npio, &ParallelPort_program); 

    // Add program to PIO instruction set and sm variables
    ParallelPort_program_init(  // Initialize PIO program 
        Npio,                   // PIO number
        sm,                     // State machine number
        offset,                 // PIO program offset
        0,                      // Index Output pin
        8,                      // Number of Output pins
        1*MHZ,                  // State machine clock frequency
        SYS_CLK                 // RP2040 System clock
    ); 

    while (true) 
    {
        for(int i = 0; i <= 255; i++)
        {
            pio_sm_put(Npio, sm, i);
            //sleep_ms(1);
        }
    }
}
