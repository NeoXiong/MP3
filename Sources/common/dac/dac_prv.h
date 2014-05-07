#ifndef DAC_PRV_H
#define DAC_PRV_H
#include<mqx.h>
#include<cstdint>

/* Enable global DACx Interrup bit*/
#define VECTOR_DAC0_INT_MASK 2
#define VECTOR_DAC1_INT_MASK 4

/* DAXx registers reset values*/
#define DACx_DAT_RESET 0
#define DACx_SR_RESET 2
#define DACx_C0_RESET 0
#define DACx_C1_RESET 0
#define DACx_C2_RESET 15 //0x0f

/* DACx_C0 bits definition*/ 

#define DAC_DISABLE   0x00
#define DAC_ENABLE    DAC_C0_DACEN_MASK

#define DAC_SEL_VREFO  0x00
#define DAC_SEL_VDDA   DAC_C0_DACRFS_MASK 

#define DAC_SEL_PDB_HW_TRIG  0x00
#define DAC_SEL_SW_TRIG  DAC_C0_DACTRGSEL_MASK 

#define DAC_SW_TRIG_STOP 0x00
#define DAC_SW_TRIG_NEXT  DAC_C0_DACSWTRG_MASK 

#define DAC_HP_MODE  0x00
#define DAC_LP_MODE  DAC_C0_LPEN_MASK 

#define DAC_BFWM_INT_DISABLE  0x00
#define DAC_BFWM_INT_ENABLE   DAC_C0_DACBWIEN_MASK

#define DAC_BFT_PTR_INT_DISABLE 0x00
#define DAC_BFT_PTR_INT_ENABLE DAC_C0_DACBTIEN_MASK

#define DAC_BFB_PTR_INT_DISABLE 0x00
#define DAC_BFB_PTR_INT_ENABLE DAC_C0_DACBBIEN_MASK

/* DACx_C1 bits definition*/ 
#define DAC_DMA_DISABLE  0x00
#define DAC_DMA_ENABLE DAC_C1_DMAEN_MASK 

#define DAC_BFWM_1WORD DAC_C1_DACBFWM(0)  
#define DAC_BFWM_2WORDS DAC_C1_DACBFWM(1) 
#define DAC_BFWM_3WORDS DAC_C1_DACBFWM(2)  
#define DAC_BFWM_4WORDS DAC_C1_DACBFWM(3) 

#define DAC_BF_NORMAL_MODE DAC_C1_DACBFMD(0)
#define DAC_BF_SWING_MODE DAC_C1_DACBFMD(1) 
#define DAC_BF_ONE_TIME_MODE DAC_C1_DACBFMD(2)

#define DAC_BF_DISABLE 0x00
#define DAC_BF_ENABLE DAC_C1_DACBFEN_MASK 

/* DACx_C2 bits definition*/ 
#define DAC_SET_PTR_AT_BF(x) DAC_C2_DACBFRP(x)
#define DAC_SET_PTR_UP_LIMIT(x) DAC_C2_DACBFUP(x)


#define Watermark_One_Word    0
#define Watermark_Two_Words   1
#define Watermark_Three_Words 2
#define Watermark_Four_Words  3

#define Clear_DACBFWMF 0x03
#define Clear_DACBFRPTF 0x05
#define Clear_DACBFRPBF 0x06

///////////////////////////////////////////
#define DEFAULT_AUDIO_SAMPLERATE 44100

#define  DAM16WORD_TWICE
#define  TOP_WM_INTR
#ifndef TOP_WM_INTR
#define  WM_BOT_INTR
#define  WM_WORDS   DAC_BFWM_4WORDS
#define  INTR_MASKS   (DAC_BFB_PTR_INT_ENABLE | DAC_BFWM_INT_ENABLE)
#else
#define  WM_WORDS   DAC_BFWM_3WORDS
#define  INTR_MASKS   (DAC_BFT_PTR_INT_ENABLE | DAC_BFWM_INT_ENABLE)
#endif


#define EDMA_CHANNEL   0  // use channel 0
#define DAC0_DMA_INT    16  // vector num 

#define DAC0_INT    97
#define PDB_INT      88

#define USE_DAC_DMA
//#define USE_PDB_INTR



