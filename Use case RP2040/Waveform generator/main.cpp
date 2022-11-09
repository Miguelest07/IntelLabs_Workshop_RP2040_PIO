/*

MicroSupply test board

*/
#include "pico/stdlib.h"
#include "pio_programs.pio.h"
#include "Includes/HMI.h"
#include "hardware/adc.h"
#include "pico-ssd1306/ssd1306.h"
#include "hardware/i2c.h"
#include "pico-ssd1306/textRenderer/TextRenderer.h"
#include "pico-ssd1306/shapeRenderer/ShapeRenderer.h"
#include <math.h>

#define HMI_cpu

#define FREQ_FACT (float)100000/781
#define MAX_CURRENT 1000
#define MAX_VOLTAGE 22000
#define MAX_FREQ 10000000

using namespace pico_HMI;
using namespace pico_ssd1306;

struct APP_Resources
{
    uint32_t REG1 = 0;
    bool REG2 = false;
    uint32_t REG3 = 0;
    uint32_t REG4 = 0;
};

uint16_t RANDOM_STATE = 0xACE1u;
char msg[10];
int pol = 0;

float read_onboard_temperature(void);
float read_adc_ch(uint ch, int samples);
uint16_t randomAlgorithm (uint16_t state);
uint16_t Rand_f (void);

