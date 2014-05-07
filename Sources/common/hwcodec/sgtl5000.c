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
* $FileName: sgtl5000.c$
* $Version : 3.8.4.0$
* $Date    : Sep-12-2012$
*
* Comments:
*
*   This include file is used to provide information needed by
*   applications using the SAI I/O functions.
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include <fio.h>
#include <i2c.h>
#include "sgtl5000.h"

#define SGTL_DEBUG
#undef SGTL_DEBUG

#define CHIP_ID 0x0000
#define CHIP_DIG_POWER 0x0002
#define CHIP_CLK_CTRL 0x0004
#define CHIP_I2S_CTRL 0x0006
#define CHIP_SSS_CTRL 0x000A
#define CHIP_ADCDAC_CTRL 0x000E
#define CHIP_DAC_VOL 0x0010
#define CHIP_PAD_STRENGTH 0x0014
#define CHIP_ANA_ADC_CTRL 0x0020
#define CHIP_ANA_HP_CTRL 0x0022
#define CHIP_ANA_CTRL 0x0024
#define CHIP_LINREG_CTRL 0x0026
#define CHIP_REF_CTRL 0x0028
#define CHIP_MIC_CTRL 0x002A
#define CHIP_LINE_OUT_CTRL 0x002C
#define CHIP_LINE_OUT_VOL 0x002E
#define CHIP_ANA_POWER 0x0030
#define CHIP_PLL_CTRL 0x0032
#define CHIP_CLK_TOP_CTRL 0x0034
#define CHIP_ANA_STATUS 0x0036
#define CHIP_ANA_TEST2 0x003A
#define CHIP_SHORT_CTRL 0x003C
#define DAP_CONTROL 0x0100
#define DAP_PEQ 0x0102
#define DAP_BASS_ENHANCE 0x0104
#define DAP_BASS_ENHANCE_CTRL 0x0106
#define DAP_AUDIO_EQ 0x0108
#define DAP_SGTL_SURROUND 0x010A
#define DAP_FILTER_COEF_ACCESS 0x010C
#define DAP_COEF_WR_B0_MSB 0x010E
#define DAP_COEF_WR_B0_LSB 0x0110
#define DAP_AUDIO_EQ_BASS_BAND0 0x0116
#define DAP_AUDIO_EQ_BAND1 0x0118
#define DAP_AUDIO_EQ_BAND2 0x011A
#define DAP_AUDIO_EQ_BAND3 0x011C
#define DAP_AUDIO_EQ_TREBLE_BAND4 0x011E
#define DAP_MAIN_CHAN 0x0120
#define DAP_MIX_CHAN 0x0122
#define DAP_AVC_CTRL 0x0124
#define DAP_AVC_THRESHOLD 0x0126
#define DAP_AVC_ATTACK 0x0128
#define DAP_AVC_DECAY 0x012A
#define DAP_COEF_WR_B1_MSB 0x012C
#define DAP_COEF_WR_B1_LSB 0x012E
#define DAP_COEF_WR_B2_MSB 0x0130
#define DAP_COEF_WR_B2_LSB 0x0132
#define DAP_COEF_WR_A1_MSB 0x0134
#define DAP_COEF_WR_A1_LSB 0x0136
#define DAP_COEF_WR_A2_MSB 0x0138
#define DAP_COEF_WR_A2_LSB 0x013A
#define SGTL5000_I2C_ADDR 0x0A
#define SGTL5000_DEBUG 1


#ifndef CLK_MULT
#define CLK_MULT 384
#endif




#if BSPCFG_ENABLE_I2C0
#define I2C_DEVICE_POLLED "i2c0:"
#elif BSPCFG_ENABLE_I2C1
#define I2C_DEVICE_POLLED "i2c1:"
#elif BSPCFG_ENABLE_I2C2
#define I2C_DEVICE_POLLED "i2c2:"
#elif BSPCFG_ENABLE_I2C3
#define I2C_DEVICE_POLLED "i2c3:"
#elif BSPCFG_ENABLE_I2C4
#define I2C_DEVICE_POLLED "i2c4:"
#elif BSPCFG_ENABLE_I2C5
#define I2C_DEVICE_POLLED "i2c5:"
#else
#error No communication device defined! Please check user_config.h and enable proper I2C device. 
#endif

#if BSPCFG_ENABLE_SAI
#define AUDIO_DEVICE "sai0:"
#elif BSPCFG_ENABLE_II2S0
#define AUDIO_DEVICE "ii2s0:"
#else
//#error No audio device defined! Please check user_config.h and enable either I2S or SAI device.
#define AUDIO_DEVICE "sai0:"  /* set BSPCFG_ENABLE_SAI to 0 at user_config.h */
#endif

#ifdef SGTL5000_DEBUG

#endif

