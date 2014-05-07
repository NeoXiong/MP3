/**HEADER********************************************************************
*
* Copyright (c) 2008 Freescale Semiconductor;
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
* $FileName: init_sai.c$
* $Version : 3.8.5.0$
* $Date    : Sep-13-2012$
*
* Comments:
*
*   This file contains board-specific SAI initialization functions.
*
*END************************************************************************/
#ifndef _INIT_SAI_DMA
#define _INIT_SAI_DMA
#include <mqx.h>
#include <bsp.h>
#include "sai_dma.h"
#include "sai_audio_dma.h"
#include "sai_ksai_dma.h"
#include <cm_kinetis.h>

AUDIO_DATA_FORMAT_DMA _bsp_audio_data_init_dma = {
    AUDIO_LITTLE_ENDIAN,    /* Endian of input data */
    AUDIO_ALIGNMENT_LEFT,   /* Alignment of input data */
    16,                      /* Bit size of input data */
    2,                      /* Sample size in bytes */
    2                       /* Number of channels */
};

KSAI_DMA_INIT_STRUCT _bsp_ksai_dma_init = {
    0,                      /* Selected peripheral (HW channel) */
    0,                      /* TX channel */
    0,                      /* RX channel */
    I2S_TX_ASYNCHRONOUS |   /* TX is asynchronous */
    I2S_RX_SYNCHRONOUS  |   /* RX hooked on TX */
    I2S_TX_BCLK_NORMAL  |   /* Both TX and RX are clocked by the transmitter */
    I2S_RX_BCLK_NORMAL,     /* bit clock (SAI_TX_BCLK) */
    I2S_TX_MASTER |         /* SAI transmitter mode */
    I2S_RX_MASTER,          /* SAI receiver mode */
    16,                     /* Data bits */
    I2S_CLK_INT,            /* Clock source */
    FALSE,                  /* Tx Dummy */
    5,                      /* Interrupt priority */
    512,                    /* Buffer size */
    CM_CLOCK_SOURCE_SYSTEM, /* Internal master clock source */
    &_bsp_audio_data_init_dma   /* Audio init */
};


const SAI_DMA_INIT_STRUCT _bsp_sai_dma_init = 
{
    "sai0:",
    _ksai_dma_int_init,
    _ksai_dma_int_deinit,
    _ksai_dma_int_rx,
    _ksai_dma_int_tx,
    _ksai_dma_int_ioctl,
    &_bsp_ksai_dma_init
};
#endif
