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

uint_32 _bsp_get_edma_done_vector(uint_8 channel)
{

	if (channel > EDMA_CHANNELS)
		return 0;

	return INT_DMA0_DMA16 + channel % DMAMUX_CHANNLES;
}

uint_32 _bsp_get_edma_error_vector(uint_8 channel)
{
	return INT_DMA_Error;
}

void _bsp_enable_dma(uint_8 module)
{
//	SIM_SCGC6 |= SIM_SCGC6_DMAMUX_MASK;
	SIM_SCGC7 |= SIM_SCGC7_DMA_MASK;
	SIM_SCGC6 |= SIM_SCGC6_DMAMUX0_MASK | SIM_SCGC6_DMAMUX1_MASK;
}

