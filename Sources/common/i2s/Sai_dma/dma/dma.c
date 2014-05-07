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
* $FileName: dma.c$
* $Version : 4.0.1$
* $Date    : Apr-19-2013$
*
* Comments:
*   This file contains generic DMA support fucntions
*
*END************************************************************************/
 
#include <mqx.h>
#include "dma.h"


static const int alignment_tab[4]  = { 4, 1, 2, 1 };


/* prepares TCD for memory to memory copy */
void dma_tcd_memcpy(DMA_TCD_STRUCT *tcd, void *src, void *dst, uint_32 size)
{
    int src_alignment;
    int dst_alignment;
    int loop_alignment;

    src_alignment = alignment_tab[((uint_32)src | (uint_32)size) & 3];
    dst_alignment = alignment_tab[((uint_32)src | (uint_32)size) & 3];

    loop_alignment = (src_alignment > dst_alignment) ? src_alignment : dst_alignment;

    _mem_zero(tcd, sizeof(*tcd));

    tcd->SRC_ADDR = (uint_32)src;
    tcd->SRC_WIDTH = src_alignment;
    tcd->SRC_OFFSET = src_alignment;

    tcd->DST_ADDR = (uint_32)dst;
    tcd->DST_WIDTH = dst_alignment;
    tcd->DST_OFFSET = dst_alignment;

    tcd->LOOP_BYTES = loop_alignment;
    tcd->LOOP_COUNT = (size >> (loop_alignment/2)); /* loop_alignment is one of 1, 2 or 4, more efficient than plain division */
}


/* prepares TCD for memory to register copy */
int dma_tcd_mem2reg(DMA_TCD_STRUCT *tcd, volatile void *reg, int regw, void *src, uint_32 size)
{
    uint_32 src_alignment;
    uint_32 reg_alignment;
    int endian_swap;
    int srcw;
    
    endian_swap = (regw < 0);
    regw = (regw < 0) ? -regw : regw;

    if ((regw != 1) && (regw != 2) && (regw != 4)) {
        return MQX_INVALID_PARAMETER;
    }

    reg_alignment = alignment_tab[((uint_32)reg | (uint_32)size) & 3];

    if (reg_alignment < regw) {
        return MQX_INVALID_PARAMETER;
    }

    _mem_zero(tcd, sizeof(*tcd));

    if (endian_swap) {
        tcd->SRC_ADDR = (uint_32)src + regw - 1;
        tcd->SRC_WIDTH = 1;
        tcd->SRC_OFFSET = -1;
        tcd->LOOP_SRC_OFFSET = 2*regw;
    }
    else {
        src_alignment = alignment_tab[((uint_32)src | (uint_32)size) & 3];
        srcw = (src_alignment > regw) ? regw : src_alignment;
        tcd->SRC_ADDR = (uint_32)src;
        tcd->SRC_WIDTH = srcw;
        tcd->SRC_OFFSET = srcw;
    }

    tcd->DST_ADDR = (uint_32)reg;
    tcd->DST_WIDTH = regw;
    tcd->DST_OFFSET = 0; /* periodic write to the same address */

    tcd->LOOP_BYTES = regw;
    tcd->LOOP_COUNT = (size >> (regw/2));

    return MQX_OK;
}


/* prepares TCD for register to memory copy */
int dma_tcd_reg2mem(DMA_TCD_STRUCT *tcd, volatile void *reg, int regw, void *dst, uint_32 size)
{
    uint_32 dst_alignment;
    uint_32 reg_alignment;
    int endian_swap;
    int dstw;

    endian_swap = (regw < -1);
    regw = (regw < 0) ? -regw : regw;

    if ((regw != 1) && (regw != 2) && (regw != 4)) {
        return MQX_INVALID_PARAMETER;
    }

    reg_alignment = alignment_tab[((uint_32)reg | (uint_32)size) & 3];

    if (reg_alignment < regw) {
        return MQX_INVALID_PARAMETER;
    }

    _mem_zero(tcd, sizeof(*tcd));

    tcd->SRC_ADDR = (uint_32)reg; /* periodic read from the same address */
    tcd->SRC_WIDTH = regw;
    tcd->SRC_OFFSET = 0;

    if (endian_swap) {
        tcd->DST_ADDR = (uint_32)dst + regw - 1;
        tcd->DST_WIDTH = 1;
        tcd->DST_OFFSET = -1;    
        tcd->LOOP_DST_OFFSET = 2*regw;
    }
    else {
        dst_alignment = alignment_tab[((uint_32)dst | (uint_32)size) & 3];
        dstw = (dst_alignment > regw) ? regw : dst_alignment;
        tcd->DST_ADDR = (uint_32)dst;
        tcd->DST_WIDTH = dstw;
        tcd->DST_OFFSET = dstw;
    }

    tcd->LOOP_BYTES = regw;
    tcd->LOOP_COUNT = (size >> (regw/2));

    return MQX_OK;
}
