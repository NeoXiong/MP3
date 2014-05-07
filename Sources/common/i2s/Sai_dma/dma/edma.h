#ifndef _mqx_edma_h_
#define _mqx_edma_h_
//#define EDMA_CHANNEL_NUMBER 16



#define EDMA_VERIFY_CHANNEL(chan)	((g_edma->chan_ve[chan/32] >> (chan%32))&0x1)
#define EDMA_SET_CHANNEL(chan)	((g_edma->chan_ve[chan/32]) |= (1 << (chan%32)))
#define EDMA_CLR_CHANNEL(chan)	((g_edma->chan_ve[chan/32]) &= ~(1 << (chan%32)))

#define DMA_GET_CITTER(base, chan) (DMA_CITER_ELINKNO_REG(base, chan) & DMA_CITER_ELINKNO_CITER_MASK)
#define DMA_GET_BITTER(base, chan) (DMA_BITER_ELINKNO_REG(base, chan) & DMA_BITER_ELINKNO_BITER_MASK)

#define MQX_EDMA_TCD_ALIGNMENT 32

extern DMA_MemMapPtr edma_base[];
extern DMAMUX_MemMapPtr dmamux_base[];


#define EDMA_TOTAL_CHANNELS (EDMA_MODULES * EDMA_CHANNELS)
#define EDMA_MODULES  ELEMENTS_OF(edma_base)
#define DMAMUX_MODULES  ELEMENTS_OF(dmamux_base)
#define EDMA_CHANNELS ELEMENTS_OF(edma_base[0]->TCD)
#define DMAMUX_CHANNLES ELEMENTS_OF(dmamux_base[0]->CHCFG)



typedef void (EDMA_CALLBACK)(pointer parameter, uint_8 chan);
typedef void (EDMA_ERR_CALLBACK)(pointer parameter, uint_32 err);

/*hardware request should be ported to chip relevant files*/
typedef enum mqx_edma_hardware_request{

	MQX_EDMA_HARD_REQ_DISABLED = 0,
	MQX_EDMA_HARD_REQ_RESERVED_1,
	MQX_EDMA_HARD_REQ_UART0_RX,
	MQX_EDMA_HARD_REQ_UART0_TX,
	MQX_EDMA_HARD_REQ_UART1_RX,
	MQX_EDMA_HARD_REQ_UART1_TX,
	MQX_EDMA_HARD_REQ_UART2_RX,
	MQX_EDMA_HARD_REQ_UART2_TX,
	MQX_EDMA_HARD_REQ_UART3_RX,
	MQX_EDMA_HARD_REQ_UART3_TX,
	MQX_EDMA_HARD_REQ_UART4_RX,
	MQX_EDMA_HARD_REQ_UART4_TX,
    MQX_EDMA_HARD_REQ_UART5_RX,
    MQX_EDMA_HARD_REQ_UART5_TX,
    MQX_EDMA_HARD_REQ_I2S0_RX,
    MQX_EDMA_HARD_REQ_I2S0_TX,
    MQX_EDMA_HARD_REQ_SPI0_RX,
    MQX_EDMA_HARD_REQ_SPI0_TX,
    MQX_EDMA_HARD_REQ_SPI1_RX,
    MQX_EDMA_HARD_REQ_SPI1_TX,
    MQX_EDMA_HARD_REQ_SPI2_RX,
    MQX_EDMA_HARD_REQ_SPI2_TX,
    MQX_EDMA_HARD_REQ_I2C0_RX,
    MQX_EDMA_HARD_REQ_I2C0_TX,
    MQX_EDMA_HARD_REQ_FTM0_CH0,
    MQX_EDMA_HARD_REQ_FTM0_CH1,
    MQX_EDMA_HARD_REQ_FTM0_CH2,
    MQX_EDMA_HARD_REQ_FTM0_CH3,
    MQX_EDMA_HARD_REQ_FTM0_CH4,
    MQX_EDMA_HARD_REQ_FTM0_CH5,
    MQX_EDMA_HARD_REQ_FTM0_CH6,
    MQX_EDMA_HARD_REQ_FTM0_CH7,
    MQX_EDMA_HARD_REQ_FTM1_CH0,
    MQX_EDMA_HARD_REQ_FTM1_CH1,
    MQX_EDMA_HARD_REQ_FTM2_CH0,
    MQX_EDMA_HARD_REQ_FTM2_CH1,
    MQX_EDMA_HARD_REQ_1588_TIMER0,
    MQX_EDMA_HARD_REQ_1588_TIMER1,
    MQX_EDMA_HARD_REQ_1588_TIMER2,
    MQX_EDMA_HARD_REQ_1588_TIMER3,
    MQX_EDMA_HARD_REQ_ADC0,
    MQX_EDMA_HARD_REQ_ADC1,
    MQX_EDMA_HARD_REQ_CMP0,
    MQX_EDMA_HARD_REQ_CMP1,
    MQX_EDMA_HARD_REQ_CMP2,
    MQX_EDMA_HARD_REQ_DAC0,
    MQX_EDMA_HARD_REQ_RESERVED_46,
    MQX_EDMA_HARD_REQ_CMT,
    MQX_EDMA_HARD_REQ_PDB,
    MQX_EDMA_HARD_REQ_PORTA,
    MQX_EDMA_HARD_REQ_PORTB,
    MQX_EDMA_HARD_REQ_PORTC,
    MQX_EDMA_HARD_REQ_PORTD,
    MQX_EDMA_HARD_REQ_PORTE,
    MQX_EDMA_HARD_REQ_DMAMUX_ALWAYS_ON_54,
    MQX_EDMA_HARD_REQ_DMAMUX_ALWAYS_ON_55,
    MQX_EDMA_HARD_REQ_DMAMUX_ALWAYS_ON_56,
    MQX_EDMA_HARD_REQ_DMAMUX_ALWAYS_ON_57,
    MQX_EDMA_HARD_REQ_DMAMUX_ALWAYS_ON_58,
    MQX_EDMA_HARD_REQ_DMAMUX_ALWAYS_ON_59,
    MQX_EDMA_HARD_REQ_DMAMUX_ALWAYS_ON_60,
    MQX_EDMA_HARD_REQ_DMAMUX_ALWAYS_ON_61,
    MQX_EDMA_HARD_REQ_DMAMUX_ALWAYS_ON_62,
    MQX_EDMA_HARD_REQ_DMAMUX_ALWAYS_ON_63
}MQX_EDMA_HARDWARE_REQUEST, _PTR_ MQX_EDMA_HARDWARE_REQUEST_PTR;



