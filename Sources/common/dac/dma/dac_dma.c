#include"../dac/dac_prv.h"
#include"../dac/dac.h"

#ifdef __CWCC__
DMA_TCD_t tcd_list[2] __attribute__ ((aligned(32)));
#else
#pragma data_alignment=32
DMA_TCD_t tcd_list[2];
#endif

extern int32_t dma_buffer_full_num;
static void dma_tcd_list_init(void);
static void dma_mux(int32_t channel, int32_t request);

void dac0_dma_init(void)
{
#if	(MQX_CPU == PSP_CPU_MK70F120M)
	  SIM_SCGC6 |= SIM_SCGC6_DMAMUX0_MASK; 	 // Enable clocks to the DMA Mux.
#else
#if MQX_CPU == PSP_CPU_MK60D100M 
    //enable minor off mode
	  SIM_SCGC6 |= SIM_SCGC6_DMAMUX_MASK; 	 // Enable clocks to the DMA Mux.
#else
#error "can not recognize chip"
#endif
#endif
  
    // ungate dma clock
    SIM_SCGC7 |= SIM_SCGC7_DMA_MASK;
    
   
    
    // clear flags
    DMA_CEEI |= DMA_CEEI_CAEE_MASK;
    DMA_CERQ |= DMA_CERQ_CAER_MASK;
    DMA_CDNE |= DMA_CDNE_CADN_MASK;
    DMA_CERR |= DMA_CERR_CAEI_MASK;
    DMA_CINT |= DMA_CINT_CAIR_MASK;

	DMA_EEI = 0x00;  /* */
    
    /*
    For proper operation, writes to the CR register must be
    performed only when the DMA channels are inactive; that is,
    when TCDn_CSR[ACTIVE] bits are cleared
    */
	
#if (MQX_CPU == PSP_CPU_MK70F120M)
    DMA_CR|=DMA_CR_EDBG_MASK;
#else
#if MQX_CPU == PSP_CPU_MK60D100M 
    //enable minor off mode
    DMA_CR = DMA_CR_EMLM_MASK;
#else
#error "can not recognize chip"
#endif
#endif
	
     dma_tcd_list_init();
	 dma_mux(DMA_CHANNEL_AUDIO, DMA_REQUEST_DAC0);
	 
	  //init_test_pin(0);
}

void dma_start_channel(uint32_t ch)
{
  /* It seemed that the two triggers(HW/SW) worked seperatedly */
	DMA_SERQ = DMA_SERQ_SERQ(ch);
}

void dma_stop_channel(uint32_t ch)
{
  /* It seemed that the two triggers(HW/SW) worked seperatedly */
	
	
	do {
		DMA_CERQ = DMA_CERQ_CERQ(ch);
	} while(DMA_CERQ & DMA_CSR_ACTIVE_MASK);
}

