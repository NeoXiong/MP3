#ifndef _CODEC_
#define _CODEC

#define SGTL5000
#ifdef SGTL5000
#include"sgtl5000.h"
#define codec_vol_up  sgtl5000_VolUp
#define codec_vol_down sgtl5000_VolDown
#define codec_mute sgtl5000_mute
#define codec_umute sgtl5000_umute
#define codec_slave_init  sgtl5000_slave_init
#define codec_slave_deinit sgtl5000_slave_deinit
#define code_set_fs	sgtl5000_set_fs
#define codec_slave_set_bitwidth  sgtl5000_slave_set_bitwidth
#define codec_power_on	sgtl5000_power_on
#else
#ifdef WM8962
#include"wm8962.h"
#define codec_vol_up  wm8962_VolUp
#define codec_vol_down wm8962_VolDown
#define codec_init  wm8962_init
#define code_set_fs	wm8962_set_fs
#define codec_set_bitwidth  wm8962_set_bitwidth
#endif
#endif
#endif
