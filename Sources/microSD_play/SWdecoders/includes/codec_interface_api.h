////////////////////////////////////////////////////////////////////////////////
//! \addtogroup codec_interface_API_Header_File Codec Interface APIs header file
//! @{
//
// Copyright (c) 2013-2014 Freescale, Inc.
//
//! \file codec_interface_api.h
//! \brief Contains common codec interface APIs definations.
////////////////////////////////////////////////////////////////////////////////
#ifndef __codec_interface_api_h__
#define __codec_interface_api_h__

////////////////////////////////////////////////////////////////////////////////
//  Includes
////////////////////////////////////////////////////////////////////////////////
#include "types.h"
#include "common_interface.h"
#include "metadata.h"
#include "error.h"

////////////////////////////////////////////////////////////////////////////////
//  Prototypes
////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
//! \brief codec extract meta data top level function.
//! 
//! \fntype Function
//!
//! \param[in] *packedFilePath           Pointer to the file path and name.
//! \param[in] *metaData                 Metadata extracted from the file.
//! \param[in] fileHandle                file handle.
//! 
//! \retval kCodecSuccess                if metadata found successfully. 
//! \retval kCodecMetadataFiletypeFailed if metadata not found
/////////////////////////////////////////////////////////////////////////////////
int32_t codec_extract_meta_data(uint8_t *packedFilePath, file_meta_data_t *metaData, int32_t fileHandle);
/////////////////////////////////////////////////////////////////////////////////
//! \brief codec get memory information function.
//!
//! \fntype Function
//!
//! \param[in] streamType       Audio Stream Type.
//!
//! \retval it returns the size of the codec. 
/////////////////////////////////////////////////////////////////////////////////
int32_t codec_get_mem_info(audio_stream_type_t streamType);
/////////////////////////////////////////////////////////////////////////////////
//! \brief codec initialization function.
//!
//! \fntype Function
//!
//! \param[in] streamType       Audio Stream Type.
//! \param[in] *memory          This is pointer to the decoder memory.
//! \param[in] *callbackFn      This is pointer to the decoder callback functions.
//! \param[in] *userData        This is pointer to the user specific data.
//!
//! \retval kCodecSuccess       if codec initializes successfully.
//! \retval kCodecInitError     if codec fails to initialize.  
/////////////////////////////////////////////////////////////////////////////////
int32_t codec_init(audio_stream_type_t streamType,int32_t *memory[],int32_t *callbackFn[],int32_t *userData);
/////////////////////////////////////////////////////////////////////////////////
//! \brief codec decoding function.
//!
//! \fntype Function
//!
//! \param[in] streamType           Audio Stream Type.
//! \param[in] *memory              This is pointer to the decoder memory.
//! \param[in] *sampleProudced      This is pointer to the samples produced by the codec.
//! \param[in] *bufOuts             This is pointer to the putput pcm buffer.
//!
//! \retval kCodecSuccess           if codec decodes frame successfully.
//! \retval kCodecDecodeError       if codec fails to decode frame.
//! \retval kCodeEndOfDecode        if decoding of input file is done.    
/////////////////////////////////////////////////////////////////////////////////
int32_t codec_decode(audio_stream_type_t streamType,int32_t *memory[],int32_t *sampleProudced,int32_t *bufOut);
/////////////////////////////////////////////////////////////////////////////////
//! \brief codec get pcm samples function.
//!
//! \fntype Function
//!
//! \param[in] streamType          Audio Stream Type.
//! \param[in] *memory             This is pointer to the decoder memory.
//! \param[in] *sampleProucded     This is pointer to the samples produced by the codec.
//! \param[in] *bufOut             This is pointer to the user specific data.
//!
//! \retval It returns number of samples get from the codec.
/////////////////////////////////////////////////////////////////////////////////
int32_t codec_get_pcm_samples(audio_stream_type_t streamType,int32_t *memory[],int32_t *sampleProucded,int32_t *bufOut);
/////////////////////////////////////////////////////////////////////////////////
//! \brief codec reset function.
//!
//! \fntype Function
//!
//! \param[in] streamType          Audio Stream Type.
//! \param[in] *memory             This is pointer to the decoder memory.
//!
//! \retval kCodecSuccess          if codec resets successfully.
//! \retval kCodecResetError       if codec fails to reset.
/////////////////////////////////////////////////////////////////////////////////
int32_t codec_reset(audio_stream_type_t streamType,int32_t *memory[]);
/////////////////////////////////////////////////////////////////////////////////
//! \brief codec seek function.
//!
//! \fntype Function
//!
//! \param[in] streamType           Audio Stream Type.
//! \param[in] *memory              This is pointer to the decoder memory.
//! \param[in] byteToSeek           This is number of bytes to seek.
//!
//! \retval It returns frame boundary align offset.
/////////////////////////////////////////////////////////////////////////////////
int32_t codec_seek(audio_stream_type_t streamType,int32_t *memory[],int32_t byteToSeek);


#endif /* __common_interface_h__ */

///////////////////////////////////////////////////////////////////////////////
// End of file
///////////////////////////////////////////////////////////////////////////////
//! @}
