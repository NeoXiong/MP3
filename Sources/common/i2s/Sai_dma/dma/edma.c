/**HEADER********************************************************************
* 
* Copyright (c) 2009 Freescale Semiconductor;
* All Rights Reserved                       
*
*************************************************************************** 
*
* THIS SOFTWARE IS PROVIDED BY FREESCALE "AS IS" AND ANY EXPRESSED OR 
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES 
* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  
* IN NO EVENT SHALL FREESCALE OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
* IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF 
* THE POSSIBILITY OF SUCH DAMAGE.
*
**************************************************************************
*
* $FileName: edma.c$
* $Version:  4.0.0$
* $Date    : 3-19-2013$
*
* Comments:
*
*   This file contains implementation of eDMA driver provided to other driver 
*
*END************************************************************************/


#include <mqx.h>

#include <bsp.h>

#include "edma.h"


#define DMA_BASE_PTRS 	{(DMA_MemMapPtr)0x40008000}

#if (MQX_CPU == PSP_CPU_MK70F120M)
	#define DMAMUX_BASE_PTRS	{(DMAMUX_MemMapPtr)0x40021000, (DMAMUX_MemMapPtr)0x40022000}
	
#else
#if MQX_CPU == PSP_CPU_MK60D100M 
	#define DMAMUX_BASE_PTRS	{(DMAMUX_MemMapPtr)0x40021000}
#endif
#endif



MQX_EDMA_STRUCT_PTR g_edma = NULL;

/*
 * Peripheral base pointers
 */

DMA_MemMapPtr edma_base[] = DMA_BASE_PTRS;
DMAMUX_MemMapPtr dmamux_base[] = DMAMUX_BASE_PTRS;

static uint_8 edma_trans_size[6] = {1, 2, 4, 0, 16, 32};

static void edma_interrupt_handler(pointer parameter);
static void edma_err_interrupt_handler(pointer parameter);

_mqx_uint dma_init() 
{
    uint_8 i, j;
    uint_32 result;
    uint_32 err_int;
	DMA_MemMapPtr base;
	DMAMUX_MemMapPtr base_mux;
	char_ptr	identifier = "edma:";
	
    /* Create an gloable struct to protect*/  
    g_edma = _mem_alloc_system_zero((_mem_size)sizeof(MQX_EDMA_STRUCT));
    if (g_edma == NULL)
    {
        return (MQX_OUT_OF_MEMORY);
    }

	/* enable DMAMUX clock */
	//TODO:
    /* create an lwsem to protect the channel resource */ 
    _lwsem_create((LWSEM_STRUCT_PTR)&g_edma->chan_sem, 1);

    /* Init Channel resource bit map */
    _lwsem_wait((LWSEM_STRUCT_PTR)&g_edma->chan_sem);

	g_edma->context = _mem_alloc_system_zero((_mem_size)(sizeof(MQX_EDMA_CHAN_CONTEXT_PTR) * EDMA_TOTAL_CHANNELS));
    
    for (i=0; i < EDMA_TOTAL_CHANNELS; i++)
    {
        EDMA_CLR_CHANNEL(i);
        g_edma->context[i] = NULL;
    }
    _lwsem_post((LWSEM_STRUCT_PTR)&g_edma->chan_sem);

	/* Do some installation */
	for (i = 0; i< EDMA_MODULES; i++)\
	{
		_bsp_enable_dma(i);
	
		/* Get base address of module's DMA controller registers */
		base = _bsp_get_DMACTRL_base_address(i * EDMA_CHANNELS + 1);
		/* Cancel current DMA transfer */
		DMA_ERQ_REG(base) = 0;
		DMA_CR_REG(base) |= DMA_CR_CX_MASK | DMA_CR_HALT_MASK;
		while (DMA_CR_REG(base) & DMA_CR_CX_MASK) {};
		/* Clear halt bit and set halt of error bit */
		DMA_CR_REG(base) = (DMA_CR_REG(base) & ~DMA_CR_HALT_MASK) | DMA_CR_HOE_MASK;		
		/* Clear all error bit */
		DMA_CERR_REG(base) |= 0x40;
		/* Init DMA MUX */
		for (j = i * EDMA_CHANNELS; j < EDMA_CHANNELS * (i + 1); j++)
		{
			base_mux = _bsp_get_DMAMUX_base_address(j);
			DMAMUX_CHCFG_REG(base_mux, _bsp_get_DMAMUX_chan(j));
		}
		
		/* register error interrupt */
		err_int = _bsp_get_edma_error_vector(i*EDMA_CHANNELS);
		_int_install_isr(err_int,edma_err_interrupt_handler, (void *)NULL);
		_bsp_int_init(err_int, 3, 0, TRUE);
#if PSP_MQX_CPU_IS_ARM_CORTEX_A5
		_int_install_isr(_bsp_get_edma_done_vector(i * EDMA_CHANNELS + 1), edma_interrupt_handler, (void *)NULL);
		_bsp_int_init(_bsp_get_edma_done_vector(i * EDMA_CHANNELS + 1), 3, 0, TRUE);
#endif
		/* Enable error interrupt */
		//DMA_SEEI_REG(base) |= 0x40;
	}

	result = _io_dev_install(identifier,
	  NULL, NULL, NULL, NULL,NULL, (pointer)g_edma); 
	return result;
}

