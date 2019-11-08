#include <string.h>
#include <stdlib.h>
#include "opus_decode.h"
#include "opus.h"

OpusDecoderHandle openDecoder(int sampleRate,int channel)
{
	OpusDecoderPara *pDecoder =
		(OpusDecoderPara *)malloc(sizeof(OpusDecoderPara));
	if (pDecoder == NULL)
	{
		return NULL;
	}

	memset(pDecoder,0,sizeof(OpusDecoderPara));

	int res = 0;
	/* Init opus decoder */
	pDecoder->m_pDecoder = opus_decoder_create(sampleRate, channel, &res);
	if (res != OPUS_OK || pDecoder->m_pDecoder == NULL)
	{
		free(pDecoder);
		return NULL;
	}

	return (OpusDecoderHandle)pDecoder;

}


void closeDecoder(OpusDecoderHandle handle)
{
	OpusDecoderPara *pDecoder = (OpusDecoderPara *)handle;
	if (pDecoder == NULL)
	{
		return;
	}

	if (pDecoder->m_pDecoder != NULL)
	{
		opus_decoder_destroy(pDecoder->m_pDecoder);
		pDecoder->m_pDecoder = NULL;
	}

	free(pDecoder);
}



int decodeOpus(OpusDecoderHandle handle,
			   unsigned char *pOpus,
			   int opusLength,
			   unsigned char *pPcm)
{
	OpusDecoderPara *pDecoder = (OpusDecoderPara *)handle;
	if (pDecoder == NULL)
	{
		return -1;
	}

	int res = opus_decode(pDecoder->m_pDecoder,
		pOpus,
		opusLength,
		(opus_int16 *)pPcm,
		100 * 1024,
		0);
	if (res <= 0)
	{
		return -2;
	}

	return res * 2;
}










