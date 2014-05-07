/*! @addtogroup cci
 *  @{
 *
 * Copyright (c) 2013-2014 Freescale, Inc.
 */
/*! @file codec_interface.h
 *  @brief Contains codec interface definitions.
 */
#ifndef __CODEC_INTERFACE_H__
#define __CODEC_INTERFACE_H__

/*
 *  Includes
 */
#include "codec_interface_public_api.h"
#if defined(__cplusplus)
extern "C" {
#endif
/*
 * Structure
 */

/*!
 * @brief Declares codec_get_memory_sizes_function_t as a ptr to a function that
 *                                                   calls codec get memory size
 */
typedef int32_t (*codec_get_memory_sizes_function_t)(void);
/*!
 * @brief Declares codec_init_function_t as a pointer to a function that calls
 *                                                                    codec init
 */
typedef int32_t (*codec_init_function_t)( int32_t *memory[],
                                      int32_t *callback_fn[], int32_t *user_data);
/*! 
 * @brief Declares codec_decode_function_t as a pointer to a function that calls
 *                                                                  codec decode
 */
typedef int32_t (*codec_decode_function_t)(int32_t *memory[],
                                    int32_t *sample_produced, int32_t *buf_out);
/*!
 * @brief Declares codec_get_pcm_samples_function_t as a ptr to a function that
 *                                                   calls codec get pcm samples
 */
typedef int32_t (*codec_get_pcm_samples_function_t)(int32_t *memory[],
                int32_t *sample_produced, int32_t max_sample, int32_t *buf_out );
/*!
 * @brief Declares codec_reset_function_t as a pointer to a function that calls
 *                                                                   codec reset  
 */
typedef int32_t (*codec_reset_function_t)(int32_t *memory[]);
/*!
 * @brief Declares codec_file_seek_function_t as a pointer to a function that
 *                                                             calls codec seek
 */
typedef int32_t  (*codec_file_seek_function_t)(int32_t  *memory[], int32_t seek_offset);
/*!
 * @brief Declares codec_get_input_frame_size_function_t as a ptr to a 
 *                                   function that calls codec get input frame size
 */
typedef int32_t (*codec_get_io_frame_size_function_t)(int32_t *in_size, 
                                                                int32_t *out_size);


/*!
 * @brief Codec Interface Function Table
 */
typedef struct _codec_interface_function_table
{
    codec_get_memory_sizes_function_t     	GetMemorySizes; /*!< get size of
                                                     required memory for codec*/                                                                 
    codec_init_function_t               	Init; /*!< Initialization of Codec*/
    codec_decode_function_t	                Decode; /*!< Decoding of codec*/
    codec_get_pcm_samples_function_t      	GetPcmSamples; /*!< get pcm samples
                                                            for codec*/  
    codec_reset_function_t              	Reset; /*!< reset codec*/
    codec_file_seek_function_t		        FileSeek; /*!< seek at specific 
                                                       offset*/ 
    codec_get_io_frame_size_function_t      GetIOFrameSize; /*!< Get io frame size*/
} codec_interface_function_table_t;

/*
 *  External Definitions
 */
extern const codec_interface_function_table_t g_codec_function_table[STREAM_TYPE_COUNT];


/* MP3 */
extern int32_t MP3DecoderGetMemorySize();
extern int32_t MP3DecoderInit(int32_t *memory[], int32_t *callback_fn[], 
                                                            int32_t *user_data );
extern int32_t MP3DecoderDecode(int32_t *memory[], int32_t *sample_produced, 
                                                            int32_t *buf_out );
//extern int32_t MP3DecoderGetPcmSamples(int32_t *memory[], int32_t *sample_produced, int32_t max_sample,int32_t *buf_out );
//extern int32_t MP3DecoderReset(int32_t *memory[]);
extern int32_t MP3DecoderSeek(int32_t *memory[], int32_t  seek_offset );
extern int32_t MP3DecoderGetIOFrameSize(int32_t *in_size, int32_t *out_size);

/* WMA */
extern int32_t WMADecoderGetMemorySize();
extern int32_t WMADecoderInit(int32_t *memory[], int32_t *callback_fn[], 
                                                            int32_t *user_data );
extern int32_t WMADecoderDecode(int32_t *memory[], int32_t *sample_produced, 
                                                            int32_t *buf_out );
extern int32_t WMADecoderGetPcmSamples(int32_t *memory[], int32_t *sample_produced, 
                                                 int32_t max_sample,int32_t *buf_out );
//extern int32_t WMADecoderReset(int32_t *memory[]);
extern int32_t WMADecoderSeek(int32_t *memory[], int32_t  seek_offset );
extern int32_t WMADecoderGetIOFrameSize(int32_t *in_size, int32_t *out_size);


/* AAC */
extern int32_t AACDecoderGetMemorySize();
extern int32_t AACDecoderInit(int32_t *memory[], int32_t *callback_fn[], 
                                                            int32_t *user_data );
extern int32_t AACDecoderDecode(int32_t *memory[], int32_t *sample_produced, 
                                                                int32_t *buf_out );
//extern int32_t AACDecoderGetPcmSamples(int32_t *memory[], int32_t *sample_produced, int32_t max_sample,int32_t *buf_out );
extern int32_t AACDecoderReset(int32_t *memory[]);
extern int32_t AACDecoderSeek(int32_t *memory[], int32_t  seek_offset );
extern int32_t AACDecoderGetIOFrameSize(int32_t *in_size, int32_t *out_size);


/* OGG */
#ifdef CODEC_OGG
extern int32_t VORBISDecoderGetMemorySize();
extern int32_t VORBISDecoderInit(int32_t *memory[], int32_t *callback_fn[], 
                                                            int32_t *user_data );
extern int32_t VORBISDecoderDecode(int32_t *memory[], int32_t *sample_produced, 
                                                                int32_t *buf_out );
//extern int32_t OGGDecoderGetPcmSamples(int32_t *memory[], int32_t *sample_produced, int32_t max_sample,int32_t *buf_out );
extern int32_t VORBISDecoderReset(int32_t *memory[]);
extern int32_t VORBISDecoderSeek(int32_t *memory[], int32_t  seek_offset );
extern int32_t VORBISDecoderGetIOFrameSize(int32_t *in_size, int32_t *out_size);
#endif

/* FLAC */
extern int32_t FLACDecoderGetMemorySize();
extern int32_t FLACDecoderInit(int32_t *memory[], int32_t *callback_fn[], 
                                                            int32_t *user_data );
extern int32_t FLACDecoderDecode(int32_t *memory[], int32_t *sample_produced, 
                                                                int32_t *buf_out );
//extern int32_t FLACDecoderGetPcmSamples(int32_t *memory[], int32_t *sample_produced, int32_t max_sample,int32_t *buf_out );
extern int32_t FLACDecoderReset(int32_t *memory[]);
extern int32_t FLACDecoderSeek(int32_t *memory[], int32_t  seek_offset );
extern int32_t FLACDecoderGetIOFrameSize(int32_t *in_size, int32_t *out_size);

/* APE */
extern int32_t APEDecoderGetMemorySize();
extern int32_t APEDecoderInit(int32_t *memory[], int32_t *callback_fn[], 
                                                            int32_t *user_data );
extern int32_t APEDecoderDecode(int32_t *memory[], int32_t *sample_produced, 
                                                                int32_t *buf_out );
//extern int32_t APEDecoderGetPcmSamples(int32_t *memory[], int32_t *sample_produced, int32_t max_sample,int32_t *buf_out );
extern int32_t APEDecoderReset(int32_t *memory[]);
extern int32_t APEDecoderSeek(int32_t *memory[], int32_t  seek_offset );
extern int32_t APEDecoderGetIOFrameSize(int32_t *in_size, int32_t *out_size);

/* WAV */

extern int32_t WAVDecoderGetMemorySize();
extern int32_t WAVDecoderInit(int32_t *memory[], int32_t *callback_fn[], 
                                                            int32_t *user_data );
extern int32_t WAVDecoderDecode(int32_t *memory[], int32_t *sample_produced, 
                                                                int32_t *buf_out );
//extern int32_t WAVDecoderGetPcmSamples(int32_t *memory[], int32_t *sample_produced, int32_t max_sample,int32_t *buf_out );
extern int32_t WAVDecoderReset(int32_t *memory[]);
extern int32_t WAVDecoderSeek(int32_t *memory[], int32_t  seek_offset );
extern int32_t WAVDecoderGetIOFrameSize(int32_t *in_size, int32_t *out_size);


/* ALAC */
extern int32_t ALACDecoderGetMemorySize();
extern int32_t ALACDecoderInit(int32_t *memory[], int32_t *callback_fn[], 
                                                            int32_t *user_data );
extern int32_t ALACDecoderDecode(int32_t *memory[], int32_t *sample_produced, 
                                                                int32_t *buf_out );
extern int32_t ALACDecoderSeek(int32_t *memory[], int32_t  seek_offset );
extern int32_t ALACDecoderGetIOFrameSize(int32_t *in_size, int32_t *out_size);


/* OPUS */
extern int32_t OPUSDecoderGetMemorySize();
extern int32_t OPUSDecoderInit(int32_t *memory[], int32_t *callback_fn[], 
                                                            int32_t *user_data );
extern int32_t OPUSDecoderDecode(int32_t *memory[], int32_t *sample_produced, 
                                                                int32_t *buf_out );
extern int32_t OPUSDecoderSeek(int32_t *memory[], int32_t  seek_offset );
extern int32_t OPUSDecoderGetIOFrameSize(int32_t *in_size, int32_t *out_size);

#if defined(__cplusplus)
}
#endif

#endif /* __CODEC_INTERFACE_H__ */

/*
 * End of file
 * @}
 */

