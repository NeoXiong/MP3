**HEADER********************************************************************
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
* $FileName: edma_mk60f.c$
* $Version : 4.0.1.0$
* $Date    : May-14-2013$
*
* Comments:
*
*   This file contains CPU specific eDMA functions.
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include "edma.h"


static const uint_32 /*PSP_INTERRUPT_TABLE_INDEX*/ edma_done_vectors[][16] =
{
    { 
        INT_DMA0_DMA16,
        INT_DMA1_DMA17,
        INT_DMA2_DMA18,
        INT_DMA3_DMA19,
        INT_DMA4_DMA20,
        INT_DMA5_DMA21,
        INT_DMA6_DMA22,
        INT_DMA7_DMA23,
        INT_DMA8_DMA24,
        INT_DMA9_DMA25,
        INT_DMA10_DMA26,
        INT_DMA11_DMA27,
        INT_DMA12_DMA28,
        INT_DMA13_DMA29,
        INT_DMA14_DMA30,
        INT_DMA15_DMA31
    }
};



static const uint_32 /*PSP_INTERRUPT_TABLE_INDEX*/ edma_error_vectors[][1] =
{
    { 
        INT_DMA_Error
    }
};



/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _bsp_get_edma_done_vectors
* Returned Value   : Number of vectors associated with the peripheral
* Comments         :
*    This function returns desired interrupt vector table indices for specified module.
*
*END*----------------------------------------------------------------------*/

int _bsp_get_edma_done_vectors(uint_32 dev_num, const uint_32 _PTR_ _PTR_ vectors_ptr)
{
    if (dev_num < ELEMENTS_OF(edma_done_vectors)) {
        *vectors_ptr = edma_done_vectors[dev_num];
        return ELEMENTS_OF(edma_done_vectors[0]);
    }
    return 0;
}


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _bsp_get_edma_error_vector
* Returned Value   : Number of vectors associated with the peripheral
* Comments         :
*    This function returns desired interrupt vector table indices for specified module.
*
*END*----------------------------------------------------------------------*/

int _bsp_get_edma_error_vectors(uint_32 dev_num, const uint_32 _PTR_ _PTR_ vectors_ptr)
{
    if (dev_num < ELEMENTS_OF(edma_error_vectors)) {
        *vectors_ptr = edma_error_vectors[dev_num];
        return ELEMENTS_OF(edma_error_vectors[0]);
    }
    return 0;
}


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _bsp_edma_enable
* Returned Value   :
* Comments         :
*    The function performs necessary operations to enable eDMA module
*
*END*----------------------------------------------------------------------*/

int _bsp_edma_enable(uint_32 dev_num)
{
    /* Enable DMAMUX clock */
    SIM_SCGC6 |= SIM_SCGC6_DMAMUX0_MASK;
    SIM_SCGC6 |= SIM_SCGC6_DMAMUX1_MASK;

    /* Enable DMA clock */
    SIM_SCGC7 |= SIM_SCGC7_DMA_MASK;

    return 0;
}