#define DELTA_VOL 0x08
#define MAX_VOL 0x78
#define MIN_VOL 0x20

#define SGTL_NOT_INIT  0
#define SGTL_INITED  0
#define HP_INIT_VOL 0x4040
#define DAC_INIT_VOL 0x3c3c

struct Sgtl5000_Info
{
	uint16_t hpVol;
	uint16_t dacVol;
	uint8_t isInit;
}g_sgtl5000_info = {HP_INIT_VOL, DAC_INIT_VOL, SGTL_NOT_INIT};
static MQX_FILE_PTR fd = NULL;
static uint16_t hp_vol = HP_INIT_VOL;
static uint16_t dac_vol = DAC_INIT_VOL;

#if (1)
	#include "i2c_kinetis.h"
//#else
/*FUNCTION****************************************************************
* 
* Function Name    : sgtl_WriteReg
* Returned Value   : MQX error code
* Comments         :
*    Writes a value to the entire register. All
*    bit-fields of the register will be written.
*
*END*********************************************************************/
_mqx_int sgtl_Init(void)
{
    uint_32 param;
    I2C_STATISTICS_STRUCT stats;
#if (defined BSP_TWRMCF54418)
    VMCF5441_STRUCT_PTR pReg = _PSP_GET_IPSBAR();
    pReg->GPIO.SRCR_CANI2C |= 0x03;
#endif
    if (fd == NULL)
    {
        fd = fopen (I2C_DEVICE_POLLED, NULL);
    }
    if (fd == NULL) 
    {
#ifdef  SGTL_DEBUG
	    	printf ("ERROR: Unable to open I2C driver!\n");
#endif
        
        return(-9);
    }
    param = 100000;
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
    param = SGTL5000_I2C_ADDR;
    if (I2C_OK != ioctl (fd, IO_IOCTL_I2C_SET_DESTINATION_ADDRESS, &param))
    {
        return(-4);
    }
    
    /* Initiate start and send I2C bus address */
    param = fwrite (&param, 1, 0, fd);
    
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
    return(MQX_OK);
}
#else   // modify by gyf
/*FUNCTION****************************************************************
* 
* Function Name    : sgtl_WriteReg
* Returned Value   : MQX error code
* Comments         :
*    Writes a value to the entire register. All
*    bit-fields of the register will be written.
*
*END*********************************************************************/
_mqx_int sgtl_WriteReg(uint_16 reg, uint_16 reg_val)
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
        #ifdef SGTL_DEBUG
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

