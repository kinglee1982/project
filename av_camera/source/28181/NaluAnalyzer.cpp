#include "NaluAnalyzer.h"
#include <assert.h>
#include "TIOBuffer.h"

NaluAnalyzer::NaluAnalyzer():
    m_pSink()
{
	m_pBuffer = new ImsIOBuffer();
    m_pBuffer->ensure(1024 * 500);
}

NaluAnalyzer::~NaluAnalyzer()
{
	if (m_pBuffer != NULL)
	{
		delete m_pBuffer;
		m_pBuffer = NULL;
	}

}

bool NaluAnalyzer::inputData(const uint8_t* buffer, size_t length,long long nTimestamp)
{
    if (length == 0)
    {
        return false;
    }

    m_pBuffer->write(buffer, length);
    
    bool found = false;
    while (true)
    {
        uint8_t* data = m_pBuffer->getReadPtr();
        size_t size = m_pBuffer->readable();

        NaluPacket firstPacket;
        if (!findNalu(data, size, 0, firstPacket))
        {
			m_pBuffer->skipAll();
            break;
        }

        NaluPacket secondPacket;
        if (!findNalu(data, size, firstPacket.length + firstPacket.prefix, secondPacket))
        {
			if (m_analyzerAll)
			{
				firstPacket.data = data;
				firstPacket.length = size;
				firstPacket.nTimestamp = nTimestamp;
				writePacket(firstPacket);
				m_pBuffer->skipAll();
				found = true;
			}
            break;
        }

        firstPacket.length = secondPacket.length - firstPacket.length;
		firstPacket.nTimestamp = nTimestamp;
        writePacket(firstPacket);
        m_pBuffer->skip(secondPacket.length);
		found = true;
    }

    return found;
}

void NaluAnalyzer::setSink(NaluAnalyzerSink* pSink,bool analyzerAll)
{
    m_pSink = pSink;
	m_analyzerAll = analyzerAll;
}

void NaluAnalyzer::clear()
{
     m_pBuffer->clear();
}


void NaluAnalyzer::writePacket(NaluPacket& packet)
{
    if (!m_pSink)
    {
        return ;
    }
    m_pSink->naluAnalyPacket(packet);
}



bool NaluAnalyzer::findNalu(uint8_t* buffer, size_t length, size_t start, NaluPacket& packet)
{
    if ((length < 3) || ((length - start) < 3))
    {
        return false;
    }

    bool found = false;
    uint8_t* p = buffer;
    for (size_t i = start; i < (length - 3); ++ i)
    {
        if ((p[i] == 0) && (p[i+1] == 0))
        {
            if (p[i+2] == 0)
            {
                if (((i + 3) < length) && (p[i+3] == 1))
                {
                    packet.data = p + i;
                    packet.length = i;
                    packet.prefix = 4;
                    packet.type = packet.data[4] & 0x1F;

                    found = true;
                    break;
                }
            }
            else if (p[i+2] == 1)
            {
                packet.data = p + i;
                packet.length = i;
                packet.prefix = 3;
                packet.type = packet.data[3] & 0x1F;

                found = true;
                break;
            }
        }
    }
    return found;
}