void dma_update_tcd_list(uint8_t listNum, uint32_t saddr, uint32_t citer, uint32_t biter)
{
	 tcd_list[listNum].SADDR =  saddr;
	 tcd_list[listNum].CITER_ELINKNO = citer;
	 tcd_list[listNum].BITER_ELINKNO =biter;
}
void DAC0_DMA_isr(void * handle)
{
	DMA_INT |=  (1<< DMA_CHANNEL_AUDIO);
	
	//DAC0_DMA_isr_hook();
	dma_buffer_full_num--;
	//Here should emerge a bug later
	g_sampleConsumed += PINGPONGBUFSIZE;
	
	if(dma_buffer_full_num <= 0)  
		{
#if 0
			light_led(0x01);
#endif
			dma_buffer_full_num = 0;
		}
	else
	{
#if 0
		light_down_led(0x01);
#endif
	
	}
        return;
}
static void dma_mux(int32_t channel, int32_t request)
{
   
#if (MQX_CPU == PSP_CPU_MK70F120M)
	//DMAMUX0_CHCFG(channel) |= (DMAMUX_CHCFG_ENBL_MASK | DMAMUX_CHCFG_SOURCE(request));
	DMAMUX0_CHCFG(channel) = (DMAMUX_CHCFG_ENBL_MASK | DMAMUX_CHCFG_SOURCE(request));
#else
#if MQX_CPU == PSP_CPU_MK60D100M 
	 DMAMUX_CHCFG(channel) = (DMAMUX_CHCFG_ENBL_MASK | DMAMUX_CHCFG_SOURCE(request));
#else
#error "can not recognize chip"
#endif
#endif
    
}
static void dma_tcd_list_init(void)
{
  int i = 0;
  
  memset(tcd_list, 0, sizeof(tcd_list));
 
  for(i = 0; i < 2; i++){
      tcd_list[i].SOFF = 1;	//////////////////////////////////////////////////// yifang 2
      // 8bits for D&S, 16bytes wrapped, ie, 4 address bit changing allowed.
#if 0      
      tcd_list[i].ATTR = DMA_ATTR_SSIZE(0) | DMA_ATTR_DSIZE(1) /*| DMA_ATTR_DMOD(5)*/;
#else
	tcd_list[i].ATTR = DMA_ATTR_SSIZE(0) | DMA_ATTR_DSIZE(1) | DMA_ATTR_DMOD(5);
						////////////////////////////////////////////////////ssize(1) yifang
#endif
      // We use scatter-gether mode, and the SADDR will be reloaded, but this field still need be set properly.  
      tcd_list[i].SLAST = 0;// -AUDIO_DMA_BUFFER_SIZE; 
      
      tcd_list[i].DOFF = 2;
      tcd_list[i].DADDR = ((uint32_t) (DAC0_BASE_PTR )) /*+ 16*/; // DAC DATA buffer base address
      ///////////////////////////////////////////////////////DAC0_BASE_PTR yifang
#if 0      
      /*tcd_list[i].NBYTES_MLOFFNO = DMA_NBYTES_MLOFFNO_NBYTES(DAC_BUFFER_SIZE);*/
	  tcd_list[i].NBYTES_MLOFFYES = (DMA_NBYTES_MLOFFYES_DMLOE_MASK | 
            DMA_NBYTES_MLOFFYES_MLOFF(-DAC_BUFFER_SIZE) |        // Destination address minus 32 ever minor_loop
            DMA_NBYTES_MLOFFYES_NBYTES(DAC_BUFFER_SIZE));        // 32 bytes per minor-loop
#else
       tcd_list[i].NBYTES_MLOFFNO = DMA_NBYTES_MLOFFNO_NBYTES(DAC_BUFFER_SIZE);     
#endif

      
      tcd_list[i].CSR = (DMA_CSR_ESG_MASK | DMA_CSR_INTMAJOR_MASK);
  }
  
  // Chain the two TDs as a ring
  tcd_list[0].DLAST_SGA = (int32_t)(&tcd_list[1]);
  tcd_list[1].DLAST_SGA = (uint32_t)(&tcd_list[0]);
}

void dma_load_td_chain(uint32_t ch)
{
    DMA_SADDR(ch) = tcd_list->SADDR;
    DMA_SOFF(ch) = tcd_list->SOFF;
    DMA_ATTR(ch) = tcd_list->ATTR;
    DMA_NBYTES_MLOFFYES(ch) = tcd_list->NBYTES_MLOFFYES;
    DMA_SLAST(ch) = tcd_list->SLAST;
  
    DMA_DADDR(ch) = tcd_list->DADDR;
    DMA_DOFF(ch) = tcd_list->DOFF;
    DMA_CITER_ELINKNO(ch) = tcd_list->CITER_ELINKNO;
    DMA_DLAST_SGA(ch) = tcd_list->DLAST_SGA;
    DMA_CSR(ch)   = tcd_list->CSR;
    DMA_BITER_ELINKNO(ch) = tcd_list->BITER_ELINKNO;
}