_mqx_uint dma_deinit(void)
{
	uint_8 i;
	uint_32 err_int;

	for (i=0; i<EDMA_CHANNELS * EDMA_MODULES; i++)
		edma_close_chan(i);
	
	_lwsem_destroy((LWSEM_STRUCT_PTR)&g_edma->chan_sem);

	for (i = 0; i < EDMA_MODULES; i++)
	{
		err_int = _bsp_get_edma_error_vector(i * EDMA_CHANNELS);
		_bsp_int_disable(err_int);
#if PSP_MQX_CPU_IS_ARM_CORTEX_A5
		_bsp_int_disable(_bsp_get_edma_done_vector(i * EDMA_CHANNELS + 1));	
#endif		
	}
	_mem_free(g_edma);

	return MQX_OK;
}
_mqx_uint edma_verify_chan(uint_8 channel)
{
	

	_lwsem_wait((LWSEM_STRUCT_PTR)&g_edma->chan_sem);	
	if (!EDMA_VERIFY_CHANNEL(channel))
	{
		_lwsem_post((LWSEM_STRUCT_PTR)&g_edma->chan_sem);
		return MQX_INVALID_PARAMETER;
	}	
	_lwsem_post((LWSEM_STRUCT_PTR)&g_edma->chan_sem);

	return MQX_OK;
}


/* request a channel for EDMA */
_mqx_uint edma_request_channel(uint_8 channel, EDMA_CALLBACK *cback, void *para, uint_8 pri, EDMA_ERR_CALLBACK *err_cback, void *err_para )
{
	_lwsem_wait((LWSEM_STRUCT_PTR)&g_edma->chan_sem);
	if (EDMA_VERIFY_CHANNEL(channel))
		return MQX_INVALID_PARAMETER;
	EDMA_SET_CHANNEL(channel);

	g_edma->context[channel] = _mem_alloc_system_zero((_mem_size)sizeof(MQX_EDMA_CHAN_CONTEXT));
	if (!g_edma->context[channel])
		return MQX_OUT_OF_MEMORY;

	g_edma->context[channel]->callback = cback;
	g_edma->context[channel]->parameter = para;
	g_edma->context[channel]->err_callback = err_cback;
	g_edma->context[channel]->err_parameter = err_para;
	g_edma->context[channel]->errsta = MQX_EDMA_ERR_NONE;
	g_edma->context[channel]->channel = channel;
#if PSP_MQX_CPU_IS_ARM_CORTEX_M4 || PSP_MQX_CPU_IS_ARM_CORTEX_M0P
	_int_install_isr(_bsp_get_edma_done_vector(channel), edma_interrupt_handler, (void *)g_edma->context[channel]);
	_bsp_int_init(_bsp_get_edma_done_vector(channel), pri, 0, TRUE);
#endif

	_lwsem_post((LWSEM_STRUCT_PTR)&g_edma->chan_sem);

	return MQX_OK;
}

