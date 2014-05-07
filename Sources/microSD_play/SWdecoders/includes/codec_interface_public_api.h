/*! @addtogroup codec_interface_API_Header_File Codec Interface APIs header file
 *  @{
 *
 * Copyright (c) 2013-2014 Freescale, Inc.
 */
/*! @file codec_interface_public_api.h
 *  @brief Contains common codec interface APIs definitions.
 */
#ifndef __CODEC_INTERFACE_PUBLIC_API_H_
#define __CODEC_INTERFACE_PUBLIC_API_H_

/*
 *  Includes
 */
#include "error.h"

#define I2S_DEMO   // add by b43107

#ifndef I2S_DEMO
#include "osa_common.h"
#endif
#if defined(__cplusplus)
extern "C" {
#endif
/*
 * Enums
 */
/*!
 * @brief Enum Stream type structure
 * This enum is used as an input to all CCI functions
 * and is used as the index into the function table. 
 * @warn do not change the order of this enum or wrong functions will be called 
 */
typedef enum _stream_type
{
    STREAM_TYPE_UNKNOWN = 0, /*!< 0 = stream type is unknown*/ 
    STREAM_TYPE_MP3,         /*!< 1 = stream type is MP3 */
    STREAM_TYPE_WMA,         /*!< 2 = stream type is WMA */
    STREAM_TYPE_AAC,         /*!< 3 = stream type is AAC */
    STREAM_TYPE_OGG,         /*!< 4 = stream type is OGG */
    STREAM_TYPE_FLAC,        /*!< 5 = stream type is FLAC */
    STREAM_TYPE_APE,         /*!< 6 = stream type is APE */
    STREAM_TYPE_PCM,         /*!< 7 = stream type is PCM */
    STREAM_TYPE_IMAADPCM,    /*!< 8 = stream type is IMA-ADPCM */
    STREAM_TYPE_MSADPCM,     /*!< 9 = stream type is MS-ADPCM */
    STREAM_TYPE_ALAC,        /*!< 10 = stream type is ALAC */
    STREAM_TYPE_OPUS,        /*!< 11 = stream type is OPUS */
    STREAM_TYPE_COUNT,       /*!< count of stream type */
} audio_stream_type_t;

/*!
 * @brief Error codes used by CCI
 */
typedef enum _codec_error_code
{
    CODEC_UNSUPPORTED_FUNCTION = -1, /*!< -1 = codec unsupported function */
    CODEC_SUCCESS = 0,           /*!< 0 = codec function execute succesfully */
    CODEC_INIT_ERROR,             /*!< 1 = Init fails */
    CODEC_DECODE_ERROR,           /*!< 2 = Decode fails */
    CODEC_END_OF_DECODE,           /*!< 3 = End of decode */
    CODEC_RESET_ERROR,            /*!< 4 = Reset fails */
    CODEC_SEEK_ERROR,             /*!< 5 = Seek fails */
    CODEC_MORE_DATA_REQUIRED,       /*!< 6 = More data requires */
    CODEC_METADATA_FILETYPE_FAILED, /*!< 7 = codec metadata extraction fails */
    CODEC_METADATA_NOT_FOUND,       /*!< 8 = codec metadata not found */
    CODEC_METADATA_UNSUPPORTED_FILE, /*!< 9 = unsupported metadata */
    CODEC_FILE_OPEN_ERROR,           /*!< 10 = file open fails */
    CODEC_FSEEK_FAILED,             /*!< 11 = file seek fails */
    CODEC_MEDIA_READ_ERROR,         /*!< 12 = codec media read fails */
    CODEC_BROKEN_FRAME,           /*!< 13 = codec broken frame */
} codec_error_code_t; 

/*!
 * @brief codec media sub type structure
 *       @todo is this used?? if so, how is it different from audio_stream
 */
typedef enum _codec_media_sub_type
{
    MEDIA_SUBTYPE_UNKNOWN = 0, /*!< 0 = unknown media sub type */ 
    MEDIA_SUBTYPE_NONE,        /*!< 1 = there is no sub type */
    MEDIA_SUBTYPE_ADTS,        /*!< 8 = ADTS sub type of aac */
    MEDIA_SUBTYPE_M4A,         /*!< 9 = M4A media sub type */
    MEDIA_SUBTYPE_COUNT,       /*!< count of sub types */
} codec_media_sub_type_t;


/*
 *  Prototypes
 */
/*! @brief codec get input/output frame size in bytes.
 *
 * @fntype Function
 *
 * @param[in] stream_type    Audio Stream Type.
 * @param[out] in_size       Input Frame Size.
 * @param[out] out_size      Output Frame Size. 
 *
 * @retval it returns the input/output frame size in bytes of the codec. 
 * @retval CODEC_UNSUPPORTED_FUNCTION  If this function is not supported for 
 *                                    streamtype.  
 */
int32_t codec_get_io_frame_size(audio_stream_type_t stream_type, 
                                        int32_t *in_size, int32_t *out_size);
/*! @brief codec get memory information function.
 *
 * @fntype Function
 *
 * @param[in] stream_type       Audio Stream Type.
 *
 * @retval It returns the size of the codec. 
 * @retval CODEC_UNSUPPORTED_FUNCTION  If this function is not supported for 
 *                                    streamtype.  
 */
int32_t codec_get_mem_info(audio_stream_type_t stream_type);
/*! @brief codec initialization function.
 *
 * @fntype Function
 *
 * @param[in] stream_type     Audio Stream Type.
 * @param[in] *memory        This is pointer to the decoder memory.
 * @param[in] *callback_fn    This is pointer to the decoder callback functions.
 * @param[in] *user_data      This is pointer to the user specific data.
 *
 * @retval CODEC_SUCCESS     if codec initializes successfully.
 * @retval CODEC_INIT_ERROR   if codec fails to initialize.  
 * @retval CODEC_UNSUPPORTED_FUNCTION  If this function is not supported for 
 *                                    streamtype    
 */
int32_t codec_init(audio_stream_type_t stream_type, int32_t *memory[], 
                                    int32_t *callback_fn[], int32_t *user_data);

/*! @brief codec decoding function.
 *
 * @fntype Function
 *
 * @param[in] stream_type           Audio Stream Type.
 * @param[in] *memory              This is pointer to the decoder memory.
 * @param[in] *sampleProudced      This is pointer to the samples produced by 
 *                                 the codec.
 * @param[in] *bufOuts             This is pointer to the decoder output pcm 
 *                                 buffer.
 *
 * @retval CODEC_SUCCESS           if codec decodes frame successfully.
 * @retval CODEC_DECODE_ERROR       if codec fails to decode frame.
 * @retval CODEC_END_OF_DECODE        if decoding of input file is done.    
 * @retval CODEC_UNSUPPORTED_FUNCTION  If this function is not supported for 
 *                                    streamtype        
 */
int32_t codec_decode(audio_stream_type_t stream_type, int32_t *memory[],
                                    int32_t *sample_produced, int32_t *buf_out);

/*! @brief codec get pcm samples function.
 *
 * @fntype Function
 *
 * @param[in] stream_type          Audio Stream Type.
 * @param[in] *memory             This is pointer to the decoder memory.
 * @param[in] *sampleProucded     This is pointer to the samples produced by 
 *                                the codec.
 * @param[in] *buf_out             This is pointer to the user specific data.
 *
 * @retval It returns number of samples get from the codec.
 */
int32_t codec_get_pcm_samples(audio_stream_type_t stream_type, int32_t *memory[],
                                            int32_t *sample_produced, int32_t *buf_out);

/*! @brief codec reset function.
 *
 * @fntype Function
 *
 * @param[in] stream_type          Audio Stream Type.
 * @param[in] *memory             This is pointer to the decoder memory.
 *
 * @retval CODEC_SUCCESS          if codec resets successfully.
 * @retval CODEC_RESET_ERROR       if codec fails to reset.
 * @retval CODEC_UNSUPPORTED_FUNCTION  If this function is not supported for 
 *                                    streamtype        
 */
int32_t codec_reset(audio_stream_type_t stream_type, int32_t *memory[]);
/*! @brief codec seek function. 
 *
 * @param[in] stream_type          Audio Stream Type.
 * @param[in] *memory             Pointer to decoder memory allocated based on 
 *                                codec_get_mem_info() and passed in by caller.
 * @param[in] byteToSeek          Number of bytes to seek.
 *
 * @retval frame boundary align offset.
 * @retval CODEC_UNSUPPORTED_FUNCTION  If this function is not supported for 
 *                                    streamtype        
 */
int32_t codec_seek(audio_stream_type_t stream_type,int32_t *memory[], 
                                                            int32_t byte_to_seek);

#if defined(__cplusplus)
}
#endif

#endif /* __CODEC_INTERFACE_PUBLIC_API_H_ */

/*
 * End of file
 * @}
 */
