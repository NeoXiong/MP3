/*
 * $copyright$
 *
 * $license$
 *
 */

/*!
 * @file    codec_interface.c
 * @brief   This file contains the CCI decoder interface APIs definitions.
 *
 */

/*
 *  Includes
 */

#define I2S_DEMO  //add by b43107

#ifdef I2S_DEMO
#include "types.h"
#include "codec_support.h"
#include "codec_interface_public_api.h"
#endif
#include "codec_interface.h"

/*
 * Codec Interface APIs Table
 */
const codec_interface_function_table_t g_codec_function_table[STREAM_TYPE_COUNT] =
{
    {
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
     },
#ifdef FASCFG_ENABLE_MP3_CODEC
    {
        &MP3DecoderGetMemorySize,
        &MP3DecoderInit,
        &MP3DecoderDecode,
        NULL,
        NULL,
        &MP3DecoderSeek,
        &MP3DecoderGetIOFrameSize,
     },
#else
    {
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
     },
     
#endif
#ifdef FASCFG_ENABLE_WMA_CODEC
    {
        &WMADecoderGetMemorySize,
        &WMADecoderInit,
        &WMADecoderDecode,
        &WMADecoderGetPcmSamples,
        NULL,
        &WMADecoderSeek,
        &WMADecoderGetIOFrameSize,
    },
#else
    {
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
     },

#endif    
#ifdef FASCFG_ENABLE_AAC_CODEC    
    {
        &AACDecoderGetMemorySize,
        &AACDecoderInit,
        &AACDecoderDecode,
        NULL,
        &AACDecoderReset,
        &AACDecoderSeek,
        &AACDecoderGetIOFrameSize,
    },
#else
    {
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
     },

#endif     
#ifdef FASCFG_ENABLE_OGG_CODEC      
    {
        &VORBISDecoderGetMemorySize,
        &VORBISDecoderInit,
        &VORBISDecoderDecode,
        NULL,
        &VORBISDecoderReset,
        &VORBISDecoderSeek,
        &VORBISDecoderGetIOFrameSize,
    },
#else
    {
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
     },

#endif    
#ifdef FASCFG_ENABLE_FLAC_CODEC       
    {
        &FLACDecoderGetMemorySize,
        &FLACDecoderInit,
        &FLACDecoderDecode,
        NULL,
        &FLACDecoderReset,
        &FLACDecoderSeek,
        &FLACDecoderGetIOFrameSize,
    },
#else
    {
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
     },

#endif
#ifdef FASCFG_ENABLE_APE_CODEC    	
    {
        &APEDecoderGetMemorySize,
        &APEDecoderInit,
        &APEDecoderDecode,
        NULL,
        &APEDecoderReset,
        &APEDecoderSeek,
        &APEDecoderGetIOFrameSize,
    },
#else
    {
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
     },

#endif    
#ifdef FASCFG_ENABLE_WAV_CODEC    
    {
        &WAVDecoderGetMemorySize,
        &WAVDecoderInit,
        &WAVDecoderDecode,
        NULL,
        &WAVDecoderReset,
        &WAVDecoderSeek,
        &WAVDecoderGetIOFrameSize,
    },
    {
        &WAVDecoderGetMemorySize,
        &WAVDecoderInit,
        &WAVDecoderDecode,
        NULL,
        &WAVDecoderReset,
        &WAVDecoderSeek,
        &WAVDecoderGetIOFrameSize,
    },
    {
        &WAVDecoderGetMemorySize,
        &WAVDecoderInit,
        &WAVDecoderDecode,
        NULL,
        &WAVDecoderReset,
        &WAVDecoderSeek,
        &WAVDecoderGetIOFrameSize,
    },
#else
    {
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
     },
    {
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
     },
    {
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
     },

#endif    
#ifdef FASCFG_ENABLE_ALAC_CODEC
    {
        &ALACDecoderGetMemorySize,
        &ALACDecoderInit,
        &ALACDecoderDecode,
        NULL,
        NULL,
        &ALACDecoderSeek,
        &ALACDecoderGetIOFrameSize,
     },
#else
    {
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
     },
  
#endif
#ifdef FASCFG_ENABLE_OPUS_CODEC
    {
        &OPUSDecoderGetMemorySize,
        &OPUSDecoderInit,
        &OPUSDecoderDecode,
        NULL,
        NULL,
        &OPUSDecoderSeek,
        &OPUSDecoderGetIOFrameSize,
     },
#else
    {
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
     },
  
#endif
     
};
/*
 * Code
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
                                           int32_t *in_size, int32_t *out_size)
{
    int32_t ret = 0;
    int32_t input_size = 0;
    int32_t output_size = 0;
    

    if ( NULL != g_codec_function_table[stream_type].GetIOFrameSize ){ 
        ret = g_codec_function_table[stream_type].GetIOFrameSize(&input_size, 
                                                                &output_size);
        *in_size = input_size;
        *out_size = output_size;  
    } else {
        *in_size = 0;
        *out_size = 0;  
        ret = CODEC_UNSUPPORTED_FUNCTION;
    } 

    return ret;
}

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
int32_t codec_get_mem_info(audio_stream_type_t stream_type)
{
    int32_t ret = 0;

    if ( NULL != g_codec_function_table[stream_type].GetMemorySizes ) 
        ret = g_codec_function_table[stream_type].GetMemorySizes();
    else
        ret = CODEC_UNSUPPORTED_FUNCTION; 

    return ret;
}
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
int32_t codec_init(audio_stream_type_t stream_type,int32_t *memory[],
                                        int32_t *callback_fn[],int32_t *user_data)

{
    int32_t ret = 0;

    if (NULL != g_codec_function_table[stream_type].Init)
        ret = g_codec_function_table[stream_type].Init(memory, callback_fn,
                                                      (int32_t *)&user_data[0]);
    else
        ret = CODEC_UNSUPPORTED_FUNCTION; 

    return ret;
}
/*! @brief codec decoding function.
 *
 * @fntype Function
 *
 * @param[in] stream_type           Audio Stream Type.
 * @param[in] *memory              This is pointer to the decoder memory.
 * @param[in] *sample_produced      This is pointer to the samples produced by 
 *                                 the codec.
 * @param[in] *buf_out             This is pointer to the decoder output pcm 
 *                                 buffer.
 *
 * @retval CODEC_SUCCESS           if codec decodes frame successfully.
 * @retval CODEC_DECODE_ERROR       if codec fails to decode frame.
 * @retval CODEC_END_OF_DECODE        if decoding of input file is done.    
 * @retval CODEC_UNSUPPORTED_FUNCTION  If this function is not supported for 
 *                                    streamtype        
 */
