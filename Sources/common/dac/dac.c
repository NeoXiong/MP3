#define DAC_C
#include <mqx.h>

#include"../audio/dai_link.h"
#include"dac_prv.h"
#ifdef AUDIO_PHY_DAC
int32_t ddma_trigcount = 2;

static void dac_stop(void);
static void dac0_hwtriger_init(uint8_t BuffMode,uint8_t Vreference, uint8_t TrigMode, uint8_t BuffInitPos, uint8_t BuffUpLimit,uint8_t WatermarkMode);
static uint32_t dac_dma_add_buffer(DAC_MemMapPtr dacx_base_ptr, uint16_t *pwBuffer, uint32_t dwSize );

int8_t dac_init(/* uint32_t pcmSampleRate, uint8_t bitWidth, uint8_t chNum*/)
{
	
	SIM_SCGC2 |= SIM_SCGC2_DAC0_MASK ;   //Allow clock to enable DAC0
	
	dac0_hwtriger_init(DAC_BF_NORMAL_MODE,DAC_SEL_VDDA,DAC_SEL_PDB_HW_TRIG,DAC_SET_PTR_AT_BF(0),DAC_SET_PTR_UP_LIMIT(15),/*WM_WORDS*/DAC_BFWM_4WORDS) ;
	dac0_dma_init();

	if(_int_install_isr(DAC0_DMA_INT, DAC0_DMA_isr,NULL) == NULL) {
			printf("install DAC0_DMA_INT fail\n");
		} 
	_nvic_int_init(DAC0_DMA_INT, 2, TRUE); 
	pdb_init(AUDIO_NULL, DEFAULT_AUDIO_SAMPLERATE);
	return 0;
}

void dac_deinit(void)
{
	dac_stop();
	
	/*
	#if (MQX_CPU == PSP_CPU_MK70F120M)
		 SIM_SCGC6 &= ~SIM_SCGC6_DMAMUX0_MASK;  // Disable clocks to the DMA Mux.
	#else
	#if MQX_CPU == PSP_CPU_MK60D100M 
		 SIM_SCGC6 &= ~SIM_SCGC6_DMAMUX_MASK;  // Disable clocks to the DMA Mux.
	#else
	#error "can not recognize chip"
	#endif
	#endif
	    SIM_SCGC7 &= ~SIM_SCGC7_DMA_MASK;        // Disable clocks to the eDMA module.
	    */
	   
}
/*
 * flush the buffer
 */
int8_t dac_flush()
{
	return 0;
}

/*
 * 
 */
int8_t dac_ioctl(uint32_t cmd, uint32_t value)
{
	return 0;
}
int32_t dac_write(uint16_t* samples, uint16_t  numSamples)
{
	/*try until write success*/
    return dac_dma_add_buffer(DAC0_BASE_PTR, samples, numSamples);
}

int32_t dac_set_format(uint32_t sampleRate, uint8_t bitWidth, uint8_t chNum)
{
	return 0;
}
int32_t dac_vol_up(void)
{
	return 0;
}
int32_t dac_vol_down(void)
{
	return 0;
}
int32_t dac_mute(void)
{
	return 0;
}
int32_t dac_umute(void)
{
	return 0;
}


void dac_start(void)
{
	dma_start_channel(0); 
	pdb_start();
}

static void dac_stop(void)
{
	dma_stop_channel(0);
	pdb_stop();
}


/*
 * return 0 succeed, none 0 failed
 */
/* dwSize must aligned with 16 x short */
static uint32_t dac_dma_add_buffer(DAC_MemMapPtr dacx_base_ptr, uint16_t *pwBuffer, uint32_t dwSize )  /* dwSize is samples number, is 2byte per frame */
{
	uint32_t dwSize_aligned16; 
	static uint8_t polling = 0;
	
	dwSize_aligned16 = dwSize - dwSize % 16;
	if (ddma_trigcount != 0) {
		uint8_t tcd_num = ddma_trigcount % 2; /* ddma_trigcount =2 ,tcdlist = 0 */
      	dma_update_tcd_list(tcd_num, (uint32_t)pwBuffer, dwSize_aligned16*2/DAC_BUFFER_SIZE,  dwSize_aligned16*2/DAC_BUFFER_SIZE);
		if (ddma_trigcount == 2) {
		 	dma_load_td_chain(DMA_CHANNEL_AUDIO);
		 	dma_start_channel(DMA_CHANNEL_AUDIO);
			ddma_trigcount--;
		 
		 }
		 else if (ddma_trigcount == 1 ){
			 ddma_trigcount--;	
			 dma_buffer_full_num = 2;
			 pdb_start();
		}
		return 0; 
	 }

    if(dma_buffer_full_num < 2)
	{
    	dma_update_tcd_list(polling, (uint32_t)pwBuffer, dwSize_aligned16*2/DAC_BUFFER_SIZE,  dwSize_aligned16*2/DAC_BUFFER_SIZE);
        polling = !polling;
        dma_buffer_full_num++;
        return 0 ;
    }
    else
	{
    	//full
		return 1;
    }
}