void edma_free_chan(uint_8 channel)
{
	MQX_EDMA_CHAN_CONTEXT_PTR chan_info = g_edma->context[channel];

	_lwsem_wait((LWSEM_STRUCT_PTR)&g_edma->chan_sem);
	EDMA_CLR_CHANNEL(channel);

	if (chan_info)
	{
		if (chan_info->tcd_buf)
			_mem_free(chan_info->tcd_buf);
		_mem_free(chan_info);
		chan_info = NULL;
	}
	
	_lwsem_post((LWSEM_STRUCT_PTR)&g_edma->chan_sem);
}

_mqx_uint edma_config_scatter(
	uint_8 channel,
	MQX_EDMA_TYPE type, MQX_EDMA_HARDWARE_REQUEST request,
	MQX_EDMA_SCATTER_STRUCT_PTR scatter_list,
	MQX_EDMA_TRANFER_SIZE trans_size,
	uint_8 nents, uint_32 burst_size
	)
{
	uint_8 i;
	MQX_EDMA_TCD_PTR p_tcd;
	DMA_MemMapPtr base;
	DMAMUX_MemMapPtr base_mux;
	uint_32 chan, chan_mux;
	uint_32 test_reg;

	/* check if the channel is allocated */
	if (edma_verify_chan(channel) != MQX_OK)
		return MQX_INVALID_PARAMETER;

	base = _bsp_get_DMACTRL_base_address(channel);
	base_mux = _bsp_get_DMAMUX_base_address(channel);
	chan = _bsp_get_DMACTRL_chan(channel);
	chan_mux = _bsp_get_DMAMUX_chan(channel);

	/* the buffer size should be multi times of burst size*/

	if (burst_size % edma_trans_size[trans_size])
		return MQX_INVALID_PARAMETER;
	
	for (i=0; i<nents; i++)
	{
		if (scatter_list[i].length % burst_size)
			return MQX_INVALID_PARAMETER;
	}

	/* allocate space for edma scatter lsit */
	if ((g_edma->context[channel]->tcd_buf) && (g_edma->context[channel]->nents < (nents - 1)))
	{
		_mem_free(g_edma->context[channel]->tcd_buf);
		g_edma->context[channel]->tcd_buf = NULL;
	}

	if ((!g_edma->context[channel]->tcd_buf)&&(nents > 1))
	{
		g_edma->context[channel]->tcd_buf =
#if MQX_USE_UNCACHED_MEM && PSP_HAS_DATA_CACHE		
		_mem_alloc_align_uncached((nents - 1) * sizeof(MQX_EDMA_TCD), MQX_EDMA_TCD_ALIGNMENT);
#else
		_mem_alloc_align((nents - 1) * sizeof(MQX_EDMA_TCD), MQX_EDMA_TCD_ALIGNMENT);
#endif
	}
	g_edma->context[channel]->nents = nents - 1;
	/* save channel type into context */
	g_edma->context[channel]->type = type;

	/* config the first descriptor */
	/**********************/
	/* config intinial source and dest address*/
    DMA_SADDR_REG(base, chan) = scatter_list[0].src_addr;
    DMA_DADDR_REG(base, chan) = scatter_list[0].dst_addr;
	/* config the transfer size for source and destination*/
    DMA_ATTR_REG(base, chan) = DMA_ATTR_SSIZE(trans_size) | DMA_ATTR_DSIZE(trans_size); 

	switch(type) {
	case MQX_EDMA_MEM_TO_MEM:
		DMA_SOFF_REG(base, chan) = edma_trans_size[trans_size];
    	DMA_DOFF_REG(base, chan) = edma_trans_size[trans_size];
    	DMA_SLAST_REG(base, chan) = 0;
		break;
	case MQX_EDMA_PERI_TO_MEM:
		DMA_SOFF_REG(base, chan) = 0;
    	DMA_DOFF_REG(base, chan) = edma_trans_size[trans_size];
    	DMA_SLAST_REG(base, chan) = 0;
		break;
	case MQX_EDMA_MEM_TO_PERI:
		DMA_SOFF_REG(base, chan) = edma_trans_size[trans_size];
    	DMA_DOFF_REG(base, chan) = 0;
    	DMA_SLAST_REG(base, chan) = 0;
		break;
	default:
		goto exit;
	}

	/* config the nbytes, only minor loop disable offset is support*/
	DMA_NBYTES_MLNO_REG(base, chan) = burst_size;
	test_reg = DMA_NBYTES_MLNO_REG(base, chan);
	DMA_CITER_ELINKNO_REG(base, chan) = DMA_CITER_ELINKNO_CITER(scatter_list[0].length / burst_size);
    DMA_BITER_ELINKNO_REG(base, chan) = DMA_BITER_ELINKNO_BITER(scatter_list[0].length / burst_size);

	if (nents > 1)
	{
		/* if it is not the only nent, enable scatter list and don't enable interrupt */
		DMA_CSR_REG(base, chan) = DMA_CSR_ESG_MASK;
		DMA_DLAST_SGA_REG(base, chan) = DMA_DLAST_SGA_DLASTSGA(g_edma->context[channel]->tcd_buf);
	}
	else
	{
		/* if it is the only nent, set the interrupt */
		DMA_CSR_REG(base, chan) = DMA_CSR_DREQ_MASK | DMA_CSR_INTMAJOR_MASK;
		DMA_DLAST_SGA_REG(base, chan) = 0;
	}

	/* config other descriptor */
	for (i=1; i<nents; i++)
	{
		p_tcd = (MQX_EDMA_TCD_PTR)(g_edma->context[channel]->tcd_buf + (i - 1) * 8);
		p_tcd->ATTR = DMA_ATTR_REG(base, chan);
		p_tcd->BITER_ELINKNO = DMA_BITER_ELINKNO_BITER(scatter_list[i].length/burst_size);
		p_tcd->CITER_ELINKNO = DMA_CITER_ELINKNO_CITER(scatter_list[i].length/burst_size);
		p_tcd->CSR = DMA_CSR_REG(base, chan);
		p_tcd->SADDR = scatter_list[i].src_addr;
		p_tcd->DADDR = scatter_list[i].dst_addr;
		p_tcd->NBYTES_MLNO = DMA_NBYTES_MLNO_REG(base, chan);
		p_tcd->SLAST = DMA_SLAST_REG(base, chan);
		p_tcd->DOFF = DMA_DOFF_REG(base, chan);
		p_tcd->SOFF = DMA_SOFF_REG(base, chan);
		/* config the DLA LAST and interrupt setting*/
		if (i != (nents - 1))
		{
			p_tcd->DLAST_SGA = DMA_DLAST_SGA_DLASTSGA((uint_32)g_edma->context[channel]->tcd_buf + (i + 1) * 32);
		}
		else
		{
			p_tcd->DLAST_SGA = 0;
			p_tcd->CSR= DMA_CSR_DREQ_MASK | DMA_CSR_INTMAJOR_MASK;
		}
	}

	/*Config the DMA request and set DMAMUX */
	DMAMUX_CHCFG_REG(base_mux, chan_mux) = 0;
	DMAMUX_CHCFG_REG(base_mux, chan_mux) = DMAMUX_CHCFG_ENBL_MASK | request;
	
	return MQX_OK;

exit:
	_mem_free(g_edma->context[channel]->tcd_buf);
	g_edma->context[channel]->tcd_buf = NULL;

	return MQX_INVALID_PARAMETER;

}