int main() 
{
    stdio_init_all();

    #ifdef HMI_cpu
//|Rotary knob|
    RotaryEncoder rotary = RotaryEncoder(pio1, &RotaryEncoder_program, 19, 20, 5, SYS_CLK);

//|OLED I2C|
    i2c_init(i2c1, 1000000); 
    gpio_set_function(27, GPIO_FUNC_I2C);
    gpio_set_function(26, GPIO_FUNC_I2C);
    gpio_pull_up(27);
    gpio_pull_up(26);

    sleep_ms(250);
    
    SSD1306 display = SSD1306(i2c1, 0x3C, Size::W128xH64);
    display.setOrientation(0);
    display.clear();

    unsigned char image[] = {
        0x60, 0x30, 0x18, 0x0E, 0x0E, 0x18, 0x30, 0x60
    };

    unsigned char A_inv [] = {
        0b11111111,
        0b11111111,
        0b11100111,
        0b11100111,
        0b11000011,
        0b11000011,
        0b11011011,
        0b10011001,
        0b10000001,
        0b10000001,
        0b10111101,
        0b10111101,
        0b10111101,
        0b10111101,
        0b11111111,
        0b11111111
    };

    unsigned char V_inv [] = {
        0xFF, 0xFF, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0xDB, 0xDB, 0xC3, 0xE7, 0xFF, 0xFF
    };

    unsigned char m_inv [] = {
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xE0, 0xEA, 0xEA, 0xEE, 0xFF, 0xFF
    };

    unsigned char USB_logo[] = {
        0x00, 0x00, 0x1F, 0xFF, 0xFF, 0xFF, 0xF8, 0x00, 0x00, 0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0xF8, 0x00,
        0x00, 0x07, 0xFF, 0xFF, 0xFF, 0xFF, 0xF8, 0x00, 0x00, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF, 0xF8, 0x00,
        0x00, 0x1F, 0xFF, 0xFF, 0xFF, 0xFF, 0xF8, 0x00, 0x00, 0x3F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xE0,
        0x00, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF0, 0x00, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF8,
        0x00, 0xFC, 0x1F, 0x07, 0x81, 0xF8, 0x00, 0xF8, 0x00, 0xFC, 0x1F, 0x06, 0x00, 0x78, 0x00, 0x78,
        0x01, 0xFC, 0x1E, 0x04, 0x00, 0x38, 0x00, 0x38, 0x01, 0xF8, 0x3E, 0x0C, 0x38, 0x38, 0x00, 0x38,
        0xFF, 0xF8, 0x3E, 0x08, 0x3C, 0x38, 0xF8, 0x38, 0xFF, 0xF8, 0x3C, 0x08, 0x1E, 0x30, 0xF8, 0x38,
        0xFF, 0xF0, 0x3C, 0x1C, 0x03, 0xF0, 0xF0, 0x78, 0xFF, 0xF0, 0x3C, 0x1C, 0x00, 0xF0, 0x00, 0xF0,
        0xFF, 0xF0, 0x7C, 0x1F, 0x00, 0x70, 0x00, 0x70, 0xFF, 0xF0, 0x78, 0x1F, 0xE0, 0x31, 0xF0, 0x38,
        0xFF, 0xF0, 0x78, 0x30, 0xF8, 0x21, 0xF8, 0x38, 0xFF, 0xE0, 0x70, 0x30, 0xF8, 0x21, 0xF0, 0x38,
        0x01, 0xF0, 0x00, 0x30, 0x78, 0x60, 0x00, 0x38, 0x01, 0xF0, 0x00, 0x70, 0x00, 0x60, 0x00, 0x38,
        0x01, 0xF8, 0x00, 0x78, 0x00, 0x60, 0x00, 0x78, 0x01, 0xFC, 0x00, 0xFC, 0x01, 0xE0, 0x00, 0xE0,
        0x01, 0xFF, 0x03, 0xFF, 0x07, 0xFF, 0xFF, 0xE0, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xE0,
        0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0x00, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0x80, 0x00,
        0x00, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0x80, 0x00, 0x00, 0x3F, 0xFF, 0xFF, 0xFF, 0xFF, 0x80, 0x00,
        0x00, 0x1F, 0xFF, 0xFF, 0xFF, 0xFF, 0x80, 0x00, 0x00, 0x07, 0xFF, 0xFF, 0xFF, 0xFF, 0x80, 0x00

    };
    
//|ADC|
    adc_init();
    adc_set_temp_sensor_enabled(true);

//|PICO power supply noise enhancement with PWM mode|
    gpio_init(23);
    gpio_set_dir(23, true);
    gpio_put(23, true);

//|HMI Status and resources|
    int HMIstatus = 0;
    int APPstatus = 0;

    APP_Resources VoltageControl;
    APP_Resources CurrentControl;

    #endif

//|Arbitrary Waveform Generator|

    APP_Resources WaveformControl;

    WaveformControl.REG1 = 488;
    WaveformControl.REG3 = 488281*FREQ_FACT;
    WaveformControl.REG4 = 488281*FREQ_FACT;

    WG_CH ch0 = WG_CH(pio0, &ParallelPort_program, SYS_CLK, WaveformControl.REG3, 9, 8, 17);
    //WG_CH ch1 = WG_CH(pio0, &ParallelPort_program, SYS_CLK, WaveformControl.REG4, 0, 8, 8);

    ch0.Buffer_fill(SAW);
    //ch1.Buffer_fill(SINE);

//|Arm core|

    for(int i = 0; i <= 8; i++)
    {
        gpio_init(i);
        gpio_set_dir(i, true);
    }
    

    uint32_t cpu_buff [ch0.buff_depht];

    // SAWTOOTH
    for (int i = 0; i < ch0.buff_depht; i++)
    {
        cpu_buff  [i] = i*255/(ch0.buff_depht-1);
    }
    /*
    // SINE
    for (int i = 0; i < ch0.buff_depht; i++)
    {
        float factor =(float) i/ch0.buff_depht;
        cpu_buff  [i] = (128 + (sin((factor*2*PI))*127));
        
    }
    */

    while (true) 
    {
        for (int i = 0; i < ch0.buff_depht; i++)
        {
            gpio_put_masked(0b111111111, cpu_buff[i] | (0<<8));
            gpio_put(8,true);
        }

        #ifdef HMI_cpu

        int mod1 = rotary.getCount(2);
        if(mod1 < 0)mod1 *= -1;
        int mod2 = mod1%2;
        int mod4 = mod1%4;

        drawText(&display, font_12x16, "uSupply", 0 ,0);
        drawText(&display, font_5x8, "V1.25", 85 ,8);

        switch (HMIstatus)
        {

//|Initial interface|
        case 0:
            display.addBitmapImage(1, 18 + (mod4 * 12), 8, 8, image);

            switch ((mod1 / 4)%3)
            {
            case 0:
                
                drawText(&display, font_8x8, "Power", 10 , 18);
                drawText(&display, font_8x8, "Current", 10 , 30);
                drawText(&display, font_8x8, "Voltage", 10 , 42);
                drawText(&display, font_8x8, "USB Control", 10 , 54);
                break;
            case 1:
                drawText(&display, font_8x8, "Temperature", 10 , 18);
                drawText(&display, font_8x8, "WaveformFreq", 10 , 30);
                drawText(&display, font_8x8, "AnalogGen", 10 , 42);
                drawText(&display, font_8x8, "DigitalGen", 10 , 54);
                break;
            
            default:
                drawText(&display, font_8x8, "APP1", 10 , 18);
                drawText(&display, font_8x8, "APP2", 10 , 30);
                drawText(&display, font_8x8, "APP3", 10 , 42);
                drawText(&display, font_8x8, "APP4", 10 , 54);
                break;
            }
            if(!rotary.getbutton())
            {
                sleep_ms(300);
                HMIstatus = (mod1%12) + 1;
            }
            break;

//|MicroSupply: Power configuration(Watts-Milliwatts)|
        case 1:
            if(!rotary.getbutton())
            {
                sleep_ms(300);
                if(mod4 == 0)HMIstatus = mod4;
                else HMIstatus = mod4 + 1;
            }

            display.addBitmapImage(1, 18 + (mod4 * 12), 8, 8, image);

            drawText(&display, font_5x8, "Initial", 10 , 18);
            drawText(&display, font_5x8, "Current", 10 , 30);
            drawText(&display, font_5x8, "Voltage", 10 , 42);
            drawText(&display, font_5x8, "USB", 10 , 54);

            drawRect(&display, 0, 63, 127, 16);
            //drawRect(&display, 0, 15, 127, 0);

            itoa(VoltageControl.REG1, msg, 10);
            drawText(&display, font_12x16, msg, 47 ,20);
            display.addBitmapImage( 109, 20, 8, 16, m_inv);
            display.addBitmapImage( 117, 20, 8, 16, V_inv);

            itoa(CurrentControl.REG1, msg, 10);
            drawText(&display, font_12x16, msg, 47 ,44);
            display.addBitmapImage( 109, 44, 8, 16, m_inv);
            display.addBitmapImage( 117, 44, 8, 16, A_inv);
            
            break;
//|MicroSupply: Current configuration(Amperes-Milliamperes)|
        case 2:
            if(!rotary.getbutton())
            {
                sleep_ms(300);
                if(mod2 == 0 && (!CurrentControl.REG2)){
                    HMIstatus = mod2;
                    APPstatus = mod2;
                }
                switch (APPstatus)
                {
                    case 0:
                    APPstatus = mod2;
                    break;
                    case 1:
                    if(CurrentControl.REG2)CurrentControl.REG2 = false;
                    else CurrentControl.REG2 = true;
                    break;
                }
            }
            if(!CurrentControl.REG2)display.addBitmapImage(1, 18 + (mod2 * 12), 8, 8, image);

            drawText(&display, font_5x8, "Back", 10 , 18);
            drawText(&display, font_5x8, "Adj", 10 , 30);
            //drawText(&display, font_5x8, "Voltage", 10 , 42);
            //drawText(&display, font_5x8, "USB", 10 , 54);

        
            switch (APPstatus)
            {
            case 0:
                itoa(CurrentControl.REG1, msg, 10);
                drawText(&display, font_12x16, msg, 47 ,20);
                display.addBitmapImage( 109, 20, 8, 16, m_inv);
                display.addBitmapImage( 117, 20, 8, 16, A_inv);
                //CurrentControl.REG3 = mod1;
                break;
            case 1:
                if(CurrentControl.REG2){
                    int delta = rotary.getDelta();
                    if (delta > 0)
                    {
                        if(CurrentControl.REG1 <= MAX_CURRENT) CurrentControl.REG1 ++;
                        if(CurrentControl.REG1 > MAX_CURRENT) CurrentControl.REG1 = MAX_CURRENT;
                    }
                    else if (delta < 0)
                    {
                        if(CurrentControl.REG1 > 0) CurrentControl.REG1 --;
                        if(CurrentControl.REG1 == 0) CurrentControl.REG1 = 0;
                    }
                    sleep_ms(10);
                }
                
                itoa(CurrentControl.REG1, msg, 10);
                drawText(&display, font_12x16, msg, 47 ,20);
                display.addBitmapImage( 109, 20, 8, 16, m_inv);
                display.addBitmapImage( 117, 20, 8, 16, A_inv);
                break;
            default:
                break;
            }
            break;

//|MicroSupply: Voltage configuration(Volts-Millivolts)|
        case 3:
            if(!rotary.getbutton())
            {
                sleep_ms(300);
                if(mod2 == 0 && (!VoltageControl.REG2)){
                    HMIstatus = mod2;
                    APPstatus = mod2;
                }
                switch (APPstatus)
                {
                    case 0:
                    APPstatus = mod2;
                    break;
                    case 1:
                    if(VoltageControl.REG2)VoltageControl.REG2 = false;
                    else VoltageControl.REG2 = true;
                    break;
                }
            }
            
            if(!VoltageControl.REG2)display.addBitmapImage(1, 18 + (mod2 * 12), 8, 8, image);
            
            drawText(&display, font_5x8, "Back", 10 , 18);
            drawText(&display, font_5x8, "Adj", 10 , 30);

            itoa(VoltageControl.REG1, msg, 10);
            drawText(&display, font_12x16, msg, 47 ,20);
            display.addBitmapImage( 109, 20, 8, 16, m_inv);
            display.addBitmapImage( 117, 20, 8, 16, V_inv);

            switch (APPstatus)
            {
                case 0:
                    itoa(VoltageControl.REG1, msg, 10);
                    drawText(&display, font_12x16, msg, 47 ,20);
                    display.addBitmapImage( 109, 20, 8, 16, m_inv);
                    display.addBitmapImage( 117, 20, 8, 16, V_inv);
                    break;
                case 1:
                    if(VoltageControl.REG2){
                        int delta = rotary.getDelta();
                        if (delta > 0)
                        {
                            if(VoltageControl.REG1 <= MAX_VOLTAGE) VoltageControl.REG1 ++;
                            if(VoltageControl.REG1 > MAX_VOLTAGE) VoltageControl.REG1 = MAX_VOLTAGE;
                        }
                        else if (delta < 0)
                        {
                            if(VoltageControl.REG1 > 0) VoltageControl.REG1 --;
                            if(VoltageControl.REG1 == 0) VoltageControl.REG1 = 0;
                        }
                        sleep_ms(10);
                    }
                    
                    itoa(VoltageControl.REG1, msg, 10);
                    drawText(&display, font_12x16, msg, 47 ,20);
                    display.addBitmapImage( 109, 20, 8, 16, m_inv);
                    display.addBitmapImage( 117, 20, 8, 16, V_inv);
                    break;
                default:
                    break;
            }
            break;

//|MicroSupply: USB Interface|
        case 4:
            display.addBitmapImage(1, 18 + (mod4 * 12), 8, 8, image);
            
            drawText(&display, font_5x8, "Back", 10 , 18);

            display.addBitmapImage( 64, 24, 64, 32, USB_logo);

            if(!rotary.getbutton())
            {
                sleep_ms(300);
                HMIstatus = mod4;
            }
            break;  

//|MicroSupply: On-board temperature|
        case 5:
            display.addBitmapImage(1, 18 + (mod4 * 12), 8, 8, image);
            
            drawText(&display, font_5x8, "Back", 10 , 18);

            itoa(read_onboard_temperature(), msg, 10);
            drawText(&display, font_12x16, msg, 47 ,20);

            if(!rotary.getbutton())
            {
                sleep_ms(300);
                HMIstatus = mod4;
            }
            break; 

//Arbitrary Waveform Generator: Waveform frequency configuration (KHz)|
        case 6:
            if(!rotary.getbutton())
            {
                sleep_ms(300);
                if(mod2 == 0 && (!WaveformControl.REG2)){
                    HMIstatus = mod2;
                    APPstatus = mod2;
                }
                switch (APPstatus)
                {
                    case 0:
                    APPstatus = mod2;
                    break;
                    case 1:
                    if(WaveformControl.REG2)WaveformControl.REG2 = false;
                    else WaveformControl.REG2 = true;
                    break;
                }
            }
            
            if(!WaveformControl.REG2)display.addBitmapImage(1, 18 + (mod2 * 12), 8, 8, image);
            
            drawText(&display, font_5x8, "Back", 10 , 18);
            drawText(&display, font_5x8, "Adj", 10 , 30);

            itoa(WaveformControl.REG1, msg, 10);
            drawText(&display, font_12x16, msg, 47 ,20);
            drawText(&display, font_12x16, "KHz", 95 ,20);

            switch (APPstatus)
            {
                case 0:
                    itoa(WaveformControl.REG1, msg, 10);
                    drawText(&display, font_12x16, msg, 47 ,20);
                    break;
                case 1:
                    if(WaveformControl.REG2){
                        int delta = rotary.getDelta();
                        if (delta > 0)
                        {
                            if(WaveformControl.REG1 <= MAX_FREQ) WaveformControl.REG1 ++;
                            if(WaveformControl.REG1 > MAX_FREQ) WaveformControl.REG1 = MAX_FREQ;
                        }
                        else if (delta < 0)
                        {
                            if(WaveformControl.REG1 > 0) WaveformControl.REG1 --;
                            if(WaveformControl.REG1 == 0) WaveformControl.REG1 = 0;
                        }

                        WaveformControl.REG3 = WaveformControl.REG1*KHZ*FREQ_FACT + 1*FREQ_FACT;
                        WaveformControl.REG4 = WaveformControl.REG1*KHZ*FREQ_FACT + 1*FREQ_FACT;

                        pio_sm_set_clkdiv(ch0.pio, ch0.sm, SYS_CLK/WaveformControl.REG3);
                        //pio_sm_set_clkdiv(ch1.pio, ch1.sm, SYS_CLK/WaveformControl.REG4);
                        sleep_ms(10);
                    }
                    
                    itoa(WaveformControl.REG1, msg, 10);
                    drawText(&display, font_12x16, msg, 47 ,20);
                    break;
                default:
                    break;
            }
            break;

//Arbitrary Waveform Generator: Arbitrary Function Generator|
        case 7:
            display.addBitmapImage(1, 18 + (mod4 * 12), 8, 8, image);
            
            drawText(&display, font_5x8, "Back", 10 , 18);

            drawText(&display, font_12x16, "AnalogGen", 47 ,20);

            if(!rotary.getbutton())
            {
                sleep_ms(300);
                HMIstatus = mod4;
            }
            break; 

//Arbitrary Waveform Generator: Voltage-controlled Digital Function Generator (0 to V+)|
        case 8:
            display.addBitmapImage(1, 18 + (mod4 * 12), 8, 8, image);
            
            drawText(&display, font_5x8, "Back", 10 , 18);

            drawText(&display, font_12x16, "DigitalGen", 47 ,20);

            

            if(!rotary.getbutton())
            {
                sleep_ms(300);
                HMIstatus = mod4;
            }
            break;

//MicroSupply: Screen saver (random points)|   
        default:
            /*
            for (int i = 0; i < 500; i++)
            {
                int randx = (Rand_f()*127/65535);
                for(int i = 0; i < 10; i++)int xxx = Rand_f();
                int randy = (Rand_f()*63/65535);
                display.setPixel(randx, randy);
                display.sendBuffer();
                //printf("pos: %i, posstp4: %i, df: %i, gear: %i, BTN: %i\n",  rotary.getCount(), rotary.getCount(4), rotary.delta, rotary.step, rotary.getbutton());
            }*/
            break;
        }

        display.sendBuffer();
        display.clear();
        #endif
    }
}

float read_onboard_temperature(void) 
{
    const float conversionFactor = 3.3f / (1 << 12);
    adc_select_input(4);
    float adc = (float)adc_read() * conversionFactor;
    float tempC = 27.0f - (adc - 0.706f) / 0.001721f;
    return tempC;
}

float read_adc_ch(uint ch, int samples) 
{
    adc_select_input(ch);
    float adc = 0;
    for(int i = 0; i < samples; i++)
    {
        adc += (float)adc_read();
    }
    return adc/samples;
}

uint16_t randomAlgorithm (uint16_t state)
{
    uint16_t newState = state;
    uint16_t newBit = 0;
    for(uint16_t i = 0; i < 32; i++)
    {
        newBit = ((state >> 0) ^ (state >> 2) ^ (state >> 3) ^ (state >> 5) ) & 1;
        newState = (state >> 1) | (newBit << 15);
    }
    return newState;
}

uint16_t Rand_f (void)
{
    uint16_t prev = RANDOM_STATE;
    RANDOM_STATE = randomAlgorithm(RANDOM_STATE);
    if(RANDOM_STATE == prev)RANDOM_STATE = randomAlgorithm(RANDOM_STATE);
    return RANDOM_STATE;
}