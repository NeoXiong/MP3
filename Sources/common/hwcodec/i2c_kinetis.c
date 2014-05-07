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
* $FileName: IIC_kinetis.c
* $Version : 
* $Date    : 
*
* Comments:  Code for initializing and using I2C
*
*
*END************************************************************************/

//#include "derivative.h"     /* include peripheral declarations */

#include <mqx.h>
#include <bsp.h>
#include <i2c.h>
#include "i2c_kinetis.h"

MQX_FILE_PTR fd = NULL;
#define I2C_DEVICE_POLLED "i2c0:"
#define I2C_DEVICE_INTERRUPT "ii2c0:"

#define I2C_SHARED

#ifdef I2C_SHARED
LWSEM_STRUCT  i2c_sem;

#define  NEW_I2C_SEM \
do { \
	    _lwsem_create((LWSEM_STRUCT_PTR)(&i2c_sem), 1);\
} while(0);


#define   GET_I2C_SEM \
do { \
	    _lwsem_wait((LWSEM_STRUCT_PTR)&i2c_sem); \
} while(0);

#define   PUT_I2C_SEM \
do { \
	    _lwsem_post((LWSEM_STRUCT_PTR)&i2c_sem); \
} while(0);

#define  DEL_I2C_SEM \
do { \
	   _lwsem_destroy((LWSEM_STRUCT_PTR)&i2c_sem);\
} while(0);
#else
#define  NEW_I2C_SEM
#define   GET_I2C_SEM 
#define   PUT_I2C_SEM
#define  DEL_I2C_SEM
#endif


/*FUNCTION****************************************************************
* 
* Function Name    : I2C_Init
* Returned Value   : MQX error code
* Comments         :
*    Writes a value to the entire register. All
*    bit-fields of the register will be written.
*
*END*********************************************************************/
_mqx_int I2C_Init()
{
	uint_32 param;
	I2C_STATISTICS_STRUCT stats;

	if (fd == NULL)
	{
	   fd = fopen (I2C_DEVICE_POLLED, NULL);
	   fclose(fd);
	   fd = fopen (I2C_DEVICE_POLLED, NULL);
	}
	if (fd == NULL)
	{
		printf ("ERROR: Unable to open I2C driver!\n");
		return(-9);
	}
	param = 200000/* 100000 */;
	if (I2C_OK != ioctl (fd, IO_IOCTL_I2C_SET_BAUD, &param))
	{
		return(-1);
	}
	if (I2C_OK != ioctl (fd, IO_IOCTL_I2C_SET_MASTER_MODE, NULL))
	{
		return(-2);
	}
	if (I2C_OK != ioctl (fd, IO_IOCTL_I2C_CLEAR_STATISTICS, NULL))
	{
		return(-3);
	}

	NEW_I2C_SEM;
	/** move to read,write reg function */
#if 0	
	param = u8I2CSlaveAddress;
	if (I2C_OK != ioctl (fd, IO_IOCTL_I2C_SET_DESTINATION_ADDRESS, &param))
	{
		return(-4);
	}
	
	/* Initiate start and send I2C bus address */
	fwrite (&param, 1, 0, fd);
	
	if (I2C_OK != ioctl (fd, IO_IOCTL_I2C_GET_STATISTICS, (pointer)&stats))
	{
		return(-5);
	}
   /* Stop I2C transfer */
 	if (I2C_OK != ioctl (fd, IO_IOCTL_I2C_STOP, NULL))
 	{
 		return(-6);
 	}
	/* Check ack (device exists) */
	if (I2C_OK == ioctl (fd, IO_IOCTL_FLUSH_OUTPUT, &param))
	{
	    if ((param) || (stats.TX_NAKS)) 
		{
	    	return(-7);
		}
	}
	else
	{
		return(-8);
	}
#endif	
	return(MQX_OK);
}

_mqx_int I2C_DeInit(void)
{
	if(fd != NULL) {
		fclose(fd);
		DEL_I2C_SEM;
		fd = NULL;
	}
        else
          return -1;
        
        return 0;
}