_mqx_uint edma_config_circle(
	uint_8 channel,
	MQX_EDMA_TYPE type, 
	MQX_EDMA_HARDWARE_REQUEST request,
	uint_32 src_addr, uint_32 dst_addr,
	MQX_EDMA_TRANFER_SIZE trans_size,uint_32 burst_size,
	uint_32 size
	)
{
	DMA_MemMapPtr base;
	DMAMUX_MemMapPtr base_mux;
	uint_32 chan, chan_mux;

	/* check if the channel is allocated */
	if (edma_verify_chan(channel) != MQX_OK)
		return MQX_INVALID_PARAMETER;

	base = _bsp_get_DMACTRL_base_address(channel);
	base_mux = _bsp_get_DMAMUX_base_address(channel);
	chan = _bsp_get_DMACTRL_chan(channel);
	chan_mux = _bsp_get_DMAMUX_chan(channel);

	/* the buffer size should be multi times of burst size*/
	if (burst_size % edma_trans_size[trans_size])
		return MQX_INVALID_PARAMETER;
	if (size % burst_size)
		return MQX_INVALID_PARAMETER;

	g_edma->context[channel]->tcd_buf = NULL;
	g_edma->context[channel]->nents = 0;

	/* save channel type into context */
	g_edma->context[channel]->type = type;

	/**********************/
	/* config intinial source and dest address*/
    DMA_SADDR_REG(base, chan) = src_addr;
    DMA_DADDR_REG(base, chan) = dst_addr;
	/* config the transfer size for source and destination*/
    DMA_ATTR_REG(base, chan) = DMA_ATTR_SSIZE(trans_size) | DMA_ATTR_DSIZE(trans_size); 

	switch(type) {
	case MQX_EDMA_MEM_TO_MEM:
		DMA_SOFF_REG(base, chan) = edma_trans_size[trans_size];
    	DMA_DOFF_REG(base, chan) = edma_trans_size[trans_size];
    	DMA_SLAST_REG(base, chan) = -(size);
    	DMA_DLAST_SGA_REG(base, chan) = -(size);
		break;
	case MQX_EDMA_PERI_TO_MEM:
		DMA_SOFF_REG(base, chan) = 0;
    	DMA_DOFF_REG(base, chan) = edma_trans_size[trans_size];
    	DMA_SLAST_REG(base, chan) = 0;
    	DMA_DLAST_SGA_REG(base, chan) = -(size);
		break;
	case MQX_EDMA_MEM_TO_PERI:
		DMA_SOFF_REG(base, chan) = edma_trans_size[trans_size];
    	DMA_DOFF_REG(base, chan) = 0;
    	DMA_SLAST_REG(base, chan) = -(size);
    	DMA_DLAST_SGA_REG(base, chan) = 0;
		break;
	default:
		return MQX_INVALID_PARAMETER;
	}

	/* config the nbytes, only minor loop disable offset is support*/
	DMA_NBYTES_MLNO_REG(base, chan) = burst_size;
	DMA_CITER_ELINKNO_REG(base, chan) = DMA_CITER_ELINKNO_CITER(size/burst_size);
    DMA_BITER_ELINKNO_REG(base, chan) = DMA_BITER_ELINKNO_BITER(size/burst_size);

	DMA_CSR_REG(base, chan) = DMA_CSR_INTHALF_MASK | DMA_CSR_INTMAJOR_MASK;	

	/*Config the DMA request and set DMAMUX */
	DMAMUX_CHCFG_REG(base_mux, chan_mux) = 0;
	DMAMUX_CHCFG_REG(base_mux, chan_mux) = DMAMUX_CHCFG_ENBL_MASK | request;

	return MQX_OK;

}

