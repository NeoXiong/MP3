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
 * $FileName: codec_interface_mqx_wrapper.h$
 * $Version : 3.8.6.0$
 * $Date    : Sep-25-2012$
 *
 * Comments: Callbacks for decoder and metadata library.
 *
 *END************************************************************************/
 #include <types.h>

 int32_t Fseek(int32_t fp, int32_t Offset, int32_t type);
 int32_t Fread(int32_t fp, uint8_t *buf, int32_t size);
 int32_t Ftell(int32_t fp);
 int32_t Fclose(int32_t fp);
 int32_t GetFileSize(int32_t fp);
 uint8_t txt_ConvertDbcsToUtf16c(char* ptr, uint16_t* pUnicodeValue);
 uint8_t txt_ConvertUtf8ToUtf16c(char* ptr, uint16_t* pUnicodeValue);
 
 int32_t get_file_data(int32_t StreamNo, uint8_t *DecoderBuffer,
		int32_t SizeToRead, int32_t updatetail, void *UserData);
int32_t seek_file_data(int32_t StreamNo, int32_t offset, int32_t position,
		void *UserData);
int32_t get_file_position(int32_t StreamNo, void *UserData);
int32_t put_file_data(uint8_t *Buf, int32_t samplestowrite, int32_t *UserData);
