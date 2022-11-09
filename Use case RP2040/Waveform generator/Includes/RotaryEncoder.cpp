#include "RotaryEncoder.h"

namespace pico_RotaryEncoder{

    RotaryEncoder::RotaryEncoder(PIO pio, const pio_program* prgm, uint button, uint pinAB, int max_step_rate, uint32_t SYS_CLK)
    {
        this->pio = pio;
        this->pin = pinAB;
        this->button = button;
        this->max_step_rate = max_step_rate;
        this->sm = pio_claim_unused_sm(this->pio, true);
        this->offset = pio_add_program(pio, prgm);
        RotaryEncoder_program_init(this->pio, this->sm, this->offset, this->pin, this->max_step_rate, SYS_CLK);
        gpio_pull_up(this->button);//button
        gpio_set_dir(this->button, false);
    }

    int RotaryEncoder::getCount(int step) 
    {
        if(step <= 0)return 0;
        this->new_value = RotaryEncoder_get_count(this->pio,this->sm)/(step*2);
        //this->new_value -= this->reset_offset;
        //this->delta = this->new_value - this->old_value;
        //this->old_value = this->new_value;
        return this->new_value;
    }

    int RotaryEncoder::getCount(void) 
    {
        this->new_value = RotaryEncoder_get_count(this->pio,this->sm)/(this->step*2);
        this->new_value -= this->reset_offset;
        this->delta = this->new_value - this->old_value;
        this->old_value = this->new_value;
        return this->new_value;
    }

    void RotaryEncoder::resetCount(void) 
    {
        this->reset_offset = RotaryEncoder_get_count(this->pio,this->sm)/(this->step*2);
    }

    int RotaryEncoder::getDelta(void) 
    {
        this->new_value = RotaryEncoder_get_count(this->pio,this->sm);
        this->delta = this->new_value - this->old_value;
        this->old_value = this->new_value;
        return this->delta;
    }

    bool RotaryEncoder::getbutton(void)
    {
        return gpio_get(this->button);
    }

    void RotaryEncoder::setStep(int step)
    {
        this->step = step;
    }
}