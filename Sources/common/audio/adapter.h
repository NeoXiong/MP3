/*
 * audio_output.h
 *
 *  Created on: Jul 17, 2013
 *      Author: B46407
 */

#ifndef AUDIO_OUTPUT_H_
#define AUDIO_OUTPUT_H_

#include<cstdint>
#define BY_PASS_ADAPTER


/*
 * \brief Write pcm data to audio output
 * \param[in]	buffer	pcm data buffer
 * \param[in]	bufLen  data length,unit 32-bit
 * \return		0
 */
int8_t audio_write(uint16_t * buffer, uint32_t bufLen);


#endif /* AUDIO_OUTPUT_H_ */
