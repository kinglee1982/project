#ifndef OPUS_ENCODE_H
#define OPUS_ENCODE_H

#include "opus.h"
#include "media_info.h"


typedef struct OpusEncoder_t
{
	OpusEncoder  *m_pEncoder;
	unsigned char m_pcmBuffer[20 * 1024];
	unsigned int  m_pcmLength;
	unsigned int  m_sampleInput;
	unsigned int  m_nSampleRate;
}OpusEncoderPara;


typedef  void*  OpusEncoderHandle;

#ifdef __cplusplus
extern "C"
{
#endif

OpusEncoderHandle openEncoder(int sampleRate,
							  int channel,
							  int bitrates,
							  int fps);

void closeEncoder(OpusEncoderHandle handle);

int encodeOpus(OpusEncoderHandle handle,
				unsigned char *pPcm,
				int pcmLength,
				unsigned char *pOpus) ;

#ifdef __cplusplus
}
#endif


#endif





