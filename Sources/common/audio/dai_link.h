#ifndef _DAI_LINK_
#define _DAI_LINK

#define AUIDO_PHY_SAI_CODEC
//#define AUDIO_PHY_DAC

#ifdef  AUIDO_PHY_SAI_CODEC
#include"../i2s/sai_codec.h"

#define dai_link_init  sai_codec_init
#define dai_link_deinit sai_codec_deinit
#define dai_link_write  sai_codec_write
#define dai_link_flush  sai_codec_flush
#define dai_link_get_occ sai_codec_get_occ
#define dai_link_ioctl  sai_codec_ioctl
#define dai_link_set_format sai_codec_set_format
#define dai_link_vol_up sai_codec_vol_up
#define dai_link_vol_down sai_codec_vol_down
#define dai_link_mute  sai_codec_mute
#define dai_link_umute sai_codec_umute


#else 
#ifdef AUDIO_PHY_DAC
#include"../dac/dac.h"


#define dai_link_init  dac_init
#define dai_link_deinit dac_deinit
#define dai_link_write  dac_write
#define dai_link_flush  dac_flush
#define dai_link_ioctl  dac_ioctl
#define dai_link_set_format dac_set_format
#define dai_link_vol_up dac_vol_up
#define dai_link_vol_down dac_vol_down
#define dai_link_mute  dac_mute
#define dai_link_umute dac_umute
#endif
#endif

#endif
