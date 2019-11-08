
#include <string.h>
#include <stdlib.h>
#include "opus_encode.h"
#include "opus.h"
#include "stdio.h"

#define PER_SAMPLE_BYTE 2





OpusEncoderHandle openEncoder(int sampleRate,
							  int channel,
							  int bitrates,
							  int fps)
{
	OpusEncoderPara *pEncdoer =
		(OpusEncoderPara *)malloc(sizeof(OpusEncoderPara));
	if (pEncdoer == NULL)
	{
		return NULL;
	}

	memset(pEncdoer,0,sizeof(OpusEncoderPara));

	int res = 0;
	/* Init opus decoder */
	pEncdoer->m_pEncoder = opus_encoder_create(sampleRate,
		channel,
		OPUS_APPLICATION_VOIP,
		&res);
	if (res != OPUS_OK || pEncdoer->m_pEncoder == NULL)
	{

		free(pEncdoer);
		return NULL;
	}

	pEncdoer->m_sampleInput = sampleRate/fps;
	pEncdoer->m_nSampleRate = sampleRate;
	opus_encoder_ctl(pEncdoer->m_pEncoder, OPUS_SET_BITRATE(bitrates));

	return (OpusEncoderHandle)pEncdoer;

}


void closeEncoder(OpusEncoderHandle handle)
{
	OpusEncoderPara *pEncdoer = (OpusEncoderPara *)handle;
	if (pEncdoer == NULL)
	{
		return;
	}

	if (pEncdoer->m_pEncoder != NULL)
	{
		opus_encoder_destroy(pEncdoer->m_pEncoder);
		pEncdoer->m_pEncoder = NULL;
	}

	free(pEncdoer);
}



int encodeOpus(OpusEncoderHandle handle,
			   unsigned char *pPcm,
			   int pcmLength,
			   unsigned char *pOpus)
{
	OpusEncoderPara *pEncdoer = (OpusEncoderPara *)handle;
	if (pEncdoer == NULL)
	{
		return -1;
	}

	if (pEncdoer->m_pcmLength + pcmLength > sizeof(pEncdoer->m_pcmBuffer))
	{
		return -1;
	}

	///新来的数据追加到pcm后面
	if (pcmLength > 0)
	{
		memcpy(pEncdoer->m_pcmBuffer + pEncdoer->m_pcmLength,
			pPcm,
			pcmLength);
		pEncdoer->m_pcmLength += pcmLength;
	}

	///不够一次编码
	if (pEncdoer->m_pcmLength  < pEncdoer->m_sampleInput * PER_SAMPLE_BYTE)
	{
       // printf("1111111111111111111111111111\n");
		return 0;
	}

	int res = opus_encode(pEncdoer->m_pEncoder,
		(const opus_int16 *)pEncdoer->m_pcmBuffer,
		pEncdoer->m_sampleInput,
		pOpus,
		4000);
	if(res <= 1)
	{
	     //   printf("2222222222222222222222\n");

		return 0;
	}

	int encode_frame = opus_packet_get_samples_per_frame(pOpus, pEncdoer->m_nSampleRate)* opus_packet_get_nb_frames(pOpus, res);
	if (encode_frame < 0 || encode_frame > (int)pEncdoer->m_sampleInput)
	{
		pEncdoer->m_pcmLength = 0;
        //printf("333333333333333333333333\n");
		return 0;
	}

	int encodePcmLength = encode_frame * PER_SAMPLE_BYTE;
	if(encodePcmLength < (int)pEncdoer->m_pcmLength)
	{
		memmove(pEncdoer->m_pcmBuffer,
			pEncdoer->m_pcmBuffer + encodePcmLength,
			pEncdoer->m_pcmLength - encodePcmLength);

		pEncdoer->m_pcmLength -= encodePcmLength;
	}
	else
	{
		pEncdoer->m_pcmLength = 0;
	}
       // printf("44444444444441111111111111111111111\n");
	return res;
}



#if 1

void opusTestEncoder(unsigned char* pPcmFrame,
				     unsigned int   pcmFrameLength,
					 unsigned char *pOpus,
					 unsigned int  *pOpusLength)
{
	*pOpusLength = 0;

	do
	{
		int ret = encodeOpus(NULL,
			pPcmFrame,
			pcmFrameLength,
			pOpus + *pOpusLength);
		if (ret <= 0)
		{
			break;
		}

		*pOpusLength += ret;


		pcmFrameLength = 0;
	} while (1);


	return ;

}


#endif