static inline int i2c_SetAddress(unsigned char u8I2CSlaveAddress)
{
	uint_32 param;
        I2C_STATISTICS_STRUCT stats;
	static uint_8 last_address = 0;	

	if(u8I2CSlaveAddress != last_address) {
	
	if (fd == NULL)  {
		printf ("ERROR: Unable to open I2C driver!\n");
		return(-9);
	}

	param = /* MAX3353_I2C_ADDRESS*/u8I2CSlaveAddress;
	if (I2C_OK != ioctl (fd, IO_IOCTL_I2C_SET_DESTINATION_ADDRESS, &param)) {
		return(-4);
	}

#if 1
	/* Initiate start and send I2C bus address */
	param = fwrite (&param, 1, 0, fd);
	
	if (I2C_OK != ioctl (fd, IO_IOCTL_I2C_GET_STATISTICS, (pointer)&stats))
	{
		printf("-5\n");
		return(-5);
	}
   /* Stop I2C transfer */
 	if (I2C_OK != ioctl (fd, IO_IOCTL_I2C_STOP, NULL))
 	{
		printf("-6\n");
 		return(-6);
 	}
	/* Check ack (device exists) */
	if (I2C_OK == ioctl (fd, IO_IOCTL_FLUSH_OUTPUT, &param))
	{
	    if ((param) || (stats.TX_NAKS)) 
		{
		//printf("-7\n");
	    	//return(-7);
		}
	}
	else
	{
		printf("-8\n");
		return(-8);
	}
#endif        
		last_address = u8I2CSlaveAddress;
	}


	return(MQX_OK);
}
/*FUNCTION****************************************************************
* 
* Function Name    : WriteReg
* Returned Value   : MQX error code
* Comments         :
*    Writes a value to the entire register. All
*    bit-fields of the register will be written.
*
*END*********************************************************************/
static inline _mqx_int _WriteReg(uint_8 reg, uint_8 reg_val)
{
	uint_8 buffer[2];
	uint_32 result;
	buffer[0] =	(uint_8)(reg & 0xFF);
	buffer[1] =	(uint_8)(reg_val & 0xFF);
	result = write(fd, buffer, 2); 
	if (2 != result)
	{
		#ifdef OTG_TRANSCEIVER_DEBUG
		 printf("_WriteReg: Error - write to address ");
		 printf("0x%04X failed.\n", reg);
		#endif
		return(-1);
	} 
	result = fflush (fd);
	if (MQX_OK != result)
	{
		return(-3);
	} 
	/* Stop I2C transfer */
	if (I2C_OK != ioctl (fd, IO_IOCTL_I2C_STOP, NULL))
	{
		return(-2);
	}
	result = 0;
	return(MQX_OK);
}

/*FUNCTION****************************************************************
* 
* Function Name    : WriteReg
* Returned Value   : MQX error code
* Comments         :
*    Reads value of register. 
*
*END*********************************************************************/
static inline _mqx_int _ReadReg(uint_8 reg, uint_8_ptr dest_ptr)
{
	uint_8 buffer[2];
	uint_32 result, param;

	buffer[0] =	(uint_8)(reg & 0xFF);
	result = write(fd, buffer, 1);
	if (1 != result)
	{
		#ifdef OTG_TRANSCEIVER_DEBUG
		 printf("_ReadReg: Error - otg transceiver not responding.\n");
		#endif
		return(-1);
	}
	result = fflush (fd);
	if (MQX_OK != result)
	{
		return(-6);
	} 
	/* Send repeated start */
	if (I2C_OK != ioctl (fd, IO_IOCTL_I2C_REPEATED_START, NULL))
	{
		#ifdef OTG_TRANSCEIVER_DEBUG	
		 printf("_ReadReg: Error - unable to send repeated start.\n");
		#endif
		return(-2);
	}
	/* Set read request */
	param = 1;
	if (I2C_OK != ioctl (fd, IO_IOCTL_I2C_SET_RX_REQUEST, &param))
	{
		#ifdef OTG_TRANSCEIVER_DEBUG	
		 printf("_ReadReg: Error - unable to set number of bytes requested.\n");
		#endif
		return(-3);
	}
	result = 0;
	/* Read all data */ 
	result = read (fd, buffer, 1);	
	if (1 != result)
	{
		#ifdef OTG_TRANSCEIVER_DEBUG	
		 printf("_ReadReg: Error - otg transceiver not responding.\n");
		#endif
		return(-4);
	}
	result = fflush (fd);
	if (MQX_OK != result)
	{
		return(-7);
	} 
	*dest_ptr = (buffer[0] & 0xFF);
	/* Stop I2C transfer */
	if (I2C_OK != ioctl (fd, IO_IOCTL_I2C_STOP, NULL))
	{
		return(-5);
	}
	return (MQX_OK);
}

/*FUNCTION****************************************************************
* 
* Function Name    : ModifyReg
* Returned Value   : MQX error code
* Comments         :
*    Modifies value of register. Bits to set to zero are defined by first 
*	 mask, bits to be set to one are defined by second mask.
*
*END*********************************************************************/
_mqx_int i2c_ModifyReg(unsigned char address, uint_8 reg, uint_8 clr_mask, uint_8 set_mask)
{
	int ret = 0;
	uint_8 reg_val = 0;
    /* Init Channel resource bit map */
    GET_I2C_SEM;
	
	if(i2c_SetAddress(address) != MQX_OK){
		 printf("set slave address: Error.\n");
		ret = -1;// return -1;
		goto end_modi8;
	}
		
	if (MQX_OK != _ReadReg(reg, &reg_val)) {
		#ifdef OTG_TRANSCEIVER_DEBUG	
		 printf("_ModifyReg: Error - cannot read from SGTL.\n");
		#endif
		ret = -1;// return -1;
		goto end_modi8;
	}
	reg_val &= clr_mask;
	reg_val |= set_mask;
	if (MQX_OK != _WriteReg(reg, reg_val))
	{
		#ifdef OTG_TRANSCEIVER_DEBUG
		 printf("_ModifyReg: Error - cannot write to SGTL.\n");
		#endif
		ret = -2;// return -2;
	}

end_modi8:
	PUT_I2C_SEM;
	return ret;
	
}


int  I2C_ReadRegister(unsigned char address,unsigned char reg,unsigned char * reg_val)
{	// liunote must add mutex when used audio contrl i2c
	int ret = 0;
	GET_I2C_SEM;
	if(i2c_SetAddress(address) != MQX_OK) {
		 printf("set slave address: Error.\n");
		 ret = -1;
		 goto end_read8; //return 0;
	}

	if (MQX_OK != _ReadReg(reg, reg_val))
	{
		//return(0);
		ret = -1;
		printf("ReadRegister 8 failed\n");
	}

end_read8:
	PUT_I2C_SEM;
	return ret;
}


int I2C_WriteRegister(unsigned char address ,unsigned char reg, unsigned char reg_val)
{
	// liunote must add mutex when used audio contrl i2c
	int ret = 0;
	GET_I2C_SEM;
	if(i2c_SetAddress(address) != MQX_OK){
		 printf("w16 set slave address: Error.\n");
		 ret = -1;
		 goto end_write8; //return ;
	}

	if (MQX_OK != _WriteReg(reg, reg_val)) {
		//return;
		ret = -1;
		printf("WriteRegister 8 failed\n");
	}
end_write8:
	PUT_I2C_SEM;
	return ret;
}

/* 16bit register for codec */
_mqx_int _WriteReg16(uint_16 reg, uint_16 reg_val)
{
    uint_8 buffer[4];
    uint_32 result;
    buffer[0] = (uint_8)((reg >> 8) & 0xFF);
    buffer[1] =	(uint_8)(reg & 0xFF);
    buffer[2] =	(uint_8)((reg_val >> 8) & 0xFF);
    buffer[3] =	(uint_8)(reg_val & 0xFF);
    result = write(fd, buffer, 4);
    if (4 != result)
    {
        #ifdef SGTL5000_DEBUG
        ////PrintMessage("sgtl_WriteReg: Error - write to address ");
        printf("0x%04X failed.\n", reg);
        #endif
        return(-1);
    }
    result = fflush (fd);
    if (MQX_OK != result)
    {
        return(-3);
    } 
    /* Stop I2C transfer */
    if (I2C_OK != ioctl (fd, IO_IOCTL_I2C_STOP, NULL))
    {
        return(-2);
    }
    result = 0;
    return(MQX_OK);
}