_mqx_uint edma_start(uint_8 channel)
{
	DMA_MemMapPtr base;

	uint_32 chan;

	/* check if the channel is allocated */
	if (edma_verify_chan(channel) != MQX_OK)
		return MQX_INVALID_PARAMETER;

	base = _bsp_get_DMACTRL_base_address(channel);
	chan = _bsp_get_DMACTRL_chan(channel);

	/* Enable interrupt */
	DMA_SERQ_REG(base) = chan;

	return MQX_OK;
}

_mqx_uint edma_close_chan(uint_8 channel)
{
	DMA_MemMapPtr base;
	DMAMUX_MemMapPtr base_mux;
	uint_32 chan, chan_mux;

	if (edma_verify_chan(channel))
		return MQX_OK;

	base = _bsp_get_DMACTRL_base_address(channel);
	base_mux = _bsp_get_DMAMUX_base_address(channel);
	chan = _bsp_get_DMACTRL_chan(channel);
	chan_mux = _bsp_get_DMAMUX_chan(channel);

	/*Disable the channel and DMA MUX*/
	edma_force_stop(channel);
#if PSP_MQX_CPU_IS_ARM_CORTEX_M4 || PSP_MQX_CPU_IS_ARM_CORTEX_M0P
	_bsp_int_disable(_bsp_get_edma_done_vector(channel));
#endif
	DMAMUX_CHCFG_REG(base_mux, chan_mux) = 0;
	if (g_edma->context[channel]->tcd_buf)
		_mem_free(g_edma->context[channel]->tcd_buf);
	_mem_free(g_edma->context[channel]);

	_lwsem_wait((LWSEM_STRUCT_PTR)&g_edma->chan_sem);
	EDMA_CLR_CHANNEL(channel);
	_lwsem_post((LWSEM_STRUCT_PTR)&g_edma->chan_sem);
	
	return MQX_OK;
}