int32_t codec_decode(audio_stream_type_t stream_type,int32_t *memory[],
                                        int32_t *sample_produced,int32_t *buf_out)

{
    int32_t ret = 0;
    int32_t produced_samples = 0;
    int32_t buf;

    if (NULL != g_codec_function_table[stream_type].Decode){

        ret = g_codec_function_table[stream_type].Decode(memory, &produced_samples,
                                                                         &buf);
        *sample_produced = produced_samples;
        *buf_out = buf;  
         
    } else {
        ret = CODEC_UNSUPPORTED_FUNCTION;
        *sample_produced = produced_samples;
        *buf_out = buf;  
    }

    return ret;
}
/*! @brief codec get pcm samples function.
 *
 * @fntype Function
 *
 * @param[in] stream_type          Audio Stream Type.
 * @param[in] *memory             This is pointer to the decoder memory.
 * @param[in] *sample_produced     This is pointer to the samples produced by 
 *                                the codec.
 * @param[in] *buf_out             This is pointer to the user specific data.
 *
 * @retval It returns number of samples get from the codec.
 */
int32_t codec_get_pcm_samples(audio_stream_type_t stream_type,int32_t *memory[],
                                        int32_t *sample_produced, int32_t *buf_out)

{
    int32_t ret = 0;
    int32_t produced_samples = 0;
    int32_t buf;

    if (NULL != g_codec_function_table[stream_type].GetPcmSamples){
        ret = g_codec_function_table[stream_type].GetPcmSamples(memory, 
                                                    &produced_samples, 64, &buf);    
        *sample_produced = produced_samples;
        *buf_out = buf;
        return ret;  
    } else {
        return *sample_produced;
    }

}
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
int32_t codec_reset(audio_stream_type_t stream_type, int32_t *memory[])
{
    int32_t ret = 0;

    if (NULL != g_codec_function_table[stream_type].Reset)
        ret = g_codec_function_table[stream_type].Reset(memory);
    else
        ret = CODEC_UNSUPPORTED_FUNCTION;

    return ret;
}
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
                                                            int32_t byte_to_seek)
{
    int32_t ret = 0;

    if (NULL != g_codec_function_table[stream_type].FileSeek){  
        ret = g_codec_function_table[stream_type].FileSeek(memory, byte_to_seek);
    }else{
        ret = CODEC_UNSUPPORTED_FUNCTION;
    }

    return ret;
}
/*
 * End of file
 * @}
 */