typedef enum mqx_edma_type{
	MQX_EDMA_INVALID_TYPE,
	MQX_EDMA_PERI_TO_MEM,
	MQX_EDMA_MEM_TO_PERI,
	MQX_EDMA_MEM_TO_MEM
}MQX_EDMA_TYPE, _PTR_ MQX_EDMA_TYPE_PTR;

typedef enum mqx_edma_err_code {
	MQX_EDMA_ERR_TRANSFER_CANCELLED = 16,
	MQX_EDMA_ERR_CHAN_PRI_ERR = 14,
	MQX_EDMA_ERR_SRC_ADDR_ERR = 7,
	MQX_EDMA_ERR_SRC_OFF_ERR = 6,
	MQX_EDMA_ERR_DST_ADDR_ERR = 5,
	MQX_EDMA_ERR_DST_OFF_ERR = 4,
	MQX_EDMA_ERR_NBYTES_CITTER_ERR = 3,
	MQX_EDMA_ERR_SCATTER_GATHER_ERR = 2,
	MQX_EDMA_ERR_SRC_BUS_ERR = 1,
	MQX_EDMA_ERR_DST_BUS_ERR = 0,
	MQX_EDMA_ERR_NONE = 0xff
}MQX_EDMA_ERR_CODE, _PTR_ MQX_EDMA_ERR_CODE_PTR;


typedef struct mqx_edma_chan_context {
	uint_8 nents;
	uint_8 channel;
	MQX_EDMA_TYPE type;
	uint_32 *tcd_buf;
	EDMA_CALLBACK *callback;
	void *parameter;
	EDMA_ERR_CALLBACK *err_callback;
	void *err_parameter;
	MQX_EDMA_ERR_CODE errsta;
}MQX_EDMA_CHAN_CONTEXT, _PTR_ MQX_EDMA_CHAN_CONTEXT_PTR;

typedef struct mqx_edma_struct {
	/* reserved space for 128 EDMA channel */
    uint_32 chan_ve[4];
    LWSEM_STRUCT  chan_sem;
    MQX_EDMA_CHAN_CONTEXT_PTR *context;
}MQX_EDMA_STRUCT, _PTR_ MQX_EDMA_STRUCT_PTR;