uint_32 edma_force_stop(uint_8 channel)
{
	uint_32 size;
	DMA_MemMapPtr base;
	uint_32 chan;

	base = _bsp_get_DMACTRL_base_address(channel);
	chan = _bsp_get_DMACTRL_chan(channel);
	
	/*forcely disable the channel */
	do {
		DMA_CERQ_REG(base) = chan;
	} while(DMA_CSR_REG(base, chan) & DMA_CSR_ACTIVE_MASK);

	edma_get_status(channel, &size);
	return size;
}

uint_32 edma_get_status(uint_8 channel, uint_32 *num)
{
	DMA_MemMapPtr base;
	uint_32 chan;

	base = _bsp_get_DMACTRL_base_address(channel);
	chan = _bsp_get_DMACTRL_chan(channel);

	/*wait until the channel is not running */
	while (DMA_CSR_REG(base, chan) & DMA_CSR_ACTIVE_MASK)	{};
	if (DMA_CSR_REG(base, chan) & DMA_CSR_DONE_MASK)
	{
		*num = DMA_NBYTES_MLNO_REG(base, chan) * DMA_GET_BITTER(base, chan);
		return TRUE;
	}
	else
	{
		*num = DMA_NBYTES_MLNO_REG(base, chan) * (DMA_GET_BITTER(base, chan) - DMA_GET_CITTER(base, chan));
		return FALSE;
	}
}

void edma_software_trigger(uint_8 channel)
{
	DMA_MemMapPtr base;
	uint_32 chan;

	base = _bsp_get_DMACTRL_base_address(channel);
	chan = _bsp_get_DMACTRL_chan(channel);

	DMA_SSRT_REG(base) = chan;
}

