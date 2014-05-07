 
#include<mqx.h>
#include"adapter.h"
#include"dai_link.h"
#include "fio.h"  




/*
 * default set is the sample rate 44100, 16 bits, 2 channel
 * umute
 */
int32_t msi_snd_init(void)
{
	printf("msi_snd_init\n");
	return dai_link_init(44100, 16, 2);
}

/*
 * write samples to device
 * must have correct set before write
 */
int32_t msi_snd_write(uint8_t * pcmStream, uint32_t pcmCnt)
{
	int32_t res = 0;
#ifdef BY_PASS_ADAPTER
	  res =  dai_link_write((uint16_t *)pcmStream, pcmCnt);
	  if(res == 0)
		  printf("%s  data NULL\n",__func__);
	return res;
#else
	audio_write((uint16_t *)pcmStream, pcmCnt);
#endif
}

/*
 * collect resources of sound device
 */
int32_t msi_snd_deinit(void)
{
	dai_link_deinit();
	return 1;
}

/*
 * set pcm sound format: sample rate, bit width, channel number
 */
int32_t msi_snd_set_format(uint32_t sampleRate, uint8_t bitWidth, uint8_t chNum)
{
	dai_link_set_format(sampleRate, bitWidth, chNum);
	return 0;
}

/*
 * turn up sound volume
 */
int32_t msi_snd_vol_up(void)
{
	return dai_link_vol_up();
}

/*
 * turn down sound volume
 */
int32_t msi_snd_vol_down(void)
{
	return dai_link_vol_down();
}

/*
 * mute sound device
 */
int32_t msi_snd_mute(void)
{
	dai_link_mute();
	return 0;
}
/*
 * umute device
 */
int32_t msi_snd_umute(void)
{
	dai_link_umute();
	return 0;
}

/*
 * flush the pcm stream out
 * 
 */
int32_t msi_snd_flush(void)
{
	dai_link_flush();
	return 0;
}
/*
 * get the buffer occupancy rate
 * return 0~100 represent 0%~ 100% 
 * 
 */
int16_t msi_snd_get_occ(void)
{
	return dai_link_get_occ();
}
