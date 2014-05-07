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
 * $FileName: codec_interface_mqx_wrapper.c$
 * $Version : 3.8.6.0$
 * $Date    : Sep-25-2012$
 *
 * Comments: Callbacks for decoder and metadata library.
 *
 *END************************************************************************/

////////////////////////////////////////////////////////////////////////////////
// Includes 
////////////////////////////////////////////////////////////////////////////////
#include <types.h>
#include <error.h>        
#include <mqx.h>
#include <fio.h>
#include "stdlib.h"
#include "string.h"
#include "cci_metadata.h"
#include "codec_support.h"
#include "codec_interface_mqx_wrapper.h"
#include "main.h"



////////////////////////////////////////////////////////////////////////////////
// Code
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//! \brief Seek Callback API for all metadata extraction.
//!
//! \fntype Function
//!
//! \param[in] fp         input file pointer
//! \param[in] Offset     seek offset
//! \param[in] type       seek type
//!
//! \retval None. 
////////////////////////////////////////////////////////////////////////////////
int32_t Fseek(int32_t fp, int32_t Offset, int32_t type)
{
	/*
	 * Not needed any more since MQX FS will do the "offset = -Offset" stuff.
	 */
	/*
    if ( type == 2 && Offset < 0 )
        Offset = -Offset;
    */
	
	/*
	 *	the type the Fseek expected is different with the ones MQX supported.
	 */
	int32_t mqx_type;
	if(2 == type){	
		mqx_type = IO_SEEK_END;
	}else if(0 == type){	
		mqx_type = IO_SEEK_SET;
	}else{	
		mqx_type = IO_SEEK_CUR;
	}
	
    return fseek((FILE_PTR )fp, Offset, mqx_type); 
}
////////////////////////////////////////////////////////////////////////////////
//! \brief Read Callback API for all metadata extraction.
//!
//! \fntype Function
//!
//! \param[in] fp         input file pointer
//! \param[in] *buf       decoder internal buffer pointer
//! \param[in] size       number of bytes to read
//!
//! \retval It returns number of bytes read. 
////////////////////////////////////////////////////////////////////////////////
int32_t Fread(int32_t fp, uint8_t *buf, int32_t size)
{  
    int32_t br = 0;
    
    br = fread(buf, sizeof(char), size, (FILE_PTR )fp);
    
    return br;
}
////////////////////////////////////////////////////////////////////////////////
//! \brief Ftell Callback API for all metadata extraction.
//!
//! \fntype Function
//!
//! \param[in] fp         input file pointer
//!
//! \retval It returns file current position. 
////////////////////////////////////////////////////////////////////////////////
int32_t Ftell(int32_t fp)
{
    return ftell((FILE_PTR)fp);
}
////////////////////////////////////////////////////////////////////////////////
//! \brief close file Callback API for all metadata extraction.
//!
//! \fntype Function
//!
//! \param[in] fp         input file pointer
//!
//! \retval None. 
////////////////////////////////////////////////////////////////////////////////
int32_t Fclose(int32_t fp)
{
    return fclose((FILE_PTR)fp);
}
////////////////////////////////////////////////////////////////////////////////
//! \brief get file size Callback API for all metadata extraction.
//!
//! \fntype Function
//!
//! \param[in] fp         input file pointer
//!
//! \retval None. 
////////////////////////////////////////////////////////////////////////////////

int32_t GetFileSize(int32_t fp)
{
	int32_t offset; 
    int32_t size;
    
    offset = ftell((FILE_PTR)fp); 
    fseek((FILE_PTR)fp,0,IO_SEEK_END); 
    size = ftell((FILE_PTR)fp);  
    fseek((FILE_PTR)fp,offset,IO_SEEK_SET);  
    
    return size; 
}
//////////////////////////////////////////////////////////////////////////////////
// Stub functions
//////////////////////////////////////////////////////////////////////////////////
// b44566 note: this section is already in codecextractmetadata.c but is disabled when I2S_DEMO Macro is enable
uint8_t txt_ConvertDbcsToUtf16c(char* ptr, uint16_t* pUnicodeValue)
{
    * pUnicodeValue = *ptr;
    
    return 1;
}

