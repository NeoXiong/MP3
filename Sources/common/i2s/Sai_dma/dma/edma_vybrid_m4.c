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
* $FileName: edma_vybrid_m4.c$
* $Version : 4.0.1.0$
* $Date    : Mar-20-2013$
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
        NVIC_DMA0_Transfer_Complete
    },
    { 
        NVIC_DMA1_Transfer_Complete
    }
};



static const uint_32 /*PSP_INTERRUPT_TABLE_INDEX*/ edma_error_vectors[][1] =
{
    { 
        NVIC_DMA0_Error
    },
    { 
        NVIC_DMA1_Error
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
    switch (dev_num) {
        case 0:
            CCM_CCGR0 |= CCM_CCGR0_CG4_MASK; /* DMAMUX0 */
            CCM_CCGR0 |= CCM_CCGR0_CG5_MASK; /* DMAMUX1 */
            CCM_CCPGR1 |= CCM_CCPGR1_PPCG8_MASK; /* DMA0 */
            CCM_CCPGR1 |= CCM_CCPGR1_PPCG9_MASK; /* DMA0 TCD */
            break;
        case 1:
            CCM_CCGR6 |= CCM_CCGR6_CG1_MASK; /* DMAMUX2 */
            CCM_CCGR6 |= CCM_CCGR6_CG2_MASK; /* DMAMUX3 */
            CCM_CCPGR3 |= CCM_CCPGR3_PPCG8_MASK; /* DMA1 */
            CCM_CCPGR3 |= CCM_CCPGR3_PPCG9_MASK; /* DMA1 TCD */
            break;
        default:
            return -1;
    }
    
    return 0;
}
