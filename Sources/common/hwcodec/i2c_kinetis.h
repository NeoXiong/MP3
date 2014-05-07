/**HEADER********************************************************************
* 
* Copyright (c) 2010 Freescale Semiconductor;
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
* $FileName: IIC_kinetis.h
* $Version : 
* $Date    : 
*
* Comments:  Code for initializing and using I2C
*
*
*END************************************************************************/

/*****************************************************************************
******************************************************************************
* Public macros
******************************************************************************
*****************************************************************************/
//#include "derivative.h"
#ifndef I2C_KINETIS_H
#define I2C_KINETIS_H


/*****************************************************************************
******************************************************************************
* Public prototypes
******************************************************************************
*****************************************************************************/
//_mqx_int I2C_Init(void);
//_mqx_int I2C_DeInit(void);

//void I2C_WriteRegister(unsigned char, unsigned char , unsigned char );
//unsigned char I2C_ReadRegister(unsigned char, unsigned char );

/* codec */
_mqx_int sgtl_ModifyReg(uint_16 reg, uint_16 clr_mask, uint_16 set_mask);  /* clr_mask , set mask bit to zero */
_mqx_int sgtl_ReadReg(uint_16 reg, uint_16_ptr dest_ptr);
_mqx_int sgtl_WriteReg(uint_16 reg, uint_16 reg_val);

#endif
