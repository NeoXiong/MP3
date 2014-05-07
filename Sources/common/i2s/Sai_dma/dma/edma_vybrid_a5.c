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
* $FileName: edma_mk60.c$
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

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _bsp_get_edma_done_vector
* Returned Value   : Vector number of done event for given eDMA channel
* Comments         :
*
*END*----------------------------------------------------------------------*/

uint_32 _bsp_get_edma_done_vector(uint_8 channel)
{
	uint_8 m = channel/EDMA_CHANNELS;

	switch (m)
	{
	case 0:
		return INT_DMA0_Transfer_Complete;
	case 1:
		return INT_DMA1_Transfer_Complete;
	default:
		break;
	}
	
    return 0;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _bsp_get_edma_error_vector
* Returned Value   : Vector number of error event of given eDMA module
* Comments         :
*
*END*----------------------------------------------------------------------*/

uint_32 _bsp_get_edma_error_vector(uint_8 channel)
{
	uint_8 m =  channel/EDMA_CHANNELS;

    switch (m)
    {
		case 0:
			return INT_DMA0_Error;
		case 1:
			return INT_DMA1_Error;
		default:
			break;
    }

    return 0;
}


void _bsp_enable_dma(uint_8 module)
{
	switch (module)
	{
	case 0:
		CCM_CCGR0 |= CCM_CCGR0_CG4_MASK | CCM_CCGR0_CG5_MASK;
		CCM_CCPGR1 |= CCM_CCPGR1_PPCG8_MASK | CCM_CCPGR1_PPCG9_MASK;
	case 1:
		CCM_CCGR6 |= CCM_CCGR6_CG1_MASK | CCM_CCGR6_CG2_MASK;
		CCM_CCPGR3 |= CCM_CCPGR3_PPCG8_MASK | CCM_CCPGR3_PPCG9_MASK;
	default:
		break;
	}
	
}

