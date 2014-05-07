/*! @addtogroup cci
 * @{
 *
 * Copyright (c) 2013-2014 Freescale, Inc.
 */
/*! @file metadata.h
 *  @brief Contains header data related to extracting metada for the @ref codec
 *                                                         interface subsystem.
 */
#ifndef _CCI_METADATA_H
#define _CCI_METADATA_H

/*
 * MetaData definitions
 */

#define I2S_DEMO  //add by b43107

#ifndef I2S_DEMO
#include "impresario_cfg.h"
#else
#include "types.h"
#endif
#include "codec_interface_public_api.h"

#if defined(__cplusplus)
extern "C" {
#endif

/*
 * Definitions
 */
#define MAX_PTP_STRING_CHARS              64
#define MAX_PTP_STRING_CHARS_GENRE        34
#define MAX_PTP_STRING_CHARS_YEAR        5


#define META_DATA_FOUND                   (WORD)(0)/*!< Metadata was found.*/
#define META_DATA_NOT_FOUND               (WORD)(0x100)/*!< Metadata was not 
                                                                        found.*/
#define META_DATA_FILE_NOT_SUPPORTED      (WORD)(0x101)/*!< File type is not
                                                                    supported.*/
#define META_DATA_FILEOPEN_ERROR          (WORD)(0x200)/*!< An error occurred
                                                       while opening the file.*/
#define META_DATA_FILEREAD_ERROR          (WORD)(0x201)/*!<  An error occurred 
                                                       while reading the file.*/
#define META_DATA_FILECLOSE_ERROR         (WORD)(0x202)/*!<  An error occurred
                                                       while closing the file.*/
#define META_DATA_FILESEEK_ERROR          (WORD)(0x203)/*!<  An error occurred
                                                       while seeking the file.*/
#define META_DATA_NOT_VBR_FORMAT          (WORD)(0x102)/*!< Data is not in VBR
                                                                       Format */
#define VBR_NOT_DETERMINED 0              /*!< VBR Not determined */
#define VBR_TRUE 1                        /*!< VBR True Value */
#define XING_TOC_SIZE   100               /*!< Xing Size */
#define VBR_TOC_SIZE    200
#define VBR_TOC_SIZE_IN_BYTES    200*4

#ifndef I2S_DEMO

#ifndef TITLE_SIZE 
#define TITLE_SIZE FASCFG_TRACK_TITLE_LEN
#define TITLE_SIZE_BYTES (TITLE_SIZE * 2)
#endif 

#ifndef ARTIST_SIZE 
#define ARTIST_SIZE FASCFG_TRACK_ARTIST_LEN
#define ARTIST_SIZE_BYTES (ARTIST_SIZE * 2)
#endif 

#ifndef ALBUM_SIZE
#define ALBUM_SIZE FASCFG_TRACK_ALBUM_LEN
#define ALBUM_SIZE_BYTES (ALBUM_SIZE * 2)
#endif

#ifndef GENRE_SIZE
#define GENRE_SIZE FASCFG_TRACK_GENRE_LEN
#define GENRE_SIZE_BYTES (GENRE_SIZE * 2)
#endif

#ifndef YEAR_SIZE
#define YEAR_SIZE FASCFG_TRACK_YEAR_LEN  /* YYYY + NULL terminator */
#define YEAR_SIZE_BYTES (YEAR_SIZE * 2)
#endif

#else

#ifndef TITLE_SIZE 
#define TITLE_SIZE MAX_PTP_STRING_CHARS
#define TITLE_SIZE_BYTES (TITLE_SIZE * 2)
#endif 

#ifndef ARTIST_SIZE 
#define ARTIST_SIZE MAX_PTP_STRING_CHARS
#define ARTIST_SIZE_BYTES (ARTIST_SIZE * 2)
#endif 

#ifndef ALBUM_SIZE
#define ALBUM_SIZE MAX_PTP_STRING_CHARS
#define ALBUM_SIZE_BYTES (ALBUM_SIZE * 2)
#endif

#ifndef GENRE_SIZE
#define GENRE_SIZE MAX_PTP_STRING_CHARS
#define GENRE_SIZE_BYTES (GENRE_SIZE * 2)
#endif

#ifndef YEAR_SIZE
#define YEAR_SIZE MAX_PTP_STRING_CHARS_YEAR  /* YYYY + NULL terminator */
#define YEAR_SIZE_BYTES (YEAR_SIZE * 2)
#endif

#endif /* I2S_Demo */


#ifndef TRACKNUM_SIZE
#define TRACKNUM_SIZE 5 /* assume track number is smaller than 
                                                    0xfff (12bit or 4095)*/
#endif

#ifndef DATE_SIZE
#define DATE_SIZE MAX_PTP_SHORT_STRING_CHARS
#define DATE_SIZE_BYTES (DATE_SIZE * 2)
#endif

#ifndef ENCODINGPROFILE_SIZE
#define ENCODINGPROFILE_SIZE MAX_PTP_SHORT_STRING_CHARS
#define ENCODINGPROFILE_SIZE_BYTES (ENCODINGPROFILE_SIZE * 2)
#endif


/*
 * Structure
 */

/*!
 * @brief Enum Codec bitrate type
 */
typedef enum _codec_bitrate_type
{
     CONSTANT_BITRATE = 0,    /*!< 0 = Constant BitRate*/ 
     VARIABLE_BITRATE,        /*!< 1 = Variable BitRate*/
     BITRATE_TYPE_UNKNOWN,     /*!< 2 = Unknown BitRate Type*/
} codec_bitrate_type_t;

/*!
 * @brief CCI Context Structure
 */
typedef struct {
    void *user_data;    /*!< Holds file pointer (in case of direct file interface)
                         or CCIDecInfo context (in case of streamer interface)*/
    /*!< CCI interface callback API for read operation*/
    int32_t (*cci_dec_read) (int32_t, uint8_t*, int32_t, int32_t, void*); 
    /*!< CCI interface callback API for seek operation*/
    int32_t (*cci_dec_seek) (int32_t, int32_t, int32_t, void*);             
    /*!< CCI interface callback API for file position operation*/
    int32_t (*cci_dec_tell) (int32_t, void*);                               
} CCI_Ctx;

/*!
 * @brief File Metadata Structure
 */
typedef struct {
    audio_stream_type_t stream_type;/*!< Mp3, Wma, etc.*/
    uint16_t song_title[TITLE_SIZE];    /*!< Title of Song */
    uint16_t song_artist[ARTIST_SIZE];  /*!< Artist of Song */
    uint16_t song_album[ALBUM_SIZE];    /*!< Album of Song */
    uint16_t song_genre[GENRE_SIZE];    /*!< ptr to buffer to store genre string */
    uint16_t song_year[YEAR_SIZE];      /*!< ptr to buffer to store album year, in
                                            UTF16 as yyyy (ex. 1969) */
    uint8_t genre;                /*!< Genre of audio file -- if MP3, use
                                            the ID3v1 tag byte */
    uint32_t track_number;        /*!< Track number of album/playlist */
    uint32_t sample_rate;         /*!< Sample Rate of song (Hz) */
    int32_t num_channels;         /*!< Number of channels */
    uint32_t bit_rate;            /*!< Bitrate of song (for MP3 VBR, this
                                             will be first bitrate found) */
    uint32_t avg_bit_rate;         /*!< Avg bitrate in VBR song (if 0, 
                                        then it was not known) */
    uint32_t total_time;          /*!< Length of song in seconds */
    uint32_t start_pos;           /*!< Position where audio starts.
                                            (if required by the decoder) */
    uint32_t num_bytes;           /*!< Total Bytes in audio part of song */
    codec_bitrate_type_t bitrate_type;  /*!< Bitrate Type */
    bool valid_vbr_header;           /*!< Is the VBR header valid ? */
    int32_t file_handle;         /*!< Fast open key */
    int32_t bits_per_sample;       /*!< AIFF decoder(??) */
    uint32_t audio_sub_type;       /*!< Audio Subtype */
    bool has_janus_drm;               /*!< indicates whether wma song is 
                                                DRM or not */
    int32_t mpeg_layer;           /*!< Mpeg Layer */
    bool no_audio;                  /*!< No Audio */
} file_meta_data_t;

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
                            file_meta_data_t *meta_data, CCI_Ctx *ctx);

