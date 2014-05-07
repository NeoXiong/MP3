/*HEADER*********************************************************************
 *
 * Copyright (c) 2008 Freescale Semiconductor;
 * All Rights Reserved
 *
 * Copyright (c) 2004-2008 Embedded Access Inc.;
 * All Rights Reserved
 *
 * Copyright (c) 1989-2008 ARC International;
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
 * $FileName: lwmsgq.c$
 * $Version : 3.8.5.1$
 * $Date    : Feb-22-2012$
 *
 * Comments:
 *
 *   This file contains functions of the Lightweight message component.
 *
 *END************************************************************************/

#include "mqx_inc.h"

#if MQX_USE_LWMSGQ
#include "lwmsgq.h"
#include "lwmsgq_prv.h"



/* OTG,add for otg */
_mqx_uint _lwmsgq_deinit  /* only for no user mode */
(
    pointer location
)
{
	KERNEL_DATA_STRUCT_PTR kernel_data;
	LWMSGQ_STRUCT_PTR      q_ptr = (LWMSGQ_STRUCT_PTR) location;
	LWMSGQ_STRUCT_PTR      lwmsg_chk_ptr;
	uint_32 found = 0;

    _GET_KERNEL_DATA(kernel_data);
//    _KLOGE4(KLOG_lwmsgq_init, location, num_messages, msg_size);      //MASK by guoyifang for compile error

        lwmsg_chk_ptr = (LWMSGQ_STRUCT_PTR) ((pointer) kernel_data->LWMSGQS.NEXT);
        while (lwmsg_chk_ptr != (LWMSGQ_STRUCT_PTR) ((pointer) &kernel_data->LWMSGQS))
        {
            if (lwmsg_chk_ptr == q_ptr)
            {
            		found = 1;
            		break;

            }
            lwmsg_chk_ptr = (LWMSGQ_STRUCT_PTR) ((pointer) lwmsg_chk_ptr->LINK.NEXT);
        }

	if(found){
		 _QUEUE_REMOVE(&kernel_data->LWMSGQS, &q_ptr->LINK);
		return MQX_OK;
	}
	return (MQX_EINVAL);
}

#endif /* MQX_USE_LWMSGQ */