static inline void SET_DACx_BUFFER( DAC_MemMapPtr dacx_base_ptr, uint8_t dacindex, int_16 buffval){
	DAC_DATL_REG(dacx_base_ptr, dacindex)  =   (buffval&0x0ff); 
	DAC_DATH_REG(dacx_base_ptr, dacindex)  =   (buffval&0xf00) >>8;                                
}
static inline void DACx_register_reset(DAC_MemMapPtr dacx_base_ptr)
{
	/* unsigned char dacbuff_index;   

	for (dacbuff_index=0; dacbuff_index<16;dacbuff_index++){
	SET_DACx_BUFFER( dacx_base_ptr, dacbuff_index, DACx_DAT_RESET); 
	}*/

	/*DAC_SR_REG(dacx_base_ptr) = DACx_SR_RESET ;*/
	DAC_C0_REG(dacx_base_ptr) &=  (~(DAC_C0_DACEN_MASK));  //DACx_C0_RESET ;
	/*DAC_C1_REG(dacx_base_ptr) = DACx_C1_RESET;*/
	/*DAC_C2_REG(dacx_base_ptr) = DACx_C2_RESET;*/
}


static void dac0_hwtriger_init(uint8_t BuffMode,uint8_t Vreference, uint8_t TrigMode, uint8_t BuffInitPos, uint8_t BuffUpLimit,uint8_t WatermarkMode)
{
    DAC_C0_REG(DAC0_BASE_PTR) = (
	    DAC_BFB_PTR_INT_DISABLE/*DAC_BFB_PTR_INT_ENABLE*/ |             
	    /*DAC_BFT_PTR_INT_DISABLE*/DAC_BFT_PTR_INT_ENABLE |             
	    /*DAC_BFWM_INT_DISABLE*/DAC_BFWM_INT_ENABLE  |
	    DAC_HP_MODE    |
	    DAC_SW_TRIG_STOP |            
	    TrigMode |  
	    Vreference |     
	    DAC_ENABLE
	    /* DAC_DISABLE */
	    );  
   
    if (  Vreference == DAC_SEL_VREFO ) {
		SIM_SCGC4 |= !SIM_SCGC4_VREF_MASK ;
		VREF_SC = 0x82;
		while (!(VREF_SC & VREF_SC_VREFST_MASK)  ){}
    }
 
    DAC_C1_REG(DAC0_BASE_PTR) = ( 
	    DAC_C1_DMAEN_MASK | /* dac dma mode  */
	    DAC_BF_ENABLE  |   //Buffer Enabled            
	    WatermarkMode |  // set 1, 2, 3,or 4 word(s) mode 
	    BuffMode               // set traversal mode, normal, swing, or onetime
	    ) ;

    DAC_C2_REG(DAC0_BASE_PTR) = BuffInitPos | BuffUpLimit;
    DAC_SR_REG(DAC0_BASE_PTR) = 0;
    //////////////////////////////exist in yifang
}






void DACC_RESET(void){

	//DAC_C1_REG(DAC0_BASE_PTR)  &= (~DAC_C1_DMAEN_MASK );
	    
	PDB0_DACINTC0 &= (~PDB_DACINTC0_TOE_MASK) ; 
	DAC_C0_REG(DAC0_BASE_PTR) &=  (~(DAC_C0_DACEN_MASK));  
	//DAC_C0_REG(DAC0_BASE_PTR) = 0;

	//DMA_INT |=  (1<< DMA_CHANNEL_AUDIO);
		
	//DACx_register_reset(DAC0_BASE_PTR);	
	
	ddma_trigcount = 2;
}

#endif



















