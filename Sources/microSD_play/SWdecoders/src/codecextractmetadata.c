/*
 * $copyright$
 *
 * $license$
 *
 */

/*!
 * @file    codecextractmetadata.c
 * @brief   This file Contains functionality that determines the file type and
 *        then calls the proper extract meta_data function
 *
 */

/*
 *  Includes
 */
#define I2S_DEMO //add by b43107

#ifndef I2S_DEMO
#include "impresario_cfg.h"
#else
#include <string.h>
#include "types.h"
#include "codec_support.h"
#endif
#include "codec_interface_public_api.h"
#include "cci_metadata.h"

/*
 *  Definitions
 */
#define SHORT_FILENAME_LENGTH 13 
#define FILE_EXTENSION_MP3    0x33504d
#define FILE_EXTENSION_WMA    0x414D57
#define FILE_EXTENSION_WAV    0x564157
#define FILE_EXTENSION_OGG    0x47474F
#define FILE_EXTENSION_M4P    0x50344d       /* APPLE: correct extension is M4P
                                                       not MP4 */
#define FILE_EXTENSION_M4A    0x41344d
#define FILE_EXTENSION_MP4    0x34504D
#define FILE_EXTENSION_AAC    0x434141
#define FILE_EXTENSION_CAF    0x464143
#define FILE_EXTENSION_FLAC   0x414C46
#define FILE_EXTENSION_APE    0x455041
#define FILE_EXTENSION_OPUS	  0x55504F
/* Enable Long Filename Extensions */
#define LFN_SUPPORT

/*
 * Prototypes
 */
 
/*
 * Stub functions
 */
#ifndef I2S_DEMO
uint8_t txt_ConvertDbcsToUtf16c(char* ptr, uint16_t* unicode_value)
{
    *unicode_value = *ptr;
    return 1;
}

uint8_t txt_ConvertUtf8ToUtf16c(char* ptr, uint16_t* unicode_value)
{
    *unicode_value = *ptr;
    return 1;
}
#endif

/*! @brief CCI extract metadata top level function for direct file interface.
 * 
 * @fntype Function
 *
 * @param[in] file_extension             File Extension.
 * @param[in] *meta_data                 meta_data extracted from the file.
 * @param[in] *ctx           	        Pointer to CCI Context structure.
 * 
 * @retval CODEC_SUCCESS                if meta_data found successfully. 
 * @retval CODEC_METADATA_FILETYPE_FAILED if meta_data not found
 */
int32_t cci_extract_meta_data(uint32_t file_extension, 
                                file_meta_data_t *meta_data, CCI_Ctx *ctx)
{
    int32_t ret = CODEC_METADATA_FILETYPE_FAILED;

    switch(file_extension){
#ifdef FASCFG_ENABLE_WAV_CODEC
    case FILE_EXTENSION_WAV:
            ret = (int32_t)codec_extract_metadata_wave(meta_data, ctx);
            break;
#endif
#ifdef FASCFG_ENABLE_FLAC_CODEC
    case FILE_EXTENSION_FLAC:
            ret = (int32_t)codec_extract_metadata_flac(meta_data, ctx);     
            break;
#endif
#ifdef FASCFG_ENABLE_MP3_CODEC
    case FILE_EXTENSION_MP3:
        ret = (int32_t)codec_extract_metadata_mp3(meta_data, ctx);
             break;
#endif

#ifdef FASCFG_ENABLE_AAC_CODEC
        case FILE_EXTENSION_AAC:
        case FILE_EXTENSION_MP4:
        case FILE_EXTENSION_M4A:
              ret = (int32_t)codec_extract_metadata_aac(meta_data, ctx);
            break;
#endif        	
        default:
            break;
        } 

    return(ret);
}
/*! @brief CCI extract metadata top level function for streamer interface.
 * 
 * @fntype Function
 *
 * @param[in] stream_type                Audio Stream Type.
 * @param[in] *meta_data                 meta_data extracted from the file.
 * @param[in] *ctx           	        Pointer to CCI Context structure.
 * 
 * @retval CODEC_SUCCESS                if meta_data found successfully. 
 * @retval CODEC_METADATA_FILETYPE_FAILED if meta_data not found
 */

int32_t ccidec_extract_meta_data(audio_stream_type_t stream_type, 
                                     file_meta_data_t *meta_data, CCI_Ctx *ctx)
{
    uint8_t ret = CODEC_METADATA_FILETYPE_FAILED;

    switch(stream_type){
#ifdef FASCFG_ENABLE_WAV_CODEC
    case STREAM_TYPE_PCM:
        ret = (uint8_t)codec_extract_metadata_wave(meta_data, ctx);
            break;
#endif
#ifdef FASCFG_ENABLE_FLAC_CODEC
    case STREAM_TYPE_FLAC:
        ret = (uint8_t)codec_extract_metadata_flac(meta_data, ctx);     
            break;
#endif
#ifdef FASCFG_ENABLE_MP3_CODEC
    case STREAM_TYPE_MP3:
        ret = (uint8_t)codec_extract_metadata_mp3(meta_data, ctx);
            break;           
#endif
#ifdef FASCFG_ENABLE_AAC_CODEC
    case STREAM_TYPE_AAC:
        ret = (uint8_t)codec_extract_metadata_aac(meta_data, ctx);
            break;
#endif
        default:
        /* Unknown Audio Stream Type (kCodecStreamTypeUnknown) */
            break;
        } 

    return(ret);
}

/*
 * End of file
 * @}
 */
 