typedef struct mqx_edma_tcd {
    uint_32 SADDR;                                  /**< TCD Source Address, array offset: 0x1000, array step: 0x20 */
    uint_16 SOFF;                                   /**< TCD Signed Source Address Offset, array offset: 0x1004, array step: 0x20 */
    uint_16 ATTR;                                   /**< TCD Transfer Attributes, array offset: 0x1006, array step: 0x20 */
    union {                                          /* offset: 0x1008, array step: 0x20 */
      uint_32 NBYTES_MLNO;                            /**< TCD Minor Byte Count (Minor Loop Disabled), array offset: 0x1008, array step: 0x20 */
      uint_32 NBYTES_MLOFFNO;                         /**< TCD Signed Minor Loop Offset (Minor Loop Enabled and Offset Disabled), array offset: 0x1008, array step: 0x20 */
      uint_32 NBYTES_MLOFFYES;                        /**< TCD Signed Minor Loop Offset (Minor Loop and Offset Enabled), array offset: 0x1008, array step: 0x20 */
    };
    uint_32 SLAST;                                  /**< TCD Last Source Address Adjustment, array offset: 0x100C, array step: 0x20 */
    uint_32 DADDR;                                  /**< TCD Destination Address, array offset: 0x1010, array step: 0x20 */
    uint_16 DOFF;                                   /**< TCD Signed Destination Address Offset, array offset: 0x1014, array step: 0x20 */
    union {                                          /* offset: 0x1016, array step: 0x20 */
      uint_16 CITER_ELINKNO;                          /**< TCD Current Minor Loop Link, Major Loop Count (Channel Linking Disabled), array offset: 0x1016, array step: 0x20 */
      uint_16 CITER_ELINKYES;                         /**< TCD Current Minor Loop Link, Major Loop Count (Channel Linking Enabled), array offset: 0x1016, array step: 0x20 */
    };
    uint_32 DLAST_SGA;                              /**< TCD Last Destination Address Adjustment/Scatter Gather Address, array offset: 0x1018, array step: 0x20 */
    uint_16 CSR;                                    /**< TCD Control and Status, array offset: 0x101C, array step: 0x20 */
    union {                                          /* offset: 0x101E, array step: 0x20 */
      uint_16 BITER_ELINKNO;                          /**< TCD Beginning Minor Loop Link, Major Loop Count (Channel Linking Disabled), array offset: 0x101E, array step: 0x20 */
      uint_16 BITER_ELINKYES;                         /**< TCD Beginning Minor Loop Link, Major Loop Count (Channel Linking Enabled), array offset: 0x101E, array step: 0x20 */
    };
}MQX_EDMA_TCD, _PTR_ MQX_EDMA_TCD_PTR;

typedef struct mqx_edma_scatter_struct {
	uint_32 src_addr;
	uint_32 dst_addr;
	uint_32 length;
}MQX_EDMA_SCATTER_STRUCT, _PTR_ MQX_EDMA_SCATTER_STRUCT_PTR;

typedef enum mqx_edma_tranfer_size {
    MQX_EDMA_TRANS_SIZE_8_BITS = 0,
    MQX_EDMA_TRANS_SIZE_16_BITS = 1,
    MQX_EDMA_TRANS_SIZE_32_BITS = 2,
    MQX_EDMA_TRANS_SIZE_16_BYTES = 4,
    MQX_EDMA_TRANS_SIZE_32_BYTES = 5
}MQX_EDMA_TRANFER_SIZE, _PTR_ MQX_EDMA_TRANFER_SIZE_PTR;


extern _mqx_uint dma_init(void);
extern _mqx_uint dma_deinit(void);
extern _mqx_uint edma_verify_chan(uint_8 channel);
extern void edma_free_chan(uint_8 channel);

extern _mqx_uint edma_config_scatter(uint_8 channel,MQX_EDMA_TYPE type,MQX_EDMA_HARDWARE_REQUEST request,MQX_EDMA_SCATTER_STRUCT_PTR scatter_list,MQX_EDMA_TRANFER_SIZE trans_size,uint_8 nents, uint_32 burst_size);
extern _mqx_uint edma_config_circle(uint_8 channel,MQX_EDMA_TYPE type,MQX_EDMA_HARDWARE_REQUEST request,uint_32 src_addr, uint_32 dst_addr,MQX_EDMA_TRANFER_SIZE trans_size,uint_32 burst_size, uint_32 size);
extern _mqx_uint edma_request_channel(uint_8 channel, EDMA_CALLBACK cback, void *para, uint_8 pri, EDMA_ERR_CALLBACK err_cback, void *err_para );
extern _mqx_uint edma_start(uint_8 channel);
extern _mqx_uint edma_close_chan(uint_8 channel);
extern uint_32 edma_force_stop(uint_8 channel);
extern uint_32 edma_get_status(uint_8 channel, uint_32 *num);
extern void edma_software_trigger(uint_8 channel);



extern uint_32 _bsp_get_edma_done_vector(uint_8 channel);
extern uint_32 _bsp_get_edma_error_vector(uint_8 channel);
extern DMAMUX_MemMapPtr _bsp_get_DMAMUX_base_address(uint_8 channel);
extern DMA_MemMapPtr _bsp_get_DMACTRL_base_address(uint_8 channel);
extern uint_8 _bsp_get_DMAMUX_chan(uint_8 channel);
extern uint_8 _bsp_get_DMACTRL_chan(uint_8 channel);
extern void _bsp_enable_dma(uint_8 module);


#endif