/*! @brief CCI extract metadata top level function for streamer interface.
 * 
 * @fntype Function
 *
 * @param[in] streamType                Audio Stream Type.
 * @param[in] *meta_data                 meta_data extracted from the file.
 * @param[in] *ctx           	        Pointer to CCI Context structure.
 * 
 * @retval CODEC_SUCCESS                if meta_data found successfully. 
 * @retval CODEC_METADATA_FILETYPE_FAILED if meta_data not found
 */

int32_t ccidec_extract_meta_data(audio_stream_type_t streamType, 
                            file_meta_data_t *meta_data, CCI_Ctx *ctx);
                            


extern int32_t codec_extract_metadata_mp3 (file_meta_data_t *meta_data,
                                        CCI_Ctx *Ctx);

extern int32_t codec_extract_metadata_wave(file_meta_data_t *meta_data,
                                        CCI_Ctx *Ctx);

extern int32_t codec_extract_metadata_aac (file_meta_data_t *meta_data,
                                        CCI_Ctx *Ctx);

extern int32_t codec_extract_metadata_flac(file_meta_data_t *meta_data,
                                        CCI_Ctx *Ctx);
#if defined(__cplusplus)
}
#endif

#endif /*  #ifndef _METADATA_H */
/*                                                                                
 * End of file
 * @}
 */