uint8_t txt_ConvertUtf8ToUtf16c(char* ptr, uint16_t* pUnicodeValue)
{
    * pUnicodeValue = *ptr;
    return 1;
}

////////////////////////////////////////////////////////////////////////////////
//! \brief Read Callback API for all codecs.
//!
//! \fntype Function
//!
//! \param[in] StreamNo         Stream No of input file
//! \param[in] *DecoderBuffer  Decoder internal buffer
//! \param[in] SizeToRead       Number of bytes to read
//! \param[in] *UserData       user data. It may be file handle
//!
//! \retval It returns number of samples read. 
////////////////////////////////////////////////////////////////////////////////
int32_t get_file_data(int32_t StreamNo, uint8_t *DecoderBuffer,
		int32_t SizeToRead, int32_t updatetail, void *UserData) {

	int32_t br = 0;

	br = fread(DecoderBuffer, sizeof(char), SizeToRead, (FILE_PTR )UserData);

	return br;
}
////////////////////////////////////////////////////////////////////////////////
//! \brief Seek Callback for all codecs.
//!
//! \fntype Function
//!
//! \param[in] StreamNo     Stream No of input file
//! \param[in] offset       seek offset
//! \param[in] position     seek position
//! \param[in] *UserData    user data. It may be file handle
//!
//! \retval None
////////////////////////////////////////////////////////////////////////////////
int32_t seek_file_data(int32_t StreamNo, int32_t offset, int32_t position,
		void *UserData) {
	int32_t mqx_type;
/*
	if (position == 2 && offset < 0)
		offset = -offset;
*/
	if (2 == position) {
		mqx_type = IO_SEEK_END;
	} else if (0 == position) {
		mqx_type = IO_SEEK_SET;
	} else if(1 == position){
		mqx_type = IO_SEEK_CUR;
	}

	return fseek((FILE_PTR) UserData, offset, mqx_type);
}
////////////////////////////////////////////////////////////////////////////////
//! \brief Ftell Callback for all codecs.
//!
//! \fntype Function
//!
//! \param[in] StreamNo     Stream No of input file
//! \param[in] *UserData   	user data. It may be file handle
//!
//! \retval It returns the current file position.
////////////////////////////////////////////////////////////////////////////////
int32_t get_file_position(int32_t StreamNo, void *UserData) {
	return ftell((FILE_PTR) UserData);
}

////////////////////////////////////////////////////////////////////////////////
//! \brief write output data all codecs.
//!
//! \fntype Reentrant Function
//!
//! \param[in] unsigned char *Buf Decoder output buffer
//! \param[in] int samplestowrite Number of samples to write
//! \param[in] void *UserData user data. It may be file handle
//!
//! \return Status of the call.
//! \retval SUCCESS
//! \retval ERROR_ASI_DFFI_INIT
//!
////////////////////////////////////////////////////////////////////////////////
#if 0
int32_t put_file_data(uint8_t *Buf, int32_t samplestowrite, int32_t *UserData) {
	int32_t bw = 0;

//	lwgpio_toggle_value(&pin1);
	//lwgpio_set_value(&pin1, LWGPIO_VALUE_HIGH);
	bw = fwrite(Buf, sizeof(char), samplestowrite, (FILE_PTR )UserData);
	//lwgpio_set_value(&pin1, LWGPIO_VALUE_LOW);

	return bw;
}
#else   //modify by b43107 
int32_t put_file_data(uint8_t *Buf, int32_t samplestowrite, int32_t *UserData) {
	int32_t bw = 0;

	//bw = fwrite(Buf, sizeof(char), samplestowrite, (FILE_PTR )UserData);
	bw = msi_snd_write(Buf, samplestowrite/2);

	return bw;
}

#endif
////////////////////////////////////////////////////////////////////////////////
// End of file
////////////////////////////////////////////////////////////////////////////////
//! @}

