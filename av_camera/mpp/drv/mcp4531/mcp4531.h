#ifndef __MCP4531_H__
#define __MCP4531_H__
#define MCP4531ADDR_MIC	0x5C
#define MCP4531ADDR_AUDIO	0x5E
typedef unsigned char uint8;
typedef unsigned short uint16;

#define IOC_MCP4531_SET_MIC_VOLUME  0x01
#define IOC_MCP4531_SET_AUDIO_VOLUME  0x03
#define IOC_MCP4531_GET_MIC_VOLUME  0x05
#define IOC_MCP4531_GET_AUDIO_VOLUME  0x07

#endif