#if PSP_MQX_CPU_IS_ARM_CORTEX_A5
static void edma_interrupt_handler(pointer parameter)
{
	
	MQX_EDMA_CHAN_CONTEXT_PTR chan_info;
	DMA_MemMapPtr base;
	uint_32 interrupt, i, j;

	for (i=0; i<EDMA_MODULES; i++)
	{
		base = _bsp_get_DMACTRL_base_address(i * EDMA_CHANNELS);
		interrupt = DMA_INT_REG(base);

		if (interrupt == 0)
			continue;

		for (j = 0; j < EDMA_CHANNELS; j++)
		{
			if (interrupt & (1<<j))
			{
				chan_info = g_edma->context[j + i * EDMA_CHANNELS];
				if (chan_info && chan_info->callback)
					chan_info->callback(chan_info->parameter, chan_info->channel);

				DMA_CINT_REG(base) = j;
				DMA_CDNE_REG(base) = j;
					
			}
		} 

	}

}

#elif (MQX_CPU == PSP_CPU_MK70F120M) || (MQX_CPU == PSP_CPU_MK70F150M) || (MQX_CPU == PSP_CPU_MK70DP256)
static void edma_interrupt_handler(pointer parameter)
{
	MQX_EDMA_CHAN_CONTEXT_PTR chan_info = (MQX_EDMA_CHAN_CONTEXT_PTR)parameter;
	DMA_MemMapPtr base;
	uint_32 interrupt, channel, base_channel;

	base = _bsp_get_DMACTRL_base_address(chan_info->channel);
	interrupt = DMA_INT_REG(base);

	base_channel = chan_info->channel - chan_info->channel % EDMA_CHANNELS;

	for (channel = chan_info->channel % DMAMUX_CHANNLES; channel < EDMA_CHANNELS; channel += DMAMUX_CHANNLES)
	{
		chan_info = g_edma->context[channel + base_channel];
		if (!chan_info)
			continue;
		if (interrupt & (1 << channel))
		{
			DMA_CINT_REG(base) = channel;
			DMA_CDNE_REG(base) = channel;

			if (edma_verify_chan(chan_info->channel) != MQX_OK)
				continue;
			if (chan_info->callback)
				chan_info->callback(chan_info->parameter, chan_info->channel);
		}

	}
}

#else
static void edma_interrupt_handler(pointer parameter)
{
	MQX_EDMA_CHAN_CONTEXT_PTR chan_info = (MQX_EDMA_CHAN_CONTEXT_PTR)parameter;
	DMA_MemMapPtr base;

	base = _bsp_get_DMACTRL_base_address(chan_info->channel);

	DMA_CINT_REG(base) = chan_info->channel;
	DMA_CDNE_REG(base) = chan_info->channel;

	if (edma_verify_chan(chan_info->channel) != MQX_OK)
		return;
	if (chan_info->callback)
		chan_info->callback(chan_info->parameter, chan_info->channel);
	
}

#endif

static void edma_err_interrupt_handler(pointer parameter)
{
	

//	uint_8 chan = (DMA_ES >> 8) & 0xf;
//	uint_32 err = DMA_ES;

//	edma_force_stop(chan);
//	DMA_CR &= DMA_CR & (~(DMA_CR_HALT_MASK));
//	
//	if (g_edma->context[chan].err_callback)
//		g_edma->context[chan].err_callback(g_edma->context[chan].err_parameter, err);
//	
//	DMA_CERR = chan;
}



DMAMUX_MemMapPtr _bsp_get_DMAMUX_base_address(uint_8 channel)
{
	uint_8 m;

	m = channel / DMAMUX_CHANNLES;

	return dmamux_base[m];
}

DMA_MemMapPtr _bsp_get_DMACTRL_base_address(uint_8 channel)
{
	uint_8 m;

	m = channel / EDMA_CHANNELS;

	return edma_base[m];
	
}

uint_8 _bsp_get_DMAMUX_chan(uint_8 channel)
{
	return channel % DMAMUX_CHANNLES;
}

uint_8 _bsp_get_DMACTRL_chan(uint_8 channel)
{
	return channel % EDMA_CHANNELS;
}


