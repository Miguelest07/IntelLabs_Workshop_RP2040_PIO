#include "WaveformGenerator_CH.h"

namespace pico_WG_CH
{
    WG_CH::WG_CH(PIO pio, const pio_program* prgm, uint32_t SYS_CLK, float freq, uint32_t indx_pin, uint32_t n_pins, uint32_t clk_pin)
    {
        this->p_index_pin = indx_pin;
        this->p_num_pins = n_pins;
        this->p_clk_pin = clk_pin;
        this->clk_freq = freq;
        this->pio = pio;     
        
        this->sm = pio_claim_unused_sm(this->pio, true); // Get a free state mahcine from PIO 0

        if(this->pio == pio0)
        {
            switch (this->sm)
            {
                case 0:this->DREQ_PIO = DREQ_PIO0_TX0;break;
                case 1:this->DREQ_PIO = DREQ_PIO0_TX1;break;
                case 2:this->DREQ_PIO = DREQ_PIO0_TX2;break;
                case 3:this->DREQ_PIO = DREQ_PIO0_TX3;break;
                default:break;
            }
            
        } 
        else
        {
            switch (this->sm)
            {
                case 0:this->DREQ_PIO = DREQ_PIO1_TX0;break;
                case 1:this->DREQ_PIO = DREQ_PIO1_TX1;break;
                case 2:this->DREQ_PIO = DREQ_PIO1_TX2;break;
                case 3:this->DREQ_PIO = DREQ_PIO1_TX3;break;
                default:break;
            }
        }
        this->offset = pio_add_program(this->pio, prgm); // Add PIO program to PIO instruction memory, return offset of the program
        ParallelPort_program_init(this->pio, this->sm, this->offset, this->p_index_pin, this->p_num_pins, this->p_clk_pin, this->clk_freq, SYS_CLK);        // Initialize PIO program 
        this->Set_DMA();
    }
    
    void WG_CH::Set_DMA(void)
    {
        this->DMA_ch_a = dma_claim_unused_channel(true);
        this->DMA_ch_b = dma_claim_unused_channel(true);

        this->DMA_ch_a_cnfg = dma_channel_get_default_config(this->DMA_ch_a);
        channel_config_set_chain_to(&this->DMA_ch_a_cnfg, this->DMA_ch_b);
        channel_config_set_dreq(&this->DMA_ch_a_cnfg, this->DREQ_PIO);
        channel_config_set_ring(&this->DMA_ch_a_cnfg, false, 8);

        this->DMA_ch_b_cnfg = dma_channel_get_default_config(this->DMA_ch_b);
        channel_config_set_chain_to(&this->DMA_ch_b_cnfg, this->DMA_ch_a);
        channel_config_set_dreq(&this->DMA_ch_b_cnfg, this->DREQ_PIO);
        channel_config_set_ring(&this->DMA_ch_b_cnfg, false, 8);

        dma_channel_configure(
            this->DMA_ch_a,
            &this->DMA_ch_a_cnfg,
            &pio0->txf[this->sm],
            this->buff,
            BUFF_DEPHT,
            false
        );

        dma_channel_configure(
            this->DMA_ch_b,
            &this->DMA_ch_b_cnfg,
            &pio0->txf[this->sm],
            this->buff,
            BUFF_DEPHT,
            false
        );
        
        dma_start_channel_mask(1u << this->DMA_ch_a);                                                       // Start SM
    }

    void WG_CH::Buffer_fill(unsigned int waveform)
    {
        int d = 0;
        float factor;
        for(int i = 0; i < this->buff_depht; i++)
        {
            factor =(float) i/this->buff_depht;
            switch (waveform)
            {
            case SINE:
                d = (128 + (sin((factor*2*PI))*127));
                break;
            case SQR:
                if(i <= this->buff_depht/2) d = 255;
                else d = 0;
                break;
            case SAW:
                d = i*255/(this->buff_depht-1);
                break;
            default:
                break;
            }
            this->buff[i] = d;
        }
    }
}