exlude adk20.c from code warrior project

update:
on k60
1.adjust pdb from 1088 and 1087 all the way(stability)
2.add aligned buffer to buffer the dma buffer
3.stdand pdb value 48000000/44100 = 1088.43537
on k70
1.adjust pdb from 1460 and 1360all the way(stability)
2.add aligned buffer to buffer the dma buffer
3.stdand pdb value 60000000/44100 = 1360
update:


extract audio output from usb audio stack
provide audio write api:

uint8_t audio_init(uint32_t pcmSampleRate);
/*
 * \brief Reset DAC and PDB
 * \return		0
 */
uint8_t audio_deinit(void);
/*
 * \brief temporary used for start DAC and PDB
 * \return		0
 */
uint8_t audio_init_tmp(void);
/*
 * \brief temporary used for clear audio buffer record
 * \return		0
 */
uint8_t audio_clear_buffer(void);
/*
 * \brief Write pcm data to audio output
 * \param[in]	buffer	pcm data buffer
 * \param[in]	bufLen  data length
 * \return		0
 */
uint8_t audio_write(uint8_t * buffer, uint32_t bufLen);

the data types declaretions conform to cstdint

update:

add distinguish of k60 and k70 in this way in dac.c etc.

#ifdef	_KINETIS_K70
	  SIM_SCGC6 |= SIM_SCGC6_DMAMUX0_MASK; 	 // Enable clocks to the DMA Mux.
#else
#ifdef _KINETIS_K60 
    //enable minor off mode
	  SIM_SCGC6 |= SIM_SCGC6_DMAMUX_MASK; 	 // Enable clocks to the DMA Mux.
#else
#error "CAN NOT RECOGNIZED CHIP"
#endif
#endif
update:
cancel the last distinguish method and use the macro MQX_CPU like this

#if (MQX_CPU == PSP_CPU_MK70F120M)
	 SIM_SCGC6 &= ~SIM_SCGC6_DMAMUX0_MASK;  // Disable clocks to the DMA Mux.
#else
#if MQX_CPU == PSP_CPU_MK60D100M 
	 SIM_SCGC6 &= ~SIM_SCGC6_DMAMUX_MASK;  // Disable clocks to the DMA Mux.
#else
#error "can not recognize chip"
#endif
#endif

update:
seperate source code files to directories: audio, usb accessory, debug test
and a header directory including audio.h, android.h

update:
this is the single module source code of audio version 4.1 api is the same with ADKApp60v3.0 and ADKApp70v3.0

update:

solved the MQX_CPU == PSP_CPU_MK70F120M error determination some time when dac.h included before the mqx.h to be included. The effect is the the first switch is satisfied.


update:
re-designed the dac module with the goal of loose coupling
add dac_start and stop into the dac module.
	stop dac includes stop pdb and start dma erq

