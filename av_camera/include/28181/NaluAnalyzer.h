#ifndef NALU_ANALYZER_H
#define NALU_ANALYZER_H

#include <string.h>
////////////////////////////////////////////////////////////////////////////

typedef unsigned char uint8_t;

class ImsIOBuffer;

struct NaluPacket
{
    enum NaluType
    {
        NALU_NULL = 0,
        NALU_SPS = 7,
        NALU_PPS = 8,
        NALU_SEI = 6,
		NALU_AUD = 9,
        NALU_IFRAME = 5
    };

    uint8_t*    data;
    int length;
    uint8_t type;
    int prefix;
	long long nTimestamp;
};


class NaluAnalyzerSink
{
public:
    virtual ~NaluAnalyzerSink() {}

    virtual void naluAnalyPacket(NaluPacket& packet) =0;

};



class NaluAnalyzer
{
public:
    NaluAnalyzer();
    ~NaluAnalyzer();

    bool inputData(const uint8_t* buffer, size_t length,long long nTimestamp);

    void setSink(NaluAnalyzerSink* pSink,bool analyzerAll = false);

    void clear();

protected:   
    void writePacket(NaluPacket& packet);
    
    bool findNalu(uint8_t* buffer, size_t length, size_t start, NaluPacket& packet);

private:
    NaluAnalyzerSink*   m_pSink;
    ImsIOBuffer     *m_pBuffer;  
	bool m_analyzerAll;

};

////////////////////////////////////////////////////////////////////////////
#endif //NALUANALYZER_TERRY_
