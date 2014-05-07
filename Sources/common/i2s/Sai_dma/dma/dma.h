#ifndef _dma_h_
#define _dma_h_ 1
/**HEADER********************************************************************
*
* Copyright (c) 2013 Freescale Semiconductor;
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
* $FileName: dma.h$
* $Version : 4.0.1$
* $Date    : Jan-17-2013$
*
* Comments:
*   This header defines API for MQX DMA drivers.
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include "edma.h"


typedef struct dma_tcd_struct {
  uint_32 SRC_ADDR;	
  uint_32 SRC_WIDTH;
  int_32  SRC_OFFSET;
  uint_32 SRC_MODULO;
  uint_32 DST_ADDR;
  uint_32 DST_WIDTH;
  int_32  DST_OFFSET;
  uint_32 DST_MODULO;
  uint_32 LOOP_BYTES;
  uint_32 LOOP_COUNT;
  int_32  LOOP_SRC_OFFSET;
  int_32  LOOP_DST_OFFSET;
} DMA_TCD_STRUCT, _PTR_ DMA_TCD_PTR;


#define DMA_STATUS_IDLE     0
#define DMA_STATUS_DONE     1
#define DMA_STATUS_ACTIVE   2
#define DMA_STATUS_ERROR    3


/*
** DMA_EOT_CALLBACK
**
** This callback function is used to notify about end of transfer on a DMA channel.
*/
typedef void (_CODE_PTR_ DMA_EOT_CALLBACK)(pointer context_data);


#ifdef __cplusplus
extern "C" {
#endif


/* prototypes for low level driver specific functions */
/*
int dma_init(void);
int dma_deinit(void);

int dma_channel_setup(int vchannel, int source);
int dma_channel_status(int vchannel);

int dma_transfer_setup(int vchannel, DMA_TCD_PTR tcd);

int dma_request_enable(int vchannel);
int dma_request_disable(int vchannel);

int dma_callback_reg(int channel, DMA_EOT_CALLBACK callback_func, pointer callback_data);
*/




/* prototypes for generic functions */

void dma_tcd_memcpy(DMA_TCD_STRUCT *tcd, void *src, void *dst, uint_32 size);

int dma_tcd_mem2reg(DMA_TCD_STRUCT *tcd, volatile void *reg, int regw, void *src, uint_32 size);
int dma_tcd_reg2mem(DMA_TCD_STRUCT *tcd, volatile void *reg, int regw, void *dst, uint_32 size);


#ifdef __cplusplus
}
#endif


#endif