_mqx_int _ReadReg16(uint_16 reg, uint_16_ptr dest_ptr)
{
    uint_8 buffer[2];
    uint_32 result, param;
    buffer[0] = (uint_8)((reg >> 8) & 0xFF);
    buffer[1] =	(uint_8)(reg & 0xFF);
    result = write(fd, buffer, 2);
    if (2 != result)
    {
        #ifdef SGTL5000_DEBUG
        //PrintMessage("sgtl_ReadReg: Error - SGTL not responding.\n");
        #endif
        return(-1);
    }
    result = fflush (fd);
    if (MQX_OK != result)
    {
        return(-6);
    } 
    /* Send repeated start */
    if (I2C_OK != ioctl (fd, IO_IOCTL_I2C_REPEATED_START, NULL))
    {
        #ifdef SGTL5000_DEBUG
        //PrintMessage("sgtl_ReadReg: Error - unable to send repeated start.\n");
        #endif
        return(-2);
    }
    /* Set read request */
    param = 2;
    if (I2C_OK != ioctl (fd, IO_IOCTL_I2C_SET_RX_REQUEST, &param))
    {
        #ifdef SGTL5000_DEBUG
        //PrintMessage("sgtl_ReadReg: Error - unable to set number of bytes requested.\n");
        #endif
        return(-3);
    }
    result = 0;
    /* Read all data */ 
    result = read (fd, buffer, 2);
    if (2 != result)
    {
        #ifdef SGTL5000_DEBUG
        //PrintMessage("sgtl_ReadReg: Error - SGTL not responding.\n");
        #endif
        return(-4);
    }
    result = fflush (fd);
    if (MQX_OK != result)
    {
        return(-7);
    } 
    *dest_ptr = (buffer[1] & 0xFFFF) | ((buffer[0] & 0xFFFF) << 8);
    /* Stop I2C transfer */
    if (I2C_OK != ioctl (fd, IO_IOCTL_I2C_STOP, NULL))
    {
        return(-5);
    }
    return (MQX_OK);
}

_mqx_int _ModifyReg16(uint_16 reg, uint_16 clr_mask, uint_16 set_mask)
{
    uint_16 reg_val = 0;
    if (MQX_OK != _ReadReg16(reg, &reg_val))
    {
        #ifdef SGTL5000_DEBUG
        //PrintMessage("sgtl_ModifyReg: Error - cannot read from SGTL.\n");
        #endif
        return(-1);
    }
    reg_val &= clr_mask;
    reg_val |= set_mask;
    if (MQX_OK != _WriteReg16(reg, reg_val))
    {
        #ifdef SGTL5000_DEBUG
        //PrintMessage("sgtl_ModifyReg: Error - cannot write to SGTL.\n");
        #endif
        return(-2);
    }
    return(MQX_OK);
}

#if 1 /* move to codec wrapper */
#define SGTL5000_I2C_ADDR 0x0A
_mqx_int sgtl_WriteReg(uint_16 reg, uint_16 reg_val)
{
	uint_8 ret = 0;
	GET_I2C_SEM;
	// must add mutex when used audio contrl i2c ?
	if(i2c_SetAddress(SGTL5000_I2C_ADDR) != MQX_OK){
		 printf("set slave address: Error.\n");
		 ret = -1;//return -1;
		 goto end_write16;
	}

	if (MQX_OK != _WriteReg16(reg, reg_val)) {
		ret = -1; //return -1;
	}

end_write16:
	PUT_I2C_SEM;
	return ret;
}

_mqx_int sgtl_ReadReg(uint_16 reg, uint_16_ptr dest_ptr)
{
	uint_8 ret = 0;
	GET_I2C_SEM;
	
	if(i2c_SetAddress(SGTL5000_I2C_ADDR) != MQX_OK){
		 printf("set slave address: Error.\n");
		 ret = -1; //return -1;
		 goto end_read16;
	}

	if (MQX_OK != _ReadReg16(reg, dest_ptr)) {
		ret = -1; //return -1;
	}

end_read16:
	PUT_I2C_SEM;
	return ret;
}

_mqx_int sgtl_ModifyReg(uint_16 reg, uint_16 clr_mask, uint_16 set_mask)
{
	uint_8 ret = 0;
	GET_I2C_SEM;
	
	if(i2c_SetAddress(SGTL5000_I2C_ADDR) != MQX_OK) {
		 printf("set slave address: Error.\n");
		 ret = -1;//return 0;
		 goto end_modi16;
	}

	if (MQX_OK != _ModifyReg16(reg, clr_mask,set_mask)) {
		printf("modify 16 err\n");
		ret = -1; //return(-1);
	}

end_modi16:
	PUT_I2C_SEM;
	return ret;
}

#endif
