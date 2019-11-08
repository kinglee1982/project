#ifndef OPUS_DECODE_H
#define OPUS_DECODE_H

#include "opus.h"
#include "media_info.h"

typedef struct OpusDecoder_t
{
	OpusDecoder  *m_pDecoder;
}OpusDecoderPara;

typedef  void*  OpusDecoderHandle;

#ifdef __cplusplus
extern "C"
{
#endif

OpusDecoderHandle openDecoder(int sampleRate,int channel);

void closeDecoder(OpusDecoderHandle handle);

int decodeOpus(OpusDecoderHandle handle,
				unsigned char *pOpus,
				int opusLength,
				unsigned char *pPcm) ;

#ifdef __cplusplus
}
#endif


#endif





