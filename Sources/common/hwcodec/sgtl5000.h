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
* $FileName: sgtl5000.h$
* $Version : 3.8.3.0$
* $Date    : Sep-12-2012$
*
* Comments:
*
*   This include file is used to provide information needed by
*   applications using the SAI I/O functions.
*
*END************************************************************************/

#ifndef __sgtl5000_h__
#define __sgtl5000_h__


int32_t sgtl5000_slave_init(void);
_mqx_int sgtl5000_slave_deinit(void);
int32_t sgtl5000_master_init(uint32_t pcmSampleRate, uint8_t bitWidth);
int32_t sgtl5000_VolUp(void);
int32_t sgtl5000_mute(void);
int32_t sgtl5000_umute(void);
int32_t sgtl5000_VolDown(void);
int32_t sgtl5000_slave_set_bitwidth(uint8_t);
int32_t sgtl5000_set_fs(uint8_t);
_mqx_int sgtl5000_power_on(void);
#endif

/* EOF */