/*FUNCTION****************************************************************
* 
* Function Name    : sgtl_WriteReg
* Returned Value   : MQX error code
* Comments         :
*    Reads value of register. 
*
*END*********************************************************************/
_mqx_int sgtl_ReadReg(uint_16 reg, uint_16_ptr dest_ptr)
{
    uint_8 buffer[2];
    uint_32 result, param;
    buffer[0] = (uint_8)((reg >> 8) & 0xFF);
    buffer[1] =	(uint_8)(reg & 0xFF);
    result = write(fd, buffer, 2);
    if (2 != result)
    {
        #ifdef SGTL_DEBUG
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
        #ifdef SGTL_DEBUG
        //PrintMessage("sgtl_ReadReg: Error - unable to send repeated start.\n");
        #endif
        return(-2);
    }
    /* Set read request */
    param = 2;
    if (I2C_OK != ioctl (fd, IO_IOCTL_I2C_SET_RX_REQUEST, &param))
    {
        #ifdef SGTL_DEBUG
        //PrintMessage("sgtl_ReadReg: Error - unable to set number of bytes requested.\n");
        #endif
        return(-3);
    }
    result = 0;
    /* Read all data */ 
    result = read (fd, buffer, 2);
    if (2 != result)
    {
        #ifdef SGTL_DEBUG
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



/*FUNCTION****************************************************************
* 
* Function Name    : sgtl_ModifyReg
* Returned Value   : MQX error code
* Comments         :
*    Modifies value of register. Bits to set to zero are defined by first 
*	 mask, bits to be set to one are defined by second mask.
*
*END*********************************************************************/
_mqx_int sgtl_ModifyReg(uint_16 reg, uint_16 clr_mask, uint_16 set_mask)
{
    uint_16 reg_val = 0;
    if (MQX_OK != sgtl_ReadReg(reg, &reg_val))
    {
        #ifdef SGTL_DEBUG
        //PrintMessage("sgtl_ModifyReg: Error - cannot read from SGTL.\n");
        #endif
        return(-1);
    }
    reg_val &= clr_mask;
    reg_val |= set_mask;
    if (MQX_OK != sgtl_WriteReg(reg, reg_val))
    {
        #ifdef SGTL_DEBUG
        //PrintMessage("sgtl_ModifyReg: Error - cannot write to SGTL.\n");
        #endif
        return(-2);
    }
    return(MQX_OK);	
}
#endif


int32_t sgtl5000_VolDown()
{
	if(( hp_vol&0x7f ) >= MAX_VOL)	//already min volume
				return 0;
	hp_vol += (DELTA_VOL<<7 | DELTA_VOL);
#ifdef  SGTL_DEBUG
	    	printf("vol is %d\n",hp_vol&0x7f);
#endif
	
	return sgtl_WriteReg(CHIP_ANA_HP_CTRL, hp_vol);
	
}

int32_t sgtl5000_VolUp()
{		
	if((hp_vol&0x7f) <= MIN_VOL)	//already max volume
		return 0;
	hp_vol -= (DELTA_VOL<<7 | DELTA_VOL);
#ifdef  SGTL_DEBUG
	    	printf("vol is %d\n",hp_vol&0x7f);
#endif
	
	return sgtl_WriteReg(CHIP_ANA_HP_CTRL, hp_vol);
	
}

int32_t sgtl5000_mute(void)
{
	 sgtl_ModifyReg(CHIP_ANA_CTRL, 0xFFFF, 0x0010); 	
}
int32_t sgtl5000_umute(void)
{
	 sgtl_ModifyReg(CHIP_ANA_CTRL, 0xFFEF, 0x0000); 
}
/*
 * set bit width in slave mode
 */

int32_t sgtl5000_slave_set_bitwidth(uint8_t bitWidth)
{
	 switch(bitWidth)
	    {
	    case 16:
	    	sgtl_ModifyReg(CHIP_I2S_CTRL, 0xFFFF, 0x0030); 
	    	break;
	    case 20: 
	    	sgtl_ModifyReg(CHIP_I2S_CTRL, 0xFFFF, 0x0020); 
	    	break;
	    case 24:
	    	sgtl_ModifyReg(CHIP_I2S_CTRL, 0xFFFF, 0x0010); 
	    	break;
	    case 32:
	    	sgtl_ModifyReg(CHIP_I2S_CTRL, 0xFFFF, 0x0000); 
	    	break;
	    default:
	#ifdef  SGTL_DEBUG
	    	printf("In file %s line %d  function :%s \n", __FILE__  , __LINE__, __FUNCTION__);
	    	printf("Set bit width failed\n");
	#endif
	    	return -1;
	    }
	 return 0;
}

/*
 * 
 */
_mqx_int sgtl5000_slave_deinit(void)
{
	//sgtl5000_mute();
	return 0;
}
/*_mqx_int sgtl5000_slave_init(void)*/
int32_t sgtl5000_slave_init(void)
{
	  // Configure the I2S clocks in SLAVE mode
	  // NOTE: I2S LRCLK is same as the system sample clock	  
	
	   sgtl5000_power_on();      //add by gyf
	
	    switch(16)
	    {
	    case 16:
	    	sgtl_ModifyReg(CHIP_I2S_CTRL, 0xFFFF, 0x0130); 
	    	break;
	    case 20: 
	    	sgtl_ModifyReg(CHIP_I2S_CTRL, 0xFFFF, 0x0120); 
	    	break;
	    case 24:
	    	sgtl_ModifyReg(CHIP_I2S_CTRL, 0xFFFF, 0x0110); 
	    	break;
	    case 32:
	    	sgtl_ModifyReg(CHIP_I2S_CTRL, 0xFFFF, 0x0100); 
	    	break;
	    default:
	#ifdef  SGTL_DEBUG
	    	printf("In file %s line %d  function :%s \n", __FILE__  , __LINE__, __FUNCTION__);
	    	printf("Set bit width failed\n");
	#endif
	          ;
	     }
            return 0;
}

/*FUNCTION****************************************************************
* 
* Function Name    : InitCodec
* Returned Value   : MQX error code
* Comments         :
*     
*
*END*********************************************************************/
_mqx_int sgtl5000_power_on(void)
{
    _mqx_uint errcode = 0;

#if 0
    _time_delay(50);
#endif

#if 0    
    /*
     * init I2C
     */
    errcode = sgtl_Init();
    if (errcode != MQX_OK)
    {
	#ifdef  SGTL_DEBUG
	    	printf("In file %s line %d  function :%s \n", __FILE__  , __LINE__, __FUNCTION__);
	    	printf("I2C init failed\n");
	#endif
        return (errcode);
    }
#endif    
    
    //--------------- Power Supply Configuration----------------
    // NOTE: This next 2 Write calls is needed ONLY if VDDD is
    // internally driven by the chip
    // Configure VDDD level to 1.2V (bits 3:0)
    sgtl_WriteReg(CHIP_LINREG_CTRL, 0x0008);
    // Power up internal linear regulator (Set bit 9)
    sgtl_WriteReg(CHIP_ANA_POWER, 0x7260);
    // NOTE: This next Write call is needed ONLY if VDDD is
    // externally driven
    // Turn off startup power supplies to save power (Clear bit 12 and 13)
    sgtl_WriteReg(CHIP_ANA_POWER, 0x4260);
    // NOTE: The next 2 Write calls is needed only if both VDDA and
    // VDDIO power supplies are less than 3.1V.
    // Enable the internal oscillator for the charge pump (Set bit 11)
    sgtl_WriteReg(CHIP_CLK_TOP_CTRL, 0x0800);
    // Enable charge pump (Set bit 11)
    sgtl_WriteReg(CHIP_ANA_POWER, 0x4A60);
    // NOTE: The next 2 modify calls is only needed if both VDDA and
    // VDDIO are greater than 3.1V
    // Configure the charge pump to use the VDDIO rail (set bit 5 and bit 6)
    sgtl_WriteReg(CHIP_LINREG_CTRL, 0x006C);
    
    //------ Reference Voltage and Bias Current Configuration----------
    // NOTE: The value written in the next 2 Write calls is dependent
    // on the VDDA voltage value.
    // Set ground, ADC, DAC reference voltage (bits 8:4). The value should
    // be set to VDDA/2. This example assumes VDDA = 1.8V. VDDA/2 = 0.9V.
    // The bias current should be set to 50% of the nominal value (bits 3:1)
    sgtl_WriteReg(CHIP_REF_CTRL, 0x01FE);
    // Set LINEOUT reference voltage to VDDIO/2 (1.65V) (bits 5:0) and bias current
    // (bits 11:8) to the recommended value of 0.36mA for 10kOhm load with 1nF
    // capacitance
    sgtl_WriteReg(CHIP_LINE_OUT_CTRL, 0x0322);
    
    //----------------Other Analog Block Configurations------------------
    // Configure slow ramp up rate to minimize pop (bit 0)
    sgtl_WriteReg(CHIP_REF_CTRL, 0x01FF);
    // Enable short detect mode for headphone left/right
    // and center channel and set short detect current trip level
    // to 75mA
    sgtl_WriteReg(CHIP_SHORT_CTRL, 0x1106);
    // Enable Zero-cross detect if needed for HP_OUT (bit 5) and ADC (bit 1)
    sgtl_WriteReg(CHIP_ANA_CTRL, 0x0133);
    
    //----------------Power up Inputs/Outputs/Digital Blocks-------------
    // Power up LINEOUT, HP, ADC, DAC
    sgtl_WriteReg(CHIP_ANA_POWER, 0x6AFF);
    // Power up desired digital blocks
    // I2S_IN (bit 0), I2S_OUT (bit 1), DAP (bit 4), DAC (bit 5),
    // ADC (bit 6) are powered on
    sgtl_WriteReg(CHIP_DIG_POWER, 0x0073);
    
    //--------------------Set LINEOUT Volume Level-----------------------
    // Set the LINEOUT volume level based on voltage reference (VAG)
    // values using this formula
    // Value = (int)(40*log(VAG_VAL/LO_VAGCNTRL) + 15)
    // Assuming VAG_VAL and LO_VAGCNTRL is set to 0.9V and 1.65V respectively, the
    // left LO volume (bits 12:8) and right LO volume (bits 4:0) value should be set
    // to 5
    sgtl_WriteReg(CHIP_LINE_OUT_VOL, 0x0505);
    
    // Configure SYS_FS clock to 48kHz
    // Configure MCLK_FREQ to 256*Fs
#if 0
    sgtl_ModifyReg(CHIP_CLK_CTRL, 0xFFC8, 0x0008); // bits 3:2
#else
    sgtl_ModifyReg(CHIP_CLK_CTRL, 0xFFC0, 0x0005); //modify by b43107 based on mcu-audio
#endif
    // I2S_IN -> DAC -> HP_OUT
    // Route I2S_IN to DAC
    sgtl_ModifyReg(CHIP_SSS_CTRL, 0xFFDF, 0x0010);   
    // Select DAC as the input to HP_OUT
    sgtl_ModifyReg(CHIP_ANA_CTRL, 0xFFBF, 0x0000);
    
    // TODO: Configure Microphone -> ADC -> I2S_OUT
    // Microphone -> ADC -> I2S_OUT
    // Set ADC input to Microphone
    //sgtl_ModifyReg(CHIP_ANA_CTRL, 0xFFFB, 0x0000); // bit 2
    sgtl_ModifyReg(CHIP_ANA_CTRL, 0xFFFF, 0x0004); // Set ADC input to LINE_IN
    // Route ADC to I2S_OUT
    sgtl_ModifyReg(CHIP_SSS_CTRL, 0xFFFC, 0x0000); // bits 1:0
    
    //---------------- Input Volume Control---------------------
    // Configure ADC left and right analog volume to desired default.
    // Example shows volume of 0dB
    sgtl_WriteReg(CHIP_ANA_ADC_CTRL, 0x0000);
    // Configure MIC gain if needed. Example shows gain of 20dB
    sgtl_ModifyReg(CHIP_MIC_CTRL, 0xFFFD, 0x0001); // bits 1:0
    
    //---------------- Volume and Mute Control---------------------
    // Configure HP_OUT left and right volume to minimum, unmute
    // HP_OUT and ramp the volume up to desired volume.
    sgtl_WriteReg(CHIP_ANA_HP_CTRL, 0x7F7F);
    // Code assumes that left and right volumes are set to same value
    sgtl_WriteReg(CHIP_ANA_HP_CTRL, hp_vol);
    sgtl_ModifyReg(CHIP_ANA_CTRL, 0xFFEF, 0x0000); // bit 5	
    // LINEOUT and DAC volume control
    sgtl_ModifyReg(CHIP_ANA_CTRL, 0xFEFF, 0x0000); // bit 8
    // Configure DAC left and right digital volume. Example shows
    // volume of 0dB
    sgtl_WriteReg(CHIP_DAC_VOL, dac_vol);
    sgtl_ModifyReg(CHIP_ADCDAC_CTRL, 0xFFFB, 0x0000); // bit 2
    sgtl_ModifyReg(CHIP_ADCDAC_CTRL, 0xFFF7, 0x0000); // bit 3
    // Unmute ADC
    sgtl_ModifyReg(CHIP_ANA_CTRL, 0xFFFE ,0x0000); // bit 0
    
    return (0);
}


/*_mqx_int sgtl5000_master_init(uint32_t pcmSampleRate, uint8_t bitWidth)*/
int32_t sgtl5000_master_init(uint32_t pcmSampleRate, uint8_t bitWidth)
{
    MQX_FILE_PTR i2s_ptr = NULL;
    _mqx_uint errcode = 0;
    uint_32 fs_freq = 44100;
    uint_32 mclk_freq = fs_freq * CLK_MULT;
#if BSPCFG_ENABLE_SAI
    uint_8 mode = (I2S_TX_MASTER | I2S_RX_SLAVE);
#else
    uint_8 mode = I2S_MODE_MASTER;
#endif
    i2s_ptr = fopen(AUDIO_DEVICE, "w");
    
    if (i2s_ptr == NULL)
    {
#ifdef  SGTL_DEBUG
	    	 printf("\n  InitCodec error: Unable to open audio device.");
#endif
       
        return(0xDEAD);
    }
     
    ioctl(i2s_ptr, IO_IOCTL_I2S_SET_MODE_MASTER, &mode);
    ioctl(i2s_ptr, IO_IOCTL_I2S_SET_MCLK_FREQ, &mclk_freq);
    ioctl(i2s_ptr, IO_IOCTL_I2S_SET_FS_FREQ, &fs_freq);
    //_time_delay(50);
/*
 * 
 * above delay may affect our effect
 */
    // errcode = sgtl_Init();
    if (errcode != MQX_OK)
    {
        if (fclose(i2s_ptr) != MQX_OK)
        {
#ifdef  SGTL_DEBUG
	    	   printf("\n  InitCodec error: Unable to close audio device.");
#endif
         
        }
        return (errcode);
    }
    //--------------- Power Supply Configuration----------------
    // NOTE: This next 2 Write calls is needed ONLY if VDDD is
    // internally driven by the chip
    // Configure VDDD level to 1.2V (bits 3:0)
    sgtl_WriteReg(CHIP_LINREG_CTRL, 0x0008);
    // Power up internal linear regulator (Set bit 9)
    sgtl_WriteReg(CHIP_ANA_POWER, 0x7260);
    // NOTE: This next Write call is needed ONLY if VDDD is
    // externally driven
    // Turn off startup power supplies to save power (Clear bit 12 and 13)
    sgtl_WriteReg(CHIP_ANA_POWER, 0x4260);
    // NOTE: The next 2 Write calls is needed only if both VDDA and
    // VDDIO power supplies are less than 3.1V.
    // Enable the internal oscillator for the charge pump (Set bit 11)
    sgtl_WriteReg(CHIP_CLK_TOP_CTRL, 0x0800);
    // Enable charge pump (Set bit 11)
    sgtl_WriteReg(CHIP_ANA_POWER, 0x4A60);
    // NOTE: The next 2 modify calls is only needed if both VDDA and
    // VDDIO are greater than 3.1V
    // Configure the charge pump to use the VDDIO rail (set bit 5 and bit 6)
    sgtl_WriteReg(CHIP_LINREG_CTRL, 0x006C);
    
    //------ Reference Voltage and Bias Current Configuration----------
    // NOTE: The value written in the next 2 Write calls is dependent
    // on the VDDA voltage value.
    // Set ground, ADC, DAC reference voltage (bits 8:4). The value should
    // be set to VDDA/2. This example assumes VDDA = 1.8V. VDDA/2 = 0.9V.
    // The bias current should be set to 50% of the nominal value (bits 3:1)
    sgtl_WriteReg(CHIP_REF_CTRL, 0x01FE);
    // Set LINEOUT reference voltage to VDDIO/2 (1.65V) (bits 5:0) and bias current
    // (bits 11:8) to the recommended value of 0.36mA for 10kOhm load with 1nF
    // capacitance
    sgtl_WriteReg(CHIP_LINE_OUT_CTRL, 0x0322);
    
    //----------------Other Analog Block Configurations------------------
    // Configure slow ramp up rate to minimize pop (bit 0)
    sgtl_WriteReg(CHIP_REF_CTRL, 0x01FF);
    // Enable short detect mode for headphone left/right
    // and center channel and set short detect current trip level
    // to 75mA
    sgtl_WriteReg(CHIP_SHORT_CTRL, 0x1106);
    // Enable Zero-cross detect if needed for HP_OUT (bit 5) and ADC (bit 1)
    sgtl_WriteReg(CHIP_ANA_CTRL, 0x0133);
    
    //----------------Power up Inputs/Outputs/Digital Blocks-------------
    // Power up LINEOUT, HP, ADC, DAC
    sgtl_WriteReg(CHIP_ANA_POWER, 0x6AFF);
    // Power up desired digital blocks
    // I2S_IN (bit 0), I2S_OUT (bit 1), DAP (bit 4), DAC (bit 5),
    // ADC (bit 6) are powered on
    sgtl_WriteReg(CHIP_DIG_POWER, 0x0073);
    
    //--------------------Set LINEOUT Volume Level-----------------------
    // Set the LINEOUT volume level based on voltage reference (VAG)
    // values using this formula
    // Value = (int)(40*log(VAG_VAL/LO_VAGCNTRL) + 15)
    // Assuming VAG_VAL and LO_VAGCNTRL is set to 0.9V and 1.65V respectively, the
    // left LO volume (bits 12:8) and right LO volume (bits 4:0) value should be set
    // to 5
    sgtl_WriteReg(CHIP_LINE_OUT_VOL, 0x0505);
    
    // Configure SYS_FS clock to 48kHz
    // Configure MCLK_FREQ to 256*Fs
#if 0
    sgtl_ModifyReg(CHIP_CLK_CTRL, 0xFFC8, 0x0008); // bits 3:2
#else
    sgtl_ModifyReg(CHIP_CLK_CTRL, 0xFFC0, 0x0005); //by b43107 based on mcu-audio
    
#endif
    
    // Configure the I2S clocks in SLAVE mode
    // NOTE: I2S LRCLK is same as the system sample clock
    // Data length = 16 bits
    
#if 0
    sgtl_ModifyReg(CHIP_I2S_CTRL, 0xFFFF, 0x0130); // bit 7
#else
    sgtl_ModifyReg(CHIP_I2S_CTRL, 0xFFFF, 0x0130); // bit 7
#endif
    // I2S_IN -> DAC -> HP_OUT
    // Route I2S_IN to DAC
    sgtl_ModifyReg(CHIP_SSS_CTRL, 0xFFDF, 0x0010);   
    // Select DAC as the input to HP_OUT
    sgtl_ModifyReg(CHIP_ANA_CTRL, 0xFFBF, 0x0000);
    
    // TODO: Configure Microphone -> ADC -> I2S_OUT
    // Microphone -> ADC -> I2S_OUT
    // Set ADC input to Microphone
    //sgtl_ModifyReg(CHIP_ANA_CTRL, 0xFFFB, 0x0000); // bit 2
    sgtl_ModifyReg(CHIP_ANA_CTRL, 0xFFFF, 0x0004); // Set ADC input to LINE_IN
    // Route ADC to I2S_OUT
    sgtl_ModifyReg(CHIP_SSS_CTRL, 0xFFFC, 0x0000); // bits 1:0
    
    //---------------- Input Volume Control---------------------
    // Configure ADC left and right analog volume to desired default.
    // Example shows volume of 0dB
    sgtl_WriteReg(CHIP_ANA_ADC_CTRL, 0x0000);
    // Configure MIC gain if needed. Example shows gain of 20dB
    sgtl_ModifyReg(CHIP_MIC_CTRL, 0xFFFD, 0x0001); // bits 1:0
    
    //---------------- Volume and Mute Control---------------------
    // Configure HP_OUT left and right volume to minimum, unmute
    // HP_OUT and ramp the volume up to desired volume.
    sgtl_WriteReg(CHIP_ANA_HP_CTRL, 0x7F7F);
    // Code assumes that left and right volumes are set to same value
    sgtl_WriteReg(CHIP_ANA_HP_CTRL, hp_vol);
    sgtl_ModifyReg(CHIP_ANA_CTRL, 0xFFEF, 0x0000); // bit 5	
    // LINEOUT and DAC volume control
    sgtl_ModifyReg(CHIP_ANA_CTRL, 0xFEFF, 0x0000); // bit 8
    // Configure DAC left and right digital volume. Example shows
    // volume of 0dB
    sgtl_WriteReg(CHIP_DAC_VOL, dac_vol);
    sgtl_ModifyReg(CHIP_ADCDAC_CTRL, 0xFFFB, 0x0000); // bit 2
    sgtl_ModifyReg(CHIP_ADCDAC_CTRL, 0xFFF7, 0x0000); // bit 3
    // Unmute ADC
    sgtl_ModifyReg(CHIP_ANA_CTRL, 0xFFFE ,0x0000); // bit 0
    if (fclose(i2s_ptr) != MQX_OK)
    {
#ifdef  SGTL_DEBUG
	    	printf("\n  InitCodec error: Unable to close audio device.");
#endif
        
    }
    
    return (0);
}


_mqx_int eInitCodec(void)
{
    MQX_FILE_PTR i2s_ptr = NULL;
    _mqx_uint errcode = 0;
    uint_32 fs_freq = 44100;
    uint16_t ana_pwr = 0;
    uint16_t lreg_ctrl = 0;
    
    uint16_t vdda = 3300;
    uint16_t vddio = 3300;
    uint16_t vag = 0;
    
    uint_32 mclk_freq = fs_freq * CLK_MULT;
#if BSPCFG_ENABLE_SAI
    uint_8 mode = (I2S_TX_MASTER | I2S_RX_SLAVE);
#else
    uint_8 mode = I2S_MODE_MASTER;
#endif
    i2s_ptr = fopen(AUDIO_DEVICE, "w");
    
    if (i2s_ptr == NULL)
    {
#ifdef  SGTL_DEBUG
	    	printf("\n  InitCodec error: Unable to open audio device.");
#endif
        
        return(0xDEAD);
    }
     
    ioctl(i2s_ptr, IO_IOCTL_I2S_SET_MODE_MASTER, &mode);
    ioctl(i2s_ptr, IO_IOCTL_I2S_SET_MCLK_FREQ, &mclk_freq);
    ioctl(i2s_ptr, IO_IOCTL_I2S_SET_FS_FREQ, &fs_freq);
    _time_delay(50);
/*
 * 
 * above delay may affect our effect
 */
    errcode = sgtl_Init();
    if (errcode != MQX_OK)
    {
        if (fclose(i2s_ptr) != MQX_OK)
        {
#ifdef  SGTL_DEBUG
	    	printf("\n  InitCodec error: Unable to close audio device.");
#endif
            
        }
        return (errcode);
    }
    //--------------- Power Supply Configuration----------------
    // NOTE: This next 2 Write calls is needed ONLY if VDDD is
    // internally driven by the chip
    // Configure VDDD level to 1.2V (bits 3:0)
    sgtl_WriteReg(CHIP_LINREG_CTRL, 0x0008);

    _time_delay(5);//10us
    sgtl_ReadReg(CHIP_ANA_POWER, &ana_pwr);
    sgtl_ReadReg(CHIP_LINREG_CTRL, &lreg_ctrl);
    
    sgtl_ModifyReg(CHIP_ANA_POWER, 0xF7FF, 0x0000);
    ana_pwr |= 0x4060;
    lreg_ctrl |= 0x0020;
    lreg_ctrl |= 1<<6;
    
    sgtl_WriteReg(CHIP_LINREG_CTRL, lreg_ctrl);
    sgtl_WriteReg(CHIP_ANA_POWER, ana_pwr);
    
    sgtl_ModifyReg(CHIP_LINREG_CTRL, 0x000F, 0x0008);
    
    
    if (ana_pwr & 0x0200)
    	sgtl_ModifyReg(CHIP_ANA_POWER, 0xDFFF, 0);
    else
    	sgtl_ModifyReg(CHIP_ANA_POWER, 0xCFFF, 0);
    		
    vag = vdda/2;
    
    if (vag <= 800)
    	vag = 0;
    else if (vag >= 1575)
    	vag = 0x1f;
    else
    	vag = (vag - 800) / 25;
    
    sgtl_ModifyReg(CHIP_REF_CTRL, 0xfe0f, vag << 4);
    	
    
    vag = vddio / 2;
    
    	if (vag <= 800)
    		vag = 0;
    	else if (vag >= 1375)
    		vag = 23;
    	else
    		vag = (vag - 800) / 25;
    sgtl_ModifyReg(CHIP_LINE_OUT_CTRL, 0xf0c0, vag << 0 | 3<<8);
    	
    sgtl_ModifyReg(CHIP_REF_CTRL, 0xfffe, 0x0001);
    
    sgtl_WriteReg(CHIP_SHORT_CTRL, 0x0000);
    
   
    sgtl_WriteReg(CHIP_SSS_CTRL, 1<<4);
    
    sgtl_WriteReg(CHIP_DIG_POWER, 0x0060);
    
    sgtl_WriteReg(CHIP_ADCDAC_CTRL, 0x020c);
    
    sgtl_WriteReg(CHIP_PAD_STRENGTH, 0x015f);
    
    sgtl_WriteReg(CHIP_ANA_CTRL, 0x0022);
    
    sgtl_WriteReg(CHIP_MIC_CTRL, 0x0000);
    
    sgtl_WriteReg(CHIP_DAC_VOL, 0x6060);
    
    sgtl_WriteReg(CHIP_ANA_ADC_CTRL, 0xf<<0 | 0xf<<4);
    
    sgtl_ModifyReg(CHIP_ANA_POWER, 0xff7f, 0x0000);
    _time_delay(600);
    
    
    if (fclose(i2s_ptr) != MQX_OK)
    {
#ifdef  SGTL_DEBUG
	    	 printf("\n  InitCodec error: Unable to close audio device.");
#endif
       
    }
    
    return (0);
}

/*FUNCTION****************************************************************
* 
* Function Name    : SetupCodec
* Returned Value   : MQX error code
* Comments         :
*     
*
*END*********************************************************************/
_mqx_int SetupCodec(MQX_FILE_PTR device)
{ 
    uint_32 mclk_freq;
    uint_32 fs_freq;
    uint_32 multiple;
    _mqx_int errcode = 0;
    uint_16 mask_ones, mask_zeros;
    uint_8 mode = 0;
    
    ioctl(device, IO_IOCTL_I2S_GET_MODE, &mode);
    ioctl(device, IO_IOCTL_I2S_GET_MCLK_FREQ, &mclk_freq);
    ioctl(device, IO_IOCTL_I2S_GET_FS_FREQ, &fs_freq);

    multiple = mclk_freq/fs_freq;
    /* Convert "multiple" to value required for SGTL5000 CHIP_CLK_CTRL register (bits 1:0)*/
    mask_ones = (multiple/128) - 2;
    if (mask_ones > 0x2)
    {
        return (-1);
    }
    
    switch (fs_freq)
    {            
    case 8000:
        mask_zeros = 0xFFE0;
        mask_ones |= 0x0020;
        break;
        
    case 11025:
        mask_zeros = 0xFFE4;
        mask_ones |= 0x0024;
        break;
        
    case 12000:
        mask_zeros = 0xFFE8;
        mask_ones |= 0x0028;
        break;
        
    case 16000:
        mask_zeros = 0xFFD0;
        mask_ones |= 0x0010;
        break;
        
    case 22050:
        mask_zeros = 0xFFD4;
        mask_ones |= 0x0014;
        break;
        
    case 24000:
        mask_zeros = 0xFFD8;
        mask_ones |= 0x0018;
        break;
        
    case 32000:
        mask_zeros = 0xFFC0;
        mask_ones |= 0x0000;
        break;
        
    case 44100:
        mask_zeros = 0xFFC4;
        mask_ones |= 0x0004;
        break;
        
    case 48000:
        mask_zeros = 0xFFC8;
        mask_ones |= 0x0008;
        break;
        
    case 96000:
        mask_zeros = 0xFFCC;
        mask_ones |= 0x000C;
        break;
        
    default:
        return(-2);
        break;
    }
    errcode = sgtl_ModifyReg(CHIP_CLK_CTRL, mask_zeros, mask_ones);

    if (errcode != MQX_OK)
    {
        return(-3);
    }
    /* 
    * Setup I2S mode in codec 
    */
#if BSPCFG_ENABLE_SAI
    if (((mode & I2S_RX_MASTER) && (mode & I2S_IO_READ)) || ((mode & I2S_TX_MASTER) && (mode & I2S_IO_WRITE)))
#else
    if (mode & I2S_MODE_MASTER)
#endif
    {
        mask_ones = 0x0000; 
    }
    else
    {
        mask_ones = 0x0800;
    }
    mask_zeros = 0xFF7F;
    
    errcode = sgtl_ModifyReg(CHIP_I2S_CTRL, mask_zeros, mask_ones);
    if (errcode != MQX_OK)
    {
        return(-4);
    }
    /* rise volume */
    /*As our use case, this code should be comment*/
#if 0
    sgtl_WriteReg(CHIP_DAC_VOL, 0x3C3C);
#endif
    return(0);
}

/* EOF */