#define AUDIO_BITS	12
#define AUDIO_NULL 0
#define AUDIO_USB 1
#define AUDIO_BT  2
#define AUDIO_ALARM 3

#define AUDIO_MAX_SOURCE 4

#define PDB_DACINTC0_TOE_MASK                    0x1u
#define DMA_CHANNEL_AUDIO 0
#define DAC_BUFFER_SIZE         0x10 //(0x10 * 2)  
#define AUDIO_DMA_BUFFER_SIZE   0x200
#define DMA_CH0_ISR_NUM         0
#define DMA_ERROR_ISR_NUM       16
#define DMA_REQUEST_DAC0        45
#define DMA_REQUEST_DAC1        46
#define SAMPLE_RATE_44100HZ     44100
#define SAMPLE_RATE_1KHZ        1000
#define DAC0_ISR_NUM            81


typedef struct{
    uint32_t SADDR;                                  /**< TCD Source Address, array offset: 0x1000, array step: 0x20 */
    uint16_t SOFF;                                   /**< TCD Signed Source Address Offset, array offset: 0x1004, array step: 0x20 */
    uint16_t ATTR;                                   /**< TCD Transfer Attributes, array offset: 0x1006, array step: 0x20 */
    union {                                          /* offset: 0x1008, array step: 0x20 */
      uint32_t NBYTES_MLNO;                            /**< TCD Minor Byte Count (Minor Loop Disabled), array offset: 0x1008, array step: 0x20 */
      uint32_t NBYTES_MLOFFNO;                         /**< TCD Signed Minor Loop Offset (Minor Loop Enabled and Offset Disabled), array offset: 0x1008, array step: 0x20 */
      uint32_t NBYTES_MLOFFYES;                        /**< TCD Signed Minor Loop Offset (Minor Loop and Offset Enabled), array offset: 0x1008, array step: 0x20 */
    };
    uint32_t SLAST;                                  /**< TCD Last Source Address Adjustment, array offset: 0x100C, array step: 0x20 */
    uint32_t DADDR;                                  /**< TCD Destination Address, array offset: 0x1010, array step: 0x20 */
    uint16_t DOFF;                                   /**< TCD Signed Destination Address Offset, array offset: 0x1014, array step: 0x20 */
    union {                                          /* offset: 0x1016, array step: 0x20 */
      uint16_t CITER_ELINKYES;                         /**< TCD Current Minor Loop Link, Major Loop Count (Channel Linking Enabled), array offset: 0x1016, array step: 0x20 */
      uint16_t CITER_ELINKNO;                          /**< TCD Current Minor Loop Link, Major Loop Count (Channel Linking Disabled), array offset: 0x1016, array step: 0x20 */
    };
    uint32_t DLAST_SGA;                              /**< TCD Last Destination Address Adjustment/Scatter Gather Address, array offset: 0x1018, array step: 0x20 */
    uint16_t CSR;                                    /**< TCD Control and Status, array offset: 0x101C, array step: 0x20 */
    union {                                          /* offset: 0x101E, array step: 0x20 */
      uint16_t BITER_ELINKNO;                          /**< TCD Beginning Minor Loop Link, Major Loop Count (Channel Linking Disabled), array offset: 0x101E, array step: 0x20 */
      uint16_t BITER_ELINKYES;                         /**< TCD Beginning Minor Loop Link, Major Loop Count (Channel Linking Enabled), array offset: 0x101E, array step: 0x20 */
    };
} DMA_TCD_t, *DMA_TCD_p;



#ifdef DAC_C
int32_t dma_buffer_full_num = 2;
#else
extern int32_t dma_buffer_full_num;

#endif

extern int32_t g_sampleConsumed;

void pdb_stop();
void pdb_start();
void pdb_init(int32_t source, uint32_t samplerate);

void dac0_dma_init(void);
void dma_start_channel(uint32_t ch);
void dma_stop_channel(uint32_t ch);
void DAC0_DMA_isr(void * handle);
void dma_update_tcd_list(uint8_t listNum, uint32_t saddr, uint32_t citer, uint32_t biter);
void dma_load_td_chain(uint32_t ch);

#endif
