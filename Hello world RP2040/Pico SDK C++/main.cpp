#include "pico/stdlib.h"
#include <stdio.h>

int main (){
    stdio_init_all();               //Initialize all of the present standard stdio types that are linked into the binary.

    uint led = 25;                  //assing led variable as number 25

    gpio_init(led);                 //initialize a GPIO
    gpio_set_dir(led, true);        //Set pin as Output

    while(true){
        printf("Hello world!\r\n"); //Serial print like funtion
        gpio_put(led, true);        //Set pin
        sleep_ms(500);              //Sleep core 500 ms
        gpio_put(led, false);       //Reset pin
        sleep_ms(500);              //Sleep core 500 ms
    }
}
