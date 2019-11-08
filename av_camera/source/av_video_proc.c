#include "av_public_head.h"
#include "av_capture.h"
#include "av_common_31.h"
#include "av_video_proc.h"
#include "av_video_init.h"
#include "av_vivo.h"
#include "av_store.h"
#include "av_split.h"
#include "av_hddfsm.h"
#include "av_socket.h"
#include "av_audio_init.h"

HI_U8 g_iVideoFrameRate = 25;    /* 帧率 */
extern HI_S32 g_Mp3FileStat;
HI_S32 decchn = 4;
HI_S32 g_s32VBSource = 0;
VB_POOL g_ahVbPool[VB_MAX_POOLS] = {0};//{[0 ... (VB_MAX_POOLS-1)] = VB_INVALID_POOLID};
//HI_U8 Cam_En_Flag=0xff;/*从低位到高位，每个位代表一个通道，0代表关闭，1代表使能 */
extern HI_CHAR *g_acpBmpBuffer[APP_VI_MAXCHNCNT];
HI_U16 Cam_En_Flag = 0xffff;
HI_U16 Cam_Snap_Chn = 0;
FB_MARK_T gFbMark;
APP_NET_PLAYFILE_REQ_ST g_stNetPlayRecodeDump;
SF_LIST_ST g_struNetSearchFileList;//存储远程搜索出来的文件列表
REC_PLAYBACK_T g_stRecLPB;
HI_S32 g_s32PBChnStat = 0;           /* 本地回放文件状态 */
HI_S32 g_iPlaybackFileCnt[APP_VDEC_CHNCNT]= {0};
HI_CHAR *g_pcRecodeFile = NULL;
int g_Videcfd = -1;
int g_Videcsd = -1;
SF_LIST_ST g_stLocalSFList[12];//存储远程搜索出来的文件列表
SF_LIST_ST g_stAlarmRecBkSFList[12]; /*报警备份时候搜索存放 */
SF_PB_CTRL g_stPbctrl[MAX_PLAY_BACK_CHN];
HI_CHAR *g_acpBmpBuffer[APP_VI_MAXCHNCNT] =
{
    (HI_CHAR *)"C H L  1",
    (HI_CHAR *)"C H L  2",
    (HI_CHAR *)"C H L  3",
    (HI_CHAR *)"C H L  4 ",
    (HI_CHAR *)"C H L  5",
    (HI_CHAR *)"C H L  6",
    (HI_CHAR *)"C H L  7",
    (HI_CHAR *)"C H L  8",
    (HI_CHAR *)"C H L  9",
    (HI_CHAR *)"C H L  1 0",
    (HI_CHAR *)"C H L  1 1",
    (HI_CHAR *)"C H L  1 2",
};

const HI_U8 g_aucASCIIMap[VPP_ASCII_MAP_SIZE] =
{
    '0',
    '1',
    '2',
    '3',
    '4',
    '5',
    '6',
    '7',
    '8',
    '9',
    'A',
    'B',
    'C',
    'D',
    'E',
    'F',
    'G',
    'H',
    'I',
    'J',
    'K',
    'L',
    'M',
    'N',
    'O',
    'P',
    'Q',
    'R',
    'S',
    'T',
    'U',
    'V',
    'W',
    'X',
    'Y',
    'Z',
    ' ',
    '-',
    ':',
    '.',
    '\'',
    ' ',
    ' ',
    ' ',
    ' ',
    ' '
};

/* 字体加粗的字符表 */
/* 点阵表 */
const HI_U8 g_aucASCIIPointMap0[VPP_ASCII_MAP_SIZE][VPP_CHR_REAL_HEIGRH] =
{
    {0x00,0x00,0x00,0x1C,0x36,0x63,0x63,0x63,0x63,0x63,0x63,0x63,0x36,0x1C,0x00,0x00}, /* 0 */
    {0x00,0x00,0x00,0x18,0x78,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x7E,0x00,0x00}, /* 1 */
    {0x00,0x00,0x00,0x3E,0x63,0x63,0x63,0x06,0x06,0x0C,0x18,0x30,0x63,0x7F,0x00,0x00}, /* 2 */
    {0x00,0x00,0x00,0x3E,0x63,0x63,0x06,0x1C,0x06,0x03,0x03,0x63,0x66,0x3C,0x00,0x00}, /* 3 */
    {0x00,0x00,0x00,0x06,0x0E,0x1E,0x36,0x36,0x66,0x66,0x7F,0x06,0x06,0x1F,0x00,0x00}, /* 4 */
    {0x00,0x00,0x00,0x7F,0x60,0x60,0x60,0x7C,0x76,0x03,0x03,0x63,0x66,0x3C,0x00,0x00}, /* 5 */
    {0x00,0x00,0x00,0x1E,0x36,0x60,0x60,0x7C,0x76,0x63,0x63,0x63,0x36,0x1C,0x00,0x00}, /* 6 */
    {0x00,0x00,0x00,0x7F,0x66,0x66,0x0C,0x0C,0x18,0x18,0x18,0x18,0x18,0x18,0x00,0x00}, /* 7 */
    {0x00,0x00,0x00,0x3E,0x63,0x63,0x63,0x36,0x1C,0x36,0x63,0x63,0x63,0x3E,0x00,0x00}, /* 8 */
    {0x00,0x00,0x00,0x1C,0x36,0x63,0x63,0x63,0x37,0x1F,0x03,0x03,0x36,0x3C,0x00,0x00}, /* 9 */

    {0x00,0x00,0x00,0x18,0x18,0x1C,0x3C,0x3C,0x36,0x3E,0x66,0x63,0x63,0xF7,0x00,0x00}, /* A */
    {0x00,0x00,0x00,0xFC,0x66,0x66,0x66,0x7C,0x66,0x63,0x63,0x63,0x66,0xFC,0x00,0x00}, /* B */
    {0x00,0x00,0x00,0x3F,0x63,0x63,0xC0,0xC0,0xC0,0xC0,0xC0,0x63,0x66,0x3C,0x00,0x00}, /* C */
    {0x00,0x00,0x00,0xFC,0x66,0x63,0x63,0x63,0x63,0x63,0x63,0x63,0x66,0xFC,0x00,0x00}, /* D */
    {0x00,0x00,0x00,0xFE,0x63,0x6C,0x6C,0x7C,0x6C,0x6C,0x60,0x63,0x63,0xFE,0x00,0x00}, /* E */
    {0x00,0x00,0x00,0xFE,0x63,0x6C,0x6C,0x7C,0x6C,0x6C,0x60,0x60,0x60,0xF0,0x00,0x00}, /* F */
    {0x00,0x00,0x00,0x3E,0x66,0x66,0xC0,0xC0,0xC0,0xCF,0xC6,0x66,0x66,0x3C,0x00,0x00}, /* G */
    {0x00,0x00,0x00,0xF7,0x63,0x63,0x63,0x63,0x7F,0x63,0x63,0x63,0x63,0xF7,0x00,0x00}, /* H */
    {0x00,0x00,0x00,0x7E,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x7E,0x00,0x00}, /* I */
    {0x00,0x00,0x00,0x3F,0x0C,0x0C,0x0C,0x0C,0x0C,0x0C,0x0C,0x0C,0x0C,0x0C,0xCC,0xF8}, /* J */
    {0x00,0x00,0x00,0xFF,0x66,0x6C,0x78,0x78,0x78,0x6C,0x6C,0x66,0x66,0xFF,0x00,0x00}, /* K */
    {0x00,0x00,0x00,0xF0,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x63,0xFF,0x00,0x00}, /* L */
    {0x00,0x00,0x00,0xFF,0x7E,0x7E,0x7E,0x7E,0x7E,0x7E,0x7E,0x7E,0x7E,0xFF,0x00,0x00}, /* M */
    {0x00,0x00,0x00,0xE7,0x73,0x73,0x7B,0x7B,0x6F,0x6F,0x6F,0x67,0x67,0xF3,0x00,0x00}, /* N */
    {0x00,0x00,0x00,0x3C,0x66,0xC3,0xC3,0xC3,0xC3,0xC3,0xC3,0xC3,0x66,0x3C,0x00,0x00}, /* O */
    {0x00,0x00,0x00,0xFE,0x63,0x63,0x63,0x63,0x7E,0x60,0x60,0x60,0x60,0xF0,0x00,0x00}, /* P */
    {0x00,0x00,0x00,0x3C,0x66,0xC3,0xC3,0xC3,0xC3,0xC3,0xFB,0xEF,0x6E,0x3C,0x07,0x00}, /* Q */
    {0x00,0x00,0x00,0xFE,0x63,0x63,0x63,0x7E,0x6C,0x6C,0x66,0x66,0x63,0xF3,0x00,0x00}, /* R */
    {0x00,0x00,0x00,0x3F,0x63,0x63,0x60,0x30,0x1C,0x06,0x03,0x63,0x63,0x7E,0x00,0x00}, /* S */
    {0x00,0x00,0x00,0xFF,0xDB,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x3C,0x00,0x00}, /* T */
    {0x00,0x00,0x00,0xF7,0x63,0x63,0x63,0x63,0x63,0x63,0x63,0x63,0x63,0x3E,0x00,0x00}, /* U */
    {0x00,0x00,0x00,0xF7,0x63,0x63,0x66,0x36,0x36,0x3C,0x3C,0x1C,0x18,0x18,0x00,0x00}, /* V */
    {0x00,0x00,0x00,0xFF,0xDB,0xDB,0xDB,0xDB,0xFF,0xFF,0x7E,0x66,0x66,0x66,0x00,0x00}, /* W */
    {0x00,0x00,0x00,0xF7,0x63,0x36,0x36,0x1C,0x1C,0x1C,0x36,0x36,0x63,0xF7,0x00,0x00}, /* X */
    {0x00,0x00,0x00,0xFF,0x66,0x66,0x3C,0x3C,0x18,0x18,0x18,0x18,0x18,0x3C,0x00,0x00}, /* Y */
    {0x00,0x00,0x00,0x7F,0xC6,0x06,0x0C,0x0C,0x18,0x30,0x30,0x63,0x63,0xFE,0x00,0x00}, /* Z */

    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, /*  */
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7F,0x7F,0x7F,0x00,0x00,0x00,0x00,0x00}, /* - */
    {0x00,0x00,0x00,0x00,0x00,0x00,0x1C,0x1C,0x00,0x00,0x00,0x00,0x1C,0x1C,0x00,0x00}, /* : */
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x70,0x70,0x00,0x00}, /* . */
    {0x00,0x70,0x70,0x30,0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, /* ' */
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, /*  */
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, /*  */
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, /*  */
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, /*  */
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}  /*  */

};

const HI_U8 g_aucASCIIPointMap[VPP_ASCII_MAP_SIZE][VPP_CHR_REAL_HEIGRH*2] =
{

    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0xF0,0x07,0x38,0x0E,0x1C,0x1C,0x0E,0x1C,0x0E,0x1C,0x06,0x38,0x07,0x38,0x07,0x38,0x07,0x38,0x07,0x38,0x07,0x38,0x07,0x38,0x07,0x38,0x07,0x38,0x07,0x1C,0x06,0x1C,0x0E,0x1C,0x0E,0x0E,0x1C,0x07,0x38,0x03,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},//;"0",0

    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xC0,0x01,0xC0,0x1F,0xC0,0x01,0xC0,0x01,0xC0,0x01,0xC0,0x01,0xC0,0x01,0xC0,0x01,0xC0,0x01,0xC0,0x01,0xC0,0x01,0xC0,0x01,0xC0,0x01,0xC0,0x01,0xC0,0x01,0xC0,0x01,0xC0,0x01,0xC0,0x01,0xC0,0x03,0xE0,0x1F,0xFC,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},//;"1",1

    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0xF0,0x0C,0x3C,0x18,0x1C,0x30,0x0E,0x30,0x0E,0x38,0x0E,0x38,0x0E,0x00,0x0E,0x00,0x1C,0x00,0x18,0x00,0x30,0x00,0x60,0x00,0xC0,0x01,0x80,0x03,0x00,0x06,0x06,0x0C,0x06,0x18,0x06,0x30,0x0E,0x3F,0xFC,0x3F,0xFC,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},//;"2",2

    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0xE0,0x1C,0x78,0x38,0x38,0x38,0x1C,0x38,0x1C,0x38,0x1C,0x00,0x1C,0x00,0x38,0x00,0x70,0x03,0xE0,0x00,0x78,0x00,0x1C,0x00,0x0C,0x00,0x0E,0x00,0x0E,0x38,0x0E,0x38,0x0E,0x38,0x0C,0x38,0x1C,0x1C,0x38,0x07,0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},//;"3",3

    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x38,0x00,0x78,0x00,0x78,0x00,0xF8,0x01,0xF8,0x01,0xF8,0x03,0x78,0x07,0x78,0x06,0x78,0x0C,0x78,0x0C,0x78,0x18,0x78,0x30,0x78,0x30,0x78,0x7F,0xFF,0x00,0x78,0x00,0x78,0x00,0x78,0x00,0x78,0x00,0x78,0x00,0x78,0x03,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},//;"4",4

    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0F,0xFE,0x0F,0xFE,0x0C,0x00,0x0C,0x00,0x0C,0x00,0x18,0x00,0x18,0x00,0x1B,0xF0,0x1E,0x38,0x1C,0x1C,0x18,0x0C,0x00,0x0E,0x00,0x0E,0x00,0x0E,0x00,0x0E,0x38,0x0E,0x38,0x0E,0x30,0x1C,0x30,0x1C,0x18,0x38,0x0F,0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},//;"5",5

    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xF8,0x03,0x8C,0x06,0x0E,0x0C,0x0E,0x1C,0x00,0x1C,0x00,0x18,0x00,0x38,0x00,0x39,0xF8,0x3F,0x1C,0x3E,0x0E,0x3C,0x07,0x38,0x07,0x38,0x07,0x38,0x07,0x38,0x07,0x1C,0x07,0x1C,0x06,0x0E,0x0E,0x07,0x1C,0x03,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},//;"6",6

    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1F,0xFE,0x1F,0xFE,0x3C,0x0C,0x38,0x18,0x30,0x18,0x30,0x30,0x00,0x30,0x00,0x60,0x00,0x60,0x00,0xC0,0x00,0xC0,0x00,0xC0,0x01,0x80,0x01,0x80,0x01,0x80,0x03,0x80,0x03,0x80,0x03,0x80,0x03,0x80,0x03,0x80,0x03,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},//;"7",7

    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0xF0,0x0E,0x38,0x1C,0x1C,0x38,0x0E,0x38,0x0E,0x38,0x0E,0x3C,0x0E,0x1E,0x1C,0x0F,0x18,0x07,0xF0,0x0F,0xF0,0x1C,0x78,0x38,0x3C,0x70,0x1E,0x70,0x0E,0x70,0x0E,0x70,0x0E,0x70,0x0E,0x38,0x1C,0x1C,0x38,0x07,0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},//;"8",8

    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0xE0,0x1C,0x30,0x38,0x18,0x38,0x1C,0x70,0x0C,0x70,0x0E,0x70,0x0E,0x70,0x0E,0x70,0x0E,0x70,0x1E,0x38,0x3E,0x1C,0x7E,0x0F,0xCE,0x00,0x0E,0x00,0x1C,0x00,0x1C,0x00,0x1C,0x38,0x38,0x38,0x70,0x38,0xE0,0x0F,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},//;"9",9

    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xC0,0x03,0xC0,0x03,0xC0,0x03,0xC0,0x03,0xC0,0x07,0xE0,0x06,0xE0,0x06,0xE0,0x06,0xE0,0x0E,0x70,0x0C,0x70,0x0C,0x70,0x0C,0x70,0x1F,0xF8,0x18,0x38,0x18,0x38,0x18,0x38,0x38,0x38,0x30,0x1C,0x30,0x1C,0x70,0x1C,0xFC,0x3F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},//;"A",10

    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7F,0xF0,0x1C,0x3C,0x1C,0x1E,0x1C,0x0E,0x1C,0x0E,0x1C,0x0E,0x1C,0x0E,0x1C,0x1C,0x1C,0x38,0x1F,0xF0,0x1C,0x1C,0x1C,0x0E,0x1C,0x06,0x1C,0x07,0x1C,0x07,0x1C,0x07,0x1C,0x07,0x1C,0x07,0x1C,0x0E,0x1C,0x1C,0x7F,0xF8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},//;"B",11

    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0xF6,0x07,0x1E,0x0C,0x0E,0x1C,0x06,0x38,0x03,0x38,0x03,0x30,0x00,0x70,0x00,0x70,0x00,0x70,0x00,0x70,0x00,0x70,0x00,0x70,0x00,0x70,0x00,0x70,0x00,0x38,0x03,0x38,0x03,0x38,0x06,0x1C,0x0E,0x0E,0x1C,0x03,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},//;"C",12

    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7F,0xE0,0x1C,0x78,0x1C,0x1C,0x1C,0x0E,0x1C,0x0E,0x1C,0x0E,0x1C,0x07,0x1C,0x07,0x1C,0x07,0x1C,0x07,0x1C,0x07,0x1C,0x07,0x1C,0x07,0x1C,0x07,0x1C,0x07,0x1C,0x0E,0x1C,0x0E,0x1C,0x0C,0x1C,0x1C,0x1C,0x78,0x7F,0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},//;"D",13

    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7F,0xFE,0x1C,0x0E,0x1C,0x06,0x1C,0x07,0x1C,0x03,0x1C,0x00,0x1C,0x18,0x1C,0x18,0x1C,0x38,0x1F,0xF8,0x1C,0x38,0x1C,0x18,0x1C,0x18,0x1C,0x00,0x1C,0x00,0x1C,0x00,0x1C,0x03,0x1C,0x03,0x1C,0x06,0x1C,0x0E,0x7F,0xFE,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},//;"E",14

    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7F,0xFF,0x1C,0x0F,0x1C,0x03,0x1C,0x03,0x1C,0x01,0x1C,0x00,0x1C,0x0C,0x1C,0x0C,0x1C,0x1C,0x1F,0xFC,0x1C,0x1C,0x1C,0x0C,0x1C,0x0C,0x1C,0x00,0x1C,0x00,0x1C,0x00,0x1C,0x00,0x1C,0x00,0x1C,0x00,0x1C,0x00,0x7F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},//;"F",15

    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0xEC,0x0F,0x3C,0x0C,0x0C,0x1C,0x0C,0x38,0x06,0x38,0x06,0x30,0x00,0x70,0x00,0x70,0x00,0x70,0x00,0x70,0x00,0x70,0x00,0x70,0x3F,0x70,0x0E,0x70,0x0E,0x38,0x0E,0x38,0x0E,0x1C,0x0E,0x1C,0x0E,0x0E,0x18,0x03,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},//;"G",16

    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFE,0x7F,0x38,0x1C,0x38,0x1C,0x38,0x1C,0x38,0x1C,0x38,0x1C,0x38,0x1C,0x38,0x1C,0x38,0x1C,0x38,0x1C,0x3F,0xFC,0x38,0x1C,0x38,0x1C,0x38,0x1C,0x38,0x1C,0x38,0x1C,0x38,0x1C,0x38,0x1C,0x38,0x1C,0x38,0x1C,0xFE,0x7F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},//;"H",17

    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1F,0xFC,0x01,0xC0,0x01,0xC0,0x01,0xC0,0x01,0xC0,0x01,0xC0,0x01,0xC0,0x01,0xC0,0x01,0xC0,0x01,0xC0,0x01,0xC0,0x01,0xC0,0x01,0xC0,0x01,0xC0,0x01,0xC0,0x01,0xC0,0x01,0xC0,0x01,0xC0,0x01,0xC0,0x01,0xC0,0x1F,0xFC,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},//;"I",18

    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0xFF,0x00,0x70,0x00,0x70,0x00,0x70,0x00,0x70,0x00,0x70,0x00,0x70,0x00,0x70,0x00,0x70,0x00,0x70,0x00,0x70,0x00,0x70,0x00,0x70,0x00,0x70,0x00,0x70,0x00,0x70,0x00,0x70,0x00,0x70,0x00,0x70,0x00,0x70,0x00,0x70,0x00,0x70,0x78,0x70,0x78,0xE0,0x79,0xC0,0x3F,0x80},//;"J",19

    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7F,0x3F,0x1C,0x1C,0x1C,0x18,0x1C,0x30,0x1C,0x70,0x1C,0x60,0x1C,0xC0,0x1D,0xC0,0x1D,0xC0,0x1F,0xC0,0x1F,0xE0,0x1E,0xE0,0x1C,0xF0,0x1C,0x70,0x1C,0x78,0x1C,0x38,0x1C,0x3C,0x1C,0x1C,0x1C,0x0E,0x1C,0x0E,0x7F,0x3F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},//;"K",20

    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7F,0x00,0x1C,0x00,0x1C,0x00,0x1C,0x00,0x1C,0x00,0x1C,0x00,0x1C,0x00,0x1C,0x00,0x1C,0x00,0x1C,0x00,0x1C,0x00,0x1C,0x00,0x1C,0x00,0x1C,0x00,0x1C,0x00,0x1C,0x00,0x1C,0x03,0x1C,0x03,0x1C,0x06,0x1C,0x0E,0x7F,0xFE,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},//;"L",21

    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFC,0x1F,0x3C,0x1E,0x3C,0x1E,0x3C,0x1E,0x3C,0x3E,0x3E,0x3E,0x3E,0x3E,0x3E,0x3E,0x3E,0x7E,0x3F,0x6E,0x37,0x6E,0x37,0x6E,0x37,0x6E,0x37,0xCE,0x33,0xCE,0x33,0xCE,0x33,0xCE,0x33,0x8E,0x33,0x8E,0x31,0x8E,0xFD,0xBF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},//;"M",22

    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFC,0x3F,0x3C,0x0C,0x3E,0x0C,0x3E,0x0C,0x3E,0x0C,0x3F,0x0C,0x37,0x0C,0x37,0x8C,0x33,0x8C,0x33,0xCC,0x31,0xCC,0x31,0xEC,0x30,0xEC,0x30,0xFC,0x30,0x7C,0x30,0x7C,0x30,0x3C,0x30,0x3C,0x30,0x3C,0x30,0x1C,0xFC,0x1C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},//;"N",23

    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0xE0,0x0E,0x38,0x1C,0x1C,0x18,0x0C,0x38,0x0E,0x38,0x0E,0x70,0x07,0x70,0x07,0x70,0x07,0x70,0x07,0x70,0x07,0x70,0x07,0x70,0x07,0x70,0x07,0x70,0x07,0x38,0x06,0x38,0x0E,0x18,0x0C,0x1C,0x1C,0x0E,0x38,0x03,0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},//;"O",24

    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7F,0xF8,0x1C,0x1C,0x1C,0x0E,0x1C,0x07,0x1C,0x07,0x1C,0x07,0x1C,0x07,0x1C,0x07,0x1C,0x0E,0x1C,0x1C,0x1F,0xF8,0x1C,0x00,0x1C,0x00,0x1C,0x00,0x1C,0x00,0x1C,0x00,0x1C,0x00,0x1C,0x00,0x1C,0x00,0x1C,0x00,0x7F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},//;"P",25

    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0xE0,0x0E,0x38,0x1C,0x1C,0x38,0x0C,0x38,0x0E,0x38,0x06,0x70,0x07,0x70,0x07,0x70,0x07,0x70,0x07,0x70,0x07,0x70,0x07,0x70,0x07,0x70,0x07,0x70,0x07,0x37,0xC7,0x3E,0xEE,0x3C,0x6E,0x1C,0x7C,0x0E,0x78,0x03,0xF0,0x00,0x3B,0x00,0x3F,0x00,0x1E,0x00,0x00,0x00,0x00},//;"Q",26

    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7F,0xF0,0x1C,0x3C,0x1C,0x1E,0x1C,0x0E,0x1C,0x0E,0x1C,0x0E,0x1C,0x0E,0x1C,0x1C,0x1C,0x38,0x1F,0xF0,0x1D,0xE0,0x1C,0xE0,0x1C,0xF0,0x1C,0x70,0x1C,0x70,0x1C,0x78,0x1C,0x38,0x1C,0x38,0x1C,0x3C,0x1C,0x1C,0x7F,0x1F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},//;"R",27

    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0xF6,0x0E,0x1E,0x1C,0x0E,0x38,0x06,0x38,0x06,0x38,0x00,0x38,0x00,0x1C,0x00,0x1F,0x00,0x07,0xE0,0x01,0xF8,0x00,0x7C,0x00,0x1E,0x00,0x0F,0x00,0x07,0x30,0x07,0x30,0x07,0x18,0x07,0x1C,0x0E,0x1E,0x1C,0x1B,0xF8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},//;"S",28

    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3F,0xFE,0x39,0xC6,0x31,0xC7,0x61,0xC3,0x61,0xC3,0x01,0xC0,0x01,0xC0,0x01,0xC0,0x01,0xC0,0x01,0xC0,0x01,0xC0,0x01,0xC0,0x01,0xC0,0x01,0xC0,0x01,0xC0,0x01,0xC0,0x01,0xC0,0x01,0xC0,0x01,0xC0,0x01,0xC0,0x07,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},//;"T",29

    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFE,0x3F,0x38,0x0C,0x38,0x0C,0x38,0x0C,0x38,0x0C,0x38,0x0C,0x38,0x0C,0x38,0x0C,0x38,0x0C,0x38,0x0C,0x38,0x0C,0x38,0x0C,0x38,0x0C,0x38,0x0C,0x38,0x0C,0x38,0x0C,0x38,0x0C,0x38,0x0C,0x18,0x18,0x1E,0x30,0x07,0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},//;"U",30

    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7E,0x1F,0x1C,0x06,0x1C,0x06,0x1C,0x06,0x0E,0x0C,0x0E,0x0C,0x0E,0x0C,0x0E,0x0C,0x07,0x18,0x07,0x18,0x07,0x18,0x07,0xB8,0x03,0xB0,0x03,0xB0,0x03,0xB0,0x01,0xE0,0x01,0xE0,0x01,0xE0,0x01,0xE0,0x00,0xC0,0x00,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},//;"V",31

    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xEF,0x71,0xC7,0x71,0xC6,0x71,0xC6,0x39,0xC6,0x38,0xC6,0x39,0xE6,0x39,0xEC,0x39,0xEC,0x39,0xEC,0x3B,0xEC,0x1F,0x6C,0x1F,0x7C,0x1F,0x78,0x1E,0x78,0x1E,0x78,0x1E,0x78,0x0E,0x38,0x0C,0x30,0x0C,0x30,0x0C,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},//;"W",32

    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7F,0x7E,0x1C,0x18,0x1E,0x18,0x0E,0x30,0x0E,0x30,0x0F,0x60,0x07,0x60,0x07,0xC0,0x03,0xC0,0x03,0xC0,0x01,0xC0,0x01,0xE0,0x03,0xE0,0x03,0xF0,0x06,0x70,0x06,0x70,0x0C,0x38,0x0C,0x38,0x18,0x1C,0x18,0x1C,0x7E,0x3F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},//;"X",33

    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7F,0x3F,0x3C,0x0C,0x1C,0x0C,0x1C,0x18,0x0E,0x18,0x0E,0x38,0x0F,0x30,0x07,0x30,0x07,0x60,0x03,0xE0,0x03,0xE0,0x03,0xC0,0x01,0xC0,0x01,0xC0,0x01,0xC0,0x01,0xC0,0x01,0xC0,0x01,0xC0,0x01,0xC0,0x01,0xC0,0x07,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},//;"Y",34

    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1F,0xFE,0x1C,0x1C,0x18,0x1C,0x30,0x38,0x30,0x78,0x00,0x70,0x00,0xF0,0x00,0xE0,0x01,0xE0,0x01,0xC0,0x03,0xC0,0x03,0x80,0x07,0x80,0x07,0x00,0x0F,0x00,0x0E,0x00,0x1E,0x06,0x1C,0x06,0x3C,0x0C,0x38,0x1C,0x7F,0xFC,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},//;"Z",35

    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},//;" ",36

    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7F,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},//;"-",37

    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xC0,0x03,0xE0,0x03,0xE0,0x01,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xC0,0x03,0xE0,0x03,0xE0,0x01,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},//;":",38

    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3C,0x00,0x3E,0x00,0x3E,0x00,0x0E,0x00,0x0E,0x00,0x0C,0x00,0x38,0x00,0x70,0x00},//;",",39

    {0x00,0x00,0x00,0x00,0x00,0x00,0x3C,0x00,0x3E,0x00,0x3E,0x00,0x0E,0x00,0x0E,0x00,0x0C,0x00,0x38,0x00,0x70,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},//;"'",40

    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},//;" ",41

    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},//;" ",42

    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},//;" ",43

    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},//;" ",44

    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}//;" ",45

};

HI_S32 APP_GetOneChnAllBlockInfo(FILE* pFile, HI_CHAR *pcBuffer, STOR_BLK_IDX_CHN_HEAD_T *pstIndexHead, HI_S32 iChn);
/*****************************************************************************
 函 数 名  : APP_WrOneMsg
 功能描述  : 写入一条日志信息
 输入参数  : FILE *pRecLogFp
             HI_S32 iRecode
             HI_S32 iEvent
 输出参数  : 无
 返 回 值  :

 修改历史      :
  1.日    期   : 2014年7月6日
    作    者   : zhangjinlei
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 APP_WrOneMsg(FILE *pRecLogFp, HI_S32 iRecode,HI_S32 iEvent ,HI_S32 s32ChnStat)
{
    HI_S32 s32Ret = 0;
    HI_CHAR cTmpBuffer[64]= {0x20};
    STOR_REC_LOG_T *pstRecLog = NULL;
    HI_CHAR *pcTmp = NULL;
    time_t t = time(NULL);
    HI_CHAR buffer[64]= {0};
    HI_CHAR cTmp[64] = {0};

    memset(cTmpBuffer, ' ', sizeof(cTmpBuffer)- 1);
    cTmpBuffer[63] = 0x0A;

    if (NULL == pRecLogFp)
    {
        AV_PRINT( " Input NULL pointer!");
        return HI_FAILURE;
    }

    s32Ret = fseek(pRecLogFp,sizeof(STOR_PARAM_ST),SEEK_SET);
    if(s32Ret < 0)
    {
        AV_PRINT( " lseek recode config err!s32Ret=%d",s32Ret);
        return HI_FAILURE;
    }

    pstRecLog = (STOR_REC_LOG_T *)malloc(sizeof(STOR_REC_LOG_T));
    if (NULL == pstRecLog)
    {
        AV_PRINT( " malloc memory failed");
        return HI_FAILURE;
    }

    s32Ret = fread(pstRecLog, 1, sizeof(STOR_REC_LOG_T), pRecLogFp);
    if(s32Ret<0)
    {
        AV_PRINT( " read recode log config err!");
        free(pstRecLog);
        pstRecLog = NULL;
        return HI_FAILURE;
    }

    if ((pstRecLog->s32NewestIndex >= STOR_LOG_MAX_NUMS) ||
            (pstRecLog->s32OldestIndex >= STOR_LOG_MAX_NUMS) ||
            (pstRecLog->s32Counter > STOR_LOG_MAX_NUMS))
    {
        pstRecLog->s32NewestIndex = 0;
        pstRecLog->s32OldestIndex = 0;
        pstRecLog->s32Counter = 0;
    }

    strftime(cTmp, sizeof(cTmp), "%Y/%m/%d %X: ", localtime(&t) );
    sprintf(buffer,"stat=0x%x,Eve=0x%x,chn=%d", iRecode, iEvent, s32ChnStat);
    strcat(cTmp,buffer);

    memcpy(cTmpBuffer, cTmp, strlen(cTmp));

    pcTmp = (HI_CHAR *)((HI_CHAR *)pstRecLog->cLogList + pstRecLog->s32NewestIndex*STOR_LOG_ITEM_LEN);
    memcpy(pcTmp, cTmpBuffer, sizeof(cTmpBuffer));

    //AV_PRINT( " s32NewestIndex=%d,s32OldestIndex=%d,s32Counter=%d",
    //    pstRecLog->s32NewestIndex, pstRecLog->s32OldestIndex, pstRecLog->s32Counter);

    if (pstRecLog->s32Counter < STOR_LOG_MAX_NUMS)
    {
        pstRecLog->s32Counter++;
    }
    else
    {
        pstRecLog->s32OldestIndex = (pstRecLog->s32OldestIndex+1)%STOR_LOG_MAX_NUMS;
    }

    pstRecLog->s32NewestIndex = (pstRecLog->s32NewestIndex+1)%STOR_LOG_MAX_NUMS;

    s32Ret = fseek(pRecLogFp, sizeof(STOR_PARAM_ST), SEEK_SET);
    if(s32Ret < 0)
    {
        AV_PRINT( " lseek recode config err!s32Ret=%d",s32Ret);
        free(pstRecLog);
        pstRecLog = NULL;
        return HI_FAILURE;
    }

    s32Ret=fwrite(pstRecLog, 1, sizeof(STOR_REC_LOG_T), pRecLogFp);
    if(s32Ret < 0)
    {
        AV_PRINT( " write recode log config err!");
        free(pstRecLog);
        pstRecLog = NULL;
        return HI_FAILURE;
    }

    fflush(pRecLogFp);
    free(pstRecLog);
    pstRecLog = NULL;

    return HI_SUCCESS;
}



/*****************************************************************************
 函 数 名  : APP_VPPSetOneChar
 功能描述  : 填充单个字符到BMP缓冲区
 输入参数  : HI_S32 iWidth
             HI_S32 iHeight
             HI_S32 iOffset
             HI_CHAR cChar
             HI_U8 *pucBuffer
 输出参数  : 无
 返 回 值  :



 修改历史      :
  1.日    期   : 2012年9月14日星期五
    作    者   : zhangjinlei
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 APP_VPPSetOneChar(HI_S32 iWidth, HI_S32 iHeight, HI_S32 iOffset,
                         HI_CHAR cChar, HI_U8 *pucBuffer)
{
    HI_S32 i = 0;
    HI_S32 j = 0;
    HI_S32 k = 0;
    HI_U8 *pucTmp = NULL;
    HI_U8 aucCharPointBuf[VPP_CHR_REAL_HEIGRH*2];
    HI_S32 s32Index = 0;
    HI_U8 uc1555Buffer[4] = {0xff,0xff,0xff,0x80};//?????,,绿，红，透明度
    HI_U8 ucTmpPoint = 0;
    HI_U32 iTmpValue = 0;
    HI_U32 iTmpPointAddr = 0;

    if (NULL == pucBuffer)
    {
        AV_PRINT( "Input null pointer!");
        return HI_FAILURE;
    }

    pucTmp = pucBuffer;
    memset(aucCharPointBuf, 0, sizeof(aucCharPointBuf));
    /* 在ASCII表中查找对应的字符，获取数组下标 */
    for (i = 0; i < VPP_ASCII_MAP_SIZE; i++)
    {
        if (cChar == g_aucASCIIMap[i])
        {
            s32Index = i;
            break;
        }
    }
    if (i >= VPP_ASCII_MAP_SIZE)
    {
        AV_PRINT("this HI_CHAR can not find in ASIIMAP, cChar=%c", cChar);

        s32Index = 36;
    }
    /* 获取对应字符的点阵数组 */
    memcpy(aucCharPointBuf, g_aucASCIIPointMap[s32Index], VPP_CHR_REAL_HEIGRH*2);
    for ( i = 0; i < VPP_CHR_REAL_HEIGRH; i++ )//每行要处理两个字节
    {
        iTmpValue = (((VPP_ONE_CHR_HEIGHT - VPP_CHR_REAL_HEIGRH)/2 + i)* iWidth +
                     iOffset*VPP_ONE_CHR_WIDTH +1)*2;
        ucTmpPoint = aucCharPointBuf[2*i];
        if (0x00 != ucTmpPoint)
        {
            for ( j = 0; j < VPP_CHR_REAL_WIDTH/2; j++ )
            {
                k = (ucTmpPoint & (0x80 >> j));//像素点的每个bit代表一个像素，用三个字节表示
                if(0 == k)
                {
                    continue;
                }
                iTmpPointAddr = iTmpValue + j * 4;

                memset(pucTmp + iTmpPointAddr, uc1555Buffer[0], sizeof(HI_CHAR));
                memset(pucTmp + iTmpPointAddr+1, uc1555Buffer[1], sizeof(HI_CHAR));
                memset(pucTmp + iTmpPointAddr+2, uc1555Buffer[2], sizeof(HI_CHAR));
                memset(pucTmp + iTmpPointAddr+3, uc1555Buffer[3], sizeof(HI_CHAR));
            }
        }
        ucTmpPoint = aucCharPointBuf[2*i+1];
        if (0x00 != ucTmpPoint)
        {
            for ( j = 8; j < VPP_CHR_REAL_WIDTH; j++ )
            {
                k = (ucTmpPoint & (0x80 >> (j-8)));//像素点的每个bit代表一个像素，用三个字节表示
                if(0 == k)
                {
                    continue;
                }

                iTmpPointAddr = iTmpValue + j * 4;

                memset(pucTmp + iTmpPointAddr, uc1555Buffer[0], sizeof(HI_CHAR));
                memset(pucTmp + iTmpPointAddr+1, uc1555Buffer[1], sizeof(HI_CHAR));
                memset(pucTmp + iTmpPointAddr+2, uc1555Buffer[2], sizeof(HI_CHAR));
                memset(pucTmp + iTmpPointAddr+3, uc1555Buffer[3], sizeof(HI_CHAR));
            }
        }
    }

    return HI_SUCCESS;
}


/* BEGIN: Added by zhangjinlei, 2014/9/17  DES:在视频界面添加通道号和分割线  PN:14091701006 */
/*****************************************************************************
 Prototype    : APP_LoadCHNbmpChar
 Description  : 填充通道号图片
 Input        : const HI_CHAR *pcBuffer
                HI_CHAR *pDataAddr
                HI_S32 s32Len
 Output       : None
 Return Value :

  History        :
  1.Date         : 2014/9/17
    Author       : zhangjinlei
    Modification : Created function

*****************************************************************************/
HI_S32 APP_LoadCHNbmpChar(const HI_CHAR *pcBuffer, HI_CHAR *pDataAddr, HI_S32 s32Len)
{
    HI_S32 s32Ret = 0;
    HI_S32 i = 0;
    HI_S32 iBufferLen = 0;
    HI_S32 iWidth = 0;
    HI_S32 iHeight = 0;
    HI_S32 iBmpSize = 0;  /* 所占用内存大小 */
//   short usBmpBackGRB = 0x00;
    HI_CHAR c1555rgb[4] = {0x00,0x00,0x00,0x00};
    HI_U8 *pucBuffer = NULL;
    HI_CHAR cTmp = 0;

    if ((NULL == pcBuffer) || (NULL == pDataAddr))
    {
        AV_PRINT("input para is invalid");
        return HI_FAILURE;
    }

    iWidth =  APP_RECODE_CHN_WIDTH + 2;
    iHeight = APP_RECODE_CHN_HEIGHT;
    iBmpSize = 4*iWidth*iHeight;

    pucBuffer = (HI_U8 *)pDataAddr;
    iBufferLen = s32Len;

    /* 设置字体显示的背景色 */
    for ( i = 0; i < iBmpSize; i=i+4)
    {
        memset(pucBuffer+i, c1555rgb[0], sizeof(HI_CHAR));
        memset(pucBuffer+i+1, c1555rgb[1], sizeof(HI_CHAR));
        memset(pucBuffer+i+2, c1555rgb[2], sizeof(HI_CHAR));
        memset(pucBuffer+i+3, c1555rgb[3], sizeof(HI_CHAR));
    }

    for (i = 0 ; i < iBufferLen; i++ )
    {
        cTmp =  *(pcBuffer + i);
        /* 填充单个字符 */
        s32Ret = APP_VPPSetOneChar(iWidth, iHeight, i, cTmp, pucBuffer);
        if (HI_SUCCESS != s32Ret)
        {
            AV_PRINT( " APP_VPPSetOneChar err!p1=%d,p2=%d,p3=%d,p4=%c",
                      iWidth, iHeight, i, cTmp);

            return HI_FAILURE;
        }
    }

    return HI_SUCCESS;
}


/*****************************************************************************
 函 数 名  : APP_GetFbMark
 功能描述  :
 输入参数  : FB_MARK_T **pFbMark
 输出参数  : FB_MARK_T **pFbMark
 返 回 值  :

 修改历史      :
  1.日    期   : 2014年12月09日
    作    者   : xzhang
    修改内容   : 新生成函数

*****************************************************************************/
HI_VOID APP_GetFbMark(FB_MARK_T **pFbMark)
{
    *pFbMark = &gFbMark;
    return;
}


/* END:   Added by zhangjinlei, 2014/9/17   PN:14091701006 */
/*****************************************************************************
 函 数 名  : APP_SetRecMark
 功能描述  : 设置录像标志
 输出参数  : 无
 返 回 值  :

 修改历史      :s32SplitMode s32SplitIndex
  1.日    期   : 2013年6月28日
    作    者   : zhangjinlei
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 APP_SetRecMark(HI_S32 bIsOpen, HI_S32 s32SplitMode, HI_S32 s32SplitIndex)
{
    HI_S32 s32Ret = 0;
    HI_S32 iIsMoreChn = 0;
    HI_CHAR *pcMemHead = NULL;
    HI_CHAR *pcDst = NULL;
    HI_U32 u32ScreenStride = VIR_SCREEN_WIDTH*4;
    HI_S32 j = 0;
    HI_S32 i = 0;
    HI_S32 k = 0;
    HI_U32 uiColorValue = 0;
    HI_U32 uiRedValue = 0xFc00;  /* 红色   */
    HIFB_POINT_S stPoint[6] = { {APP_RECODE_POINT_1_X, APP_RECODE_POINT_1_Y},
        {APP_RECODE_POINT_2_X, APP_RECODE_POINT_2_Y},
        {APP_RECODE_POINT_3_X, APP_RECODE_POINT_3_Y},
        {APP_RECODE_POINT_4_X, APP_RECODE_POINT_4_Y},
        {APP_RECODE_POINT_5_X, APP_RECODE_POINT_5_Y},
        {0, 0}
    };
    /*4通道或者1通道的红点位置 */
    HIFB_POINT_S stChnPoint[6] = {  {APP_RECODE_CHN_1_X, APP_RECODE_CHN_1_Y},
        {APP_RECODE_CHN_2_X, APP_RECODE_CHN_2_Y},
        {APP_RECODE_CHN_3_X, APP_RECODE_CHN_3_Y},
        {APP_RECODE_CHN_4_X, APP_RECODE_CHN_4_Y},
        {APP_RECODE_CHN_5_X, APP_RECODE_CHN_5_Y},
        {0, 0}
    };
    HI_CHAR *pcChnDst = NULL;
    HI_U32 uiShowOrNot = 0;
    HI_S32 iWidth = 0;
    HI_S32 iHeight = 0;
    HI_S32 iBmpSize = 0;  /* 所占用内存大小 */
    HI_CHAR *pucBuffer = NULL;
    HI_CHAR *pcChnStr = NULL;
    HI_CHAR *pcTmp = NULL;

    HIFB_POINT_S stLinePoint[3] = {  {APP_RECODE_LINE_1_X, APP_RECODE_LINE_1_Y},
        {APP_RECODE_LINE_2_X, APP_RECODE_LINE_2_Y},
        {0, 0}
    };

    FB_MARK_T *pFbMark = NULL;
    APP_GetFbMark(&pFbMark);

    if ((pFbMark->s32Fd == -1) || (NULL == pFbMark->pcMemAddr))
    {
        AV_PRINT("FbMark para is invalid");
        return HI_FAILURE;
    }

    pcMemHead = pFbMark->pcMemAddr;
    if ((s32SplitMode < PREVIEW_CHN1) || (s32SplitMode > PREVIEW_9_CHN_SCREEN))
    {
        return HI_FAILURE;
    }

    //AV_PRINT("s32SplitMode(%d), s32SplitIndex(%d)",
    //			s32SplitMode, s32SplitIndex);

    /*49画面为多画面*/
    switch (s32SplitMode)
    {
    case PREVIEW_4_CHN_SCREEN1:
    case PREVIEW_4_CHN_SCREEN2:
    case PREVIEW_4_CHN_SCREEN3:
    case PREVIEW_9_CHN_SCREEN:
        iIsMoreChn = 1;
        break;
    default:
        iIsMoreChn = 0;
        break;
    }

    iWidth =  APP_RECODE_CHN_WIDTH + 2;
    iHeight = APP_RECODE_CHN_HEIGHT;
    iBmpSize = 4*iWidth*iHeight;

    pucBuffer = (HI_CHAR *)malloc(iBmpSize);
    if(NULL == pucBuffer)
    {
        AV_PRINT("malloc osd memroy err(%s)!p1=%d,p2=%d,p3=%d", ERR_STR, iHeight, iWidth, iBmpSize);
        return HI_FAILURE;
    }

    /* 9分屏不需要分割线和录像标志 */
    if(s32SplitMode == PREVIEW_9_CHN_SCREEN)
    {
        bIsOpen = 0;
    }
    for ( i = 0; i < 5; i++)
    {
        uiColorValue = 0;
        uiShowOrNot = 0;

        if (i < 4)
        {
            /*4通道模式 */
            if ((s32SplitMode == PREVIEW_4_CHN_SCREEN2) && (1 == s32SplitIndex))
            {
                /*中4通道 */
                if (STOR_GetChnRecStat(i+4))
                {
                    uiColorValue = uiRedValue * bIsOpen * iIsMoreChn;
                }
                uiShowOrNot = ((0xFF&(0x01 << (i+4)))>0?1:0)*iIsMoreChn;
                if (uiShowOrNot)
                {
                    pcChnStr = g_acpBmpBuffer[i+4];
                }
            }
            else if((s32SplitMode == PREVIEW_4_CHN_SCREEN1)&&(0 == s32SplitIndex))
            {
                /*前4通道*/
                if (STOR_GetChnRecStat(i))
                {
                    uiColorValue = uiRedValue * bIsOpen * iIsMoreChn;
                }
                uiShowOrNot = ((0xFF&(0x01 << i))>0?1:0)*iIsMoreChn;
                if (uiShowOrNot)
                {
                    pcChnStr = g_acpBmpBuffer[i];
                }

            }
            else if((s32SplitMode == PREVIEW_4_CHN_SCREEN3)&&(1 == s32SplitIndex))
            {
                /*后4通道*/
                if (STOR_GetChnRecStat(i))
                {
                    uiColorValue = uiRedValue * bIsOpen * iIsMoreChn;
                }
                uiShowOrNot = ((0xFF&(0x01 << (i+4)))>0?1:0)*iIsMoreChn;
                if (uiShowOrNot)
                {
                    pcChnStr = g_acpBmpBuffer[i+8];
                }
            }
        }
        else
        {
            if (STOR_GetChnRecStat(s32SplitMode-1))
            {
                uiColorValue = uiRedValue * bIsOpen * (!iIsMoreChn);
            }

            uiShowOrNot = ((0xFF&(0x01 << (s32SplitMode-1)))>0?1:0)*(!iIsMoreChn);
            if (uiShowOrNot)
            {
                pcChnStr = g_acpBmpBuffer[s32SplitMode-1];
            }
        }
        pcDst = pcMemHead + stPoint[i].s32YPos*u32ScreenStride + stPoint[i].s32XPos*4;
        pcChnDst = pcMemHead + stChnPoint[i].s32YPos*u32ScreenStride + stChnPoint[i].s32XPos*4;
        if (1 == uiShowOrNot)
        {
            s32Ret = APP_LoadCHNbmpChar(pcChnStr, pucBuffer, strlen(pcChnStr));
            if (HI_SUCCESS != s32Ret)
            {
                AV_PRINT("APP_VPPSetOneChar err!p1=%d,p2=%d,p4=%s", iWidth, iHeight, pcChnStr);
                free(pucBuffer);
                pucBuffer = NULL;
                return HI_FAILURE;
            }

        }
        uiColorValue = 0xFFFF0000;
        HI_CHAR c1555rgb[4] = {0x00,0x40,0xf0,0x63};//最后一个字节代表透明度
        /* 绘制录像标志 */
        for(j = 0; j < APP_RECODE_HEIGHT; j++)
        {
//		memset(pcDst,uiColorValue,APP_RECODE_WIDTH*4);
            for(k = 0; k < APP_RECODE_WIDTH*4; k=k+4)
            {
                memset(pcDst+k, c1555rgb[0], sizeof(HI_CHAR));
                memset(pcDst+k+1, c1555rgb[1], sizeof(HI_CHAR));
                memset(pcDst+k+2, c1555rgb[2], sizeof(HI_CHAR));
                memset(pcDst+k+3, c1555rgb[3], sizeof(HI_CHAR));
            }
            pcDst += u32ScreenStride;
            if (1 == uiShowOrNot)
            {
                pcTmp = pucBuffer + j*iWidth*2;
                memcpy(pcChnDst,pcTmp,iWidth*2);
                pcChnDst += u32ScreenStride;
            }
            else
            {
                memset(pcChnDst,uiShowOrNot,iWidth*2);
                pcChnDst += u32ScreenStride;
            }
        }
    }
    if (APP_VIDEOOUT_ISMORE_YES == iIsMoreChn)
    {
        uiColorValue = 0xFFFFFF;

    }
    else
    {
        uiColorValue = 0;
    }

    /* 绘制4分屏的分割线 */
    pcDst = pcMemHead + stLinePoint[1].s32YPos*u32ScreenStride + stLinePoint[1].s32XPos*4;
    for (j = 0; j < APP_RECODE_LINE_HEIGHT; j++)
    {
        memset(pcDst,uiColorValue,APP_RECODE_LINE_WIDTH*4);
        pcDst += u32ScreenStride;
    }

    pcDst = pcMemHead + stLinePoint[0].s32YPos*u32ScreenStride + stLinePoint[0].s32XPos*4;
    for (j = 0; j < APP_RECODE_LINE1_HEIGHT; j++)
    {
        memset(pcDst,uiColorValue,APP_RECODE_LINE1_WIDTH*4);
        pcDst += u32ScreenStride;
    }
    free(pucBuffer);
    pucBuffer = NULL;

    return HI_SUCCESS;
}


/*****************************************************************************
 函 数 名  : HDD_WrRecStat
 功能描述  : 记录录像日志信息
 输入参数  : HI_S32 iRecode
             HI_S32 iEvent
 输出参数  : 无
 返 回 值  :

 修改历史      :
  1.日    期   : 2014年7月6日
    作    者   : zhangjinlei
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 HDD_WrRecStat(HI_S32 iRecode,HI_S32 iEvent, HI_S32 s32LineNum, HI_CHAR *pcFileName)
{
    FILE *pRecLogFp = NULL;
    HI_S32 s32Ret = 0;
    HI_CHAR cTmpBuffer[64]= {0};
    FILE *recLogFp = NULL;
    HI_S32 s32CurPart = 0;
    STOR_REC_LOG_T *pstRecLog = NULL;

    if (NULL == pcFileName)
    {
        AV_PRINT("Input NULL pointer!");
        return HI_FAILURE;
    }

    sprintf(cTmpBuffer, "%s", pcFileName);
    /* 如果录像日志文件不存在 */
    if (access(cTmpBuffer ,F_OK) != 0)
    {
        AV_PRINT("create rec log file ,log=%s", cTmpBuffer);

        recLogFp = fopen(cTmpBuffer, "wb");
        if (recLogFp == NULL)
        {
            AV_PRINT("rec log file failed,log=%s", cTmpBuffer);
            return HI_FAILURE;
        }

        s32Ret=fwrite(&s32CurPart, sizeof(s32CurPart), 1, recLogFp);
        if(s32Ret < 0)
        {
            AV_PRINT("write recode log config err!");

            fclose(recLogFp);
            return HI_FAILURE;
        }

        pstRecLog = (STOR_REC_LOG_T *)malloc(sizeof(STOR_REC_LOG_T));
        if (NULL == pstRecLog)
        {
            AV_PRINT("malloc memory failed");
            fclose(recLogFp);
            return HI_FAILURE;
        }

        s32Ret=fwrite(pstRecLog, sizeof(STOR_REC_LOG_T), 1, recLogFp);
        if(s32Ret < 0)
        {
            AV_PRINT("write recode log config err!");
            fclose(recLogFp);
            free(pstRecLog);
            pstRecLog = NULL;
            return HI_FAILURE;
        }

        free(pstRecLog);
        pstRecLog = NULL;

        fflush(recLogFp);
        fclose(recLogFp);
    }

    pRecLogFp = fopen(cTmpBuffer, "r+");
    if (pRecLogFp == NULL)
    {
        AV_PRINT("open recode log file failed,log=%s", cTmpBuffer);
        return HI_FAILURE;
    }

    s32Ret = APP_WrOneMsg(pRecLogFp, iRecode, iEvent, s32LineNum);
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT( " APP_WrOneMsg err!s32Ret=%d",s32Ret);
        fflush(pRecLogFp);
        fclose(pRecLogFp);
        return HI_FAILURE;
    }

    fflush(pRecLogFp);
    fclose(pRecLogFp);

    return HI_SUCCESS;
}


HI_S32 HI3521_COMM_VDEC_Start(HI_S32 s32ChnNum, VDEC_CHN_ATTR_S *pstAttr)
{
    HI_S32  i;
    for(i=0; i<s32ChnNum; i++)
    {
        CHECK_CHN_RET(HI_MPI_VDEC_CreateChn(i, &pstAttr[i]), i, "HI_MPI_VDEC_CreateChn");
        CHECK_CHN_RET(HI_MPI_VDEC_StartRecvStream(i), i, "HI_MPI_VDEC_StartRecvStream");
    }

    return HI_SUCCESS;
}

HI_S32 HI3521_COMM_VDEC_Stop(HI_S32 s32ChnNum)
{
    CHECK_CHN_RET(HI_MPI_VDEC_StopRecvStream(s32ChnNum),s32ChnNum, "HI_MPI_VDEC_StopRecvStream");
    CHECK_CHN_RET(HI_MPI_VDEC_DestroyChn(s32ChnNum),s32ChnNum, "HI_MPI_VDEC_DestroyChn");
    return HI_SUCCESS;
}

HI_VOID	HI3521_COMM_VDEC_ChnAttr(HI_S32 s32ChnNum,
                                 VDEC_CHN_ATTR_S *pstVdecChnAttr, PAYLOAD_TYPE_E enType, SIZE_S *pstSize)
{
    HI_S32 i;

    for(i=0; i<s32ChnNum; i++)
    {
        pstVdecChnAttr[i].enType       = enType;
        pstVdecChnAttr[i].u32BufSize   = 3 * pstSize->u32Width * pstSize->u32Height;
        pstVdecChnAttr[i].u32Priority  = 5;
        pstVdecChnAttr[i].u32PicWidth  = pstSize->u32Width;
        pstVdecChnAttr[i].u32PicHeight = pstSize->u32Height;
        if (PT_H264 == enType || PT_MP4VIDEO == enType)
        {
            pstVdecChnAttr[i].stVdecVideoAttr.enMode=VIDEO_MODE_FRAME;
            pstVdecChnAttr[i].stVdecVideoAttr.u32RefFrameNum = 1;
            pstVdecChnAttr[i].stVdecVideoAttr.bTemporalMvpEnable = HI_FALSE;
        }
        else if (PT_JPEG == enType || PT_MJPEG == enType)
        {
            pstVdecChnAttr[i].stVdecJpegAttr.enMode = VIDEO_MODE_FRAME;
            pstVdecChnAttr[i].stVdecJpegAttr.enJpegFormat = JPG_COLOR_FMT_YCBCR420;
        }
        else if(PT_H265 == enType)
        {
            pstVdecChnAttr[i].stVdecVideoAttr.enMode=VIDEO_MODE_STREAM;
            pstVdecChnAttr[i].stVdecVideoAttr.u32RefFrameNum = 4;
            pstVdecChnAttr[i].stVdecVideoAttr.bTemporalMvpEnable = HI_TRUE;
        }
    }
}

HI_S32	HI3521_COMM_VDEC_InitModCommVb(VB_CONF_S *pstModVbConf)
{


    HI_MPI_VB_ExitModCommPool(VB_UID_VDEC);

    if(0 == g_s32VBSource)
    {
        CHECK_RET(HI_MPI_VB_SetModPoolConf(VB_UID_VDEC, pstModVbConf), "HI_MPI_VB_SetModPoolConf");
        CHECK_RET(HI_MPI_VB_InitModCommPool(VB_UID_VDEC), "HI_MPI_VB_InitModCommPool");
    }

    return HI_SUCCESS;
}

HI_VOID	HI3521_COMM_VDEC_ModCommPoolConf(VB_CONF_S *pstModVbConf,
        PAYLOAD_TYPE_E enType, SIZE_S *pstSize, HI_S32 s32ChnNum)
{
    HI_S32 PicSize, PmvSize;

    memset(pstModVbConf, 0, sizeof(VB_CONF_S));
    pstModVbConf->u32MaxPoolCnt = 2;

    VB_PIC_BLK_SIZE(pstSize->u32Width, pstSize->u32Height, enType, PicSize);
    pstModVbConf->astCommPool[0].u32BlkSize = PicSize;
    pstModVbConf->astCommPool[0].u32BlkCnt  = 4*s32ChnNum;

    /* NOTICE:
    1. if the VDEC channel is H264 channel and support to decode B frame, then you should allocate PmvBuffer
    2. if the VDEC channel is MPEG4 channel, then you should allocate PmvBuffer.
    */
    if(PT_H265 == enType)
    {
        VB_PMV_BLK_SIZE(pstSize->u32Width, pstSize->u32Height, enType, PmvSize);
        pstModVbConf->astCommPool[1].u32BlkSize = PmvSize;
        pstModVbConf->astCommPool[1].u32BlkCnt  = 4*s32ChnNum;
    }
}

HI_VOID	HI3521_COMM_VDEC_VpssGrpAttr(HI_S32 s32ChnNum, VPSS_GRP_ATTR_S *pstVpssGrpAttr, SIZE_S *pstSize)
{
    HI_S32 i;

    for(i=0; i<s32ChnNum; i++)
    {
        pstVpssGrpAttr->enDieMode = VPSS_DIE_MODE_NODIE;
        pstVpssGrpAttr->bIeEn     = HI_FALSE;
        pstVpssGrpAttr->bDciEn    = HI_TRUE;
        pstVpssGrpAttr->bNrEn     = HI_TRUE;
        pstVpssGrpAttr->bHistEn   = HI_FALSE;
        pstVpssGrpAttr->enPixFmt  = PIXEL_FORMAT_YUV_SEMIPLANAR_420;
        pstVpssGrpAttr->u32MaxW   = ALIGN_UP(pstSize->u32Width,  16);
        pstVpssGrpAttr->u32MaxH   = ALIGN_UP(pstSize->u32Height, 16);
    }
}

/*****************************************************************************
 函 数 名  : HI3521_StartVideoVDEC
 功能描述  : 开启视频解码
 输入参数  : void
 输出参数  : 无
 返 回 值  :

 修改历史      :
  1.日    期   : 2013年10月24日
    作    者   : zhangjinlei
    修改内容   : 新生成函数

*****************************************************************************/
int HI3521_StartVideoVDEC(HI_S32 deccmd)
{
    VB_CONF_S stModVbConf;
    VPSS_GRP VpssGrp = 12;
    HI_S32 u32HDWndNum = 4;
    SIZE_S stSize;
    int iRet = 0;
    int i = 0;
    VDEC_CHN_ATTR_S stVdecChnAttr;
    if((deccmd&0xf00)==0)
    {
        stSize.u32Width = 720;//_720P_WIDTH;
        stSize.u32Height = 576;
    }
    else
    {
        stSize.u32Width = 1280;//_720P_WIDTH;
        stSize.u32Height = 720;
    }
    /************************************************
    step2:  init mod common VB
    *************************************************/

    HI3521_COMM_VDEC_ModCommPoolConf(&stModVbConf, PT_H264, &stSize, u32HDWndNum);
    iRet = HI3521_COMM_VDEC_InitModCommVb(&stModVbConf);
    if(iRet != HI_SUCCESS)
    {
        AV_PRINT("init mod common vb fail for %#x!\n", iRet);
        return HI_FAILURE;
    }
    /************************************************
    step3:  start VDEC
    *************************************************/
    HI3521_COMM_VDEC_ChnAttr(u32HDWndNum, &stVdecChnAttr, PT_H264, &stSize);
    iRet = HI3521_COMM_VDEC_Start(u32HDWndNum, &stVdecChnAttr);
    if(iRet != HI_SUCCESS)
    {
        SAMPLE_PRT("start VDEC fail for %#x!\n", iRet);
        return HI_FAILURE;
    }

    /************************************************
    step6:  VDEC bind VPSS
    *************************************************/
    for(i=0; i<u32HDWndNum; i++)
    {
        iRet = HI3521_Comm_VDEC_BindVpss(i, VpssGrp+i);
        if(iRet != HI_SUCCESS)
        {
            AV_PRINT("vdec bind vpss fail for %#x!\n", iRet);
            return HI_FAILURE;
        }
    }
    SPLIT_MODE stSplitMode = {0};
    stSplitMode.dwMode = PREVIEW_4_CHN_SCREEN1;
    stSplitMode.dwIndex = 0;
    HI3521_Split_ViDisable();
    HI3521_VdecChnInit(&stSplitMode);
    return HI_SUCCESS;
}

HI_VOID HI3521_COMM_VDEC_ThreadParam(HI_S32 s32ChnNum, VdecThreadParam *pstVdecSend,
                                     VDEC_CHN_ATTR_S *pstVdecChnAttr, char *pStreamFileName)
{
    int i;

    for(i=0; i<s32ChnNum; i++)
    {
        sprintf(pstVdecSend[i].cFileName, pStreamFileName, i);
        pstVdecSend[i].s32MilliSec     = -1;
        pstVdecSend[i].s32ChnId        = i;
        pstVdecSend[i].s32IntervalTime = 1;
        pstVdecSend[i].u64PtsInit      = 0;
        pstVdecSend[i].u64PtsIncrease  = 0;
        pstVdecSend[i].eCtrlSinal      = VDEC_CTRL_START;
        pstVdecSend[i].bLoopSend       = HI_TRUE;
        pstVdecSend[i].bManuSend       = HI_FALSE;
        pstVdecSend[i].enType          = pstVdecChnAttr[i].enType;
        pstVdecSend[i].s32MinBufSize   = (pstVdecChnAttr[i].u32PicWidth * pstVdecChnAttr[i].u32PicHeight * 3)>>1;
        if (PT_H264 == pstVdecChnAttr[i].enType  || PT_MP4VIDEO == pstVdecChnAttr[i].enType)
        {
            pstVdecSend[i].s32StreamMode  = pstVdecChnAttr[i].stVdecVideoAttr.enMode;
        }
        else
        {
            pstVdecSend[i].s32StreamMode = VIDEO_MODE_FRAME;
        }
    }
}


HI_VOID * HI3521_COMM_VDEC_SendStream(HI_VOID *pArgs)
{
    VdecThreadParam *pstVdecThreadParam =(VdecThreadParam *)pArgs;
    FILE *fpStrm=NULL;
    HI_U8 *pu8Buf = NULL;
    VDEC_STREAM_S stStream;
    HI_BOOL bFindStart, bFindEnd;
    HI_S32 s32Ret,  i,  start = 0;
    HI_S32 s32UsedBytes = 0, s32ReadLen = 0;
    HI_U64 u64pts = 0;
    HI_S32 len;
    HI_BOOL sHasReadStream = HI_FALSE;

    if(pstVdecThreadParam->cFileName != 0)
    {
        fpStrm = fopen(pstVdecThreadParam->cFileName, "rb");
        if(fpStrm == NULL)
        {
            printf("SAMPLE_TEST:can't open file %s in send stream thread:%d\n",pstVdecThreadParam->cFileName, pstVdecThreadParam->s32ChnId);
            return (HI_VOID *)(HI_FAILURE);
        }
    }
    AV_PRINT("@%s pid is %d,tid%d", __FUNCTION__,getpid(),gettid());
    AV_PRINT("#[av]HI3521_COMM_VDEC_SendStream succ");
    //printf("SAMPLE_TEST:chn %d, stream file:%s, bufsize: %d\n",
    //pstVdecThreadParam->s32ChnId, pstVdecThreadParam->cFileName, pstVdecThreadParam->s32MinBufSize);

    pu8Buf = (HI_U8 *)malloc(pstVdecThreadParam->s32MinBufSize);
    if(pu8Buf == NULL)
    {
        printf("SAMPLE_TEST:can't alloc %d in send stream thread:%d\n", pstVdecThreadParam->s32MinBufSize, pstVdecThreadParam->s32ChnId);
        if(fpStrm != NULL)
            fclose(fpStrm);
        return (HI_VOID *)(HI_FAILURE);
    }
    fflush(stdout);

    u64pts = pstVdecThreadParam->u64PtsInit;
    while (1)
    {
        if (pstVdecThreadParam->eCtrlSinal == VDEC_CTRL_STOP)
        {
            break;
        }
        else if (pstVdecThreadParam->eCtrlSinal == VDEC_CTRL_PAUSE)
        {
            //sleep(MIN2(pstVdecThreadParam->s32IntervalTime,1000));
            TM_USleep(MIN2(pstVdecThreadParam->s32IntervalTime,1000),0);
            continue;
        }

        if ( (pstVdecThreadParam->s32StreamMode==VIDEO_MODE_FRAME) && (pstVdecThreadParam->enType == PT_MP4VIDEO) )
        {
            bFindStart = HI_FALSE;
            bFindEnd   = HI_FALSE;
            fseek(fpStrm, s32UsedBytes, SEEK_SET);
            s32ReadLen = fread(pu8Buf, 1, pstVdecThreadParam->s32MinBufSize, fpStrm);
            if (s32ReadLen == 0)
            {
                if (pstVdecThreadParam->bLoopSend)
                {
                    s32UsedBytes = 0;
                    fseek(fpStrm, 0, SEEK_SET);
                    s32ReadLen = fread(pu8Buf, 1, pstVdecThreadParam->s32MinBufSize, fpStrm);
                }
                else
                {
                    break;
                }
            }

            for (i=0; i<s32ReadLen-4; i++)
            {
                if (pu8Buf[i] == 0 && pu8Buf[i+1] == 0 && pu8Buf[i+2] == 1 && pu8Buf[i+3] == 0xB6)
                {
                    bFindStart = HI_TRUE;
                    i += 4;
                    break;
                }
            }

            for (; i<s32ReadLen-4; i++)
            {
                if (pu8Buf[i  ] == 0 && pu8Buf[i+1] == 0 && pu8Buf[i+2] == 1 && pu8Buf[i+3] == 0xB6)
                {
                    bFindEnd = HI_TRUE;
                    break;
                }
            }

            s32ReadLen = i;
            if (bFindStart == HI_FALSE)
            {
                printf("SAMPLE_TEST: chn %d can not find start code! s32ReadLen %d, s32UsedBytes %d. \n",
                       pstVdecThreadParam->s32ChnId, s32ReadLen, s32UsedBytes);
            }
            else if (bFindEnd == HI_FALSE)
            {
                s32ReadLen = i+4;
            }

        }
        else if ( (pstVdecThreadParam->s32StreamMode==VIDEO_MODE_FRAME) && (pstVdecThreadParam->enType == PT_H264) )
        {
            bFindStart = HI_FALSE;
            bFindEnd   = HI_FALSE;
            fseek(fpStrm, s32UsedBytes, SEEK_SET);
            s32ReadLen = fread(pu8Buf, 1, pstVdecThreadParam->s32MinBufSize, fpStrm);
            if (s32ReadLen == 0)
            {
                if (pstVdecThreadParam->bLoopSend)
                {
                    s32UsedBytes = 0;
                    fseek(fpStrm, 0, SEEK_SET);
                    s32ReadLen = fread(pu8Buf, 1, pstVdecThreadParam->s32MinBufSize, fpStrm);
                }
                else
                {
                    break;
                }
            }

            for (i=0; i<s32ReadLen-5; i++)
            {
                if (  pu8Buf[i  ] == 0 && pu8Buf[i+1] == 0 && pu8Buf[i+2] == 1 &&
                        ( (pu8Buf[i+3]&0x1F) == 0x5 || (pu8Buf[i+3]&0x1F) == 0x1 ) &&
                        ( (pu8Buf[i+4]&0x80) == 0x80)
                   )
                {
                    bFindStart = HI_TRUE;
                    i += 4;
                    break;
                }
            }

            for (; i<s32ReadLen-5; i++)
            {
                if (  pu8Buf[i  ] == 0 && pu8Buf[i+1] == 0 && pu8Buf[i+2] == 1 &&
                        ( ((pu8Buf[i+3]&0x1F) == 0x7) || ((pu8Buf[i+3]&0x1F) == 0x8) || ((pu8Buf[i+3]&0x1F) == 0x6)
                          || (((pu8Buf[i+3]&0x1F) == 0x5 || (pu8Buf[i+3]&0x1F) == 0x1) &&((pu8Buf[i+4]&0x80) == 0x80))
                        )
                   )
                {
                    bFindEnd = HI_TRUE;
                    break;
                }
            }

            if(i > 0) s32ReadLen = i;
            if (bFindStart == HI_FALSE)
            {
                printf("SAMPLE_TEST: chn %d can not find start code!s32ReadLen %d, s32UsedBytes %d. \n",
                       pstVdecThreadParam->s32ChnId, s32ReadLen, s32UsedBytes);
            }
            else if (bFindEnd == HI_FALSE)
            {
                s32ReadLen = i+5;
            }

        }
        else if ((pstVdecThreadParam->enType == PT_MJPEG) )
        {
            bFindStart = HI_FALSE;
            bFindEnd   = HI_FALSE;
            fseek(fpStrm, s32UsedBytes, SEEK_SET);
            s32ReadLen = fread(pu8Buf, 1, pstVdecThreadParam->s32MinBufSize, fpStrm);
            if (s32ReadLen == 0)
            {
                if (pstVdecThreadParam->bLoopSend)
                {
                    s32UsedBytes = 0;
                    fseek(fpStrm, 0, SEEK_SET);
                    s32ReadLen = fread(pu8Buf, 1, pstVdecThreadParam->s32MinBufSize, fpStrm);
                }
                else
                {
                    break;
                }
            }


            for (i=0; i<s32ReadLen-2; i++)
            {
                if (pu8Buf[i] == 0xFF && pu8Buf[i+1] == 0xD8)
                {
                    start = i;
                    bFindStart = HI_TRUE;
                    i = i + 2;
                    break;
                }
            }

            for (; i<s32ReadLen-4; i++)
            {
                if ( (pu8Buf[i] == 0xFF) && (pu8Buf[i+1]& 0xF0) == 0xE0 )
                {
                    len = (pu8Buf[i+2]<<8) + pu8Buf[i+3];
                    i += 1 + len;
                }
                else
                {
                    break;
                }
            }

            for (; i<s32ReadLen-2; i++)
            {
                if (pu8Buf[i] == 0xFF && pu8Buf[i+1] == 0xD8)
                {
                    bFindEnd = HI_TRUE;
                    break;
                }
            }
            s32ReadLen = i;
            if (bFindStart == HI_FALSE)
            {
                printf("SAMPLE_TEST: chn %d can not find start code! s32ReadLen %d, s32UsedBytes %d. \n",
                       pstVdecThreadParam->s32ChnId, s32ReadLen, s32UsedBytes);
            }
            else if (bFindEnd == HI_FALSE)
            {
                s32ReadLen = i+2;
            }
        }
        else if ((pstVdecThreadParam->enType == PT_JPEG) )
        {
            if (HI_TRUE != sHasReadStream)
            {

                bFindStart = HI_FALSE;
                bFindEnd   = HI_FALSE;

                fseek(fpStrm, s32UsedBytes, SEEK_SET);
                s32ReadLen = fread(pu8Buf, 1, pstVdecThreadParam->s32MinBufSize, fpStrm);
                if (s32ReadLen == 0)
                {
                    if (pstVdecThreadParam->bLoopSend)
                    {
                        s32UsedBytes = 0;
                        fseek(fpStrm, 0, SEEK_SET);
                        s32ReadLen = fread(pu8Buf, 1, pstVdecThreadParam->s32MinBufSize, fpStrm);
                    }
                    else
                    {
                        break;
                    }
                }


                for (i=0; i<s32ReadLen-2; i++)
                {
                    if (pu8Buf[i] == 0xFF && pu8Buf[i+1] == 0xD8)
                    {
                        start = i;
                        bFindStart = HI_TRUE;
                        i = i + 2;
                        break;
                    }
                }

                for (; i<s32ReadLen-4; i++)
                {
                    if ( (pu8Buf[i] == 0xFF) && (pu8Buf[i+1]& 0xF0) == 0xE0 )
                    {
                        len = (pu8Buf[i+2]<<8) + pu8Buf[i+3];
                        i += 1 + len;
                    }
                    else
                    {
                        break;
                    }
                }

                for (; i<s32ReadLen-2; i++)
                {
                    if (pu8Buf[i] == 0xFF && pu8Buf[i+1] == 0xD8)
                    {
                        bFindEnd = HI_TRUE;
                        break;
                    }
                }
                s32ReadLen = i;
                if (bFindStart == HI_FALSE)
                {
                    printf("SAMPLE_TEST: chn %d can not find start code! s32ReadLen %d, s32UsedBytes %d. \n",
                           pstVdecThreadParam->s32ChnId, s32ReadLen, s32UsedBytes);
                }
                else if (bFindEnd == HI_FALSE)
                {
                    s32ReadLen = i+2;
                }
                sHasReadStream = HI_TRUE;
            }
        }
        else
        {
            fseek(fpStrm, s32UsedBytes, SEEK_SET);
            s32ReadLen = fread(pu8Buf, 1, pstVdecThreadParam->s32MinBufSize, fpStrm);
            if (s32ReadLen == 0)
            {
                if (pstVdecThreadParam->bLoopSend)
                {
                    s32UsedBytes = 0;
                    fseek(fpStrm, 0, SEEK_SET);
                    s32ReadLen = fread(pu8Buf, 1, pstVdecThreadParam->s32MinBufSize, fpStrm);
                }
                else
                {
                    break;
                }
            }
        }

        stStream.u64PTS  = u64pts;
        stStream.pu8Addr = pu8Buf + start;
        stStream.u32Len  = s32ReadLen;
        stStream.bEndOfFrame  = (pstVdecThreadParam->s32StreamMode==VIDEO_MODE_FRAME)? HI_TRUE: HI_FALSE;
        stStream.bEndOfStream = HI_FALSE;


        //printf("Send One Frame");
        //fflush(stdout);

        s32Ret=HI_MPI_VDEC_SendStream(pstVdecThreadParam->s32ChnId, &stStream, pstVdecThreadParam->s32MilliSec);
        pstVdecThreadParam->cUserCmd = 0;
        if (HI_SUCCESS != s32Ret)
        {
            //usleep(100);
            TM_USleep(0,100);
        }
        else
        {
            s32UsedBytes = s32UsedBytes +s32ReadLen + start;
            u64pts += pstVdecThreadParam->u64PtsIncrease;
        }
        TM_USleep(0,1000);
        //usleep(1000);
    }

    /* send the flag of stream end */
    memset(&stStream, 0, sizeof(VDEC_STREAM_S) );
    stStream.bEndOfStream = HI_TRUE;
    HI_MPI_VDEC_SendStream(pstVdecThreadParam->s32ChnId, &stStream, -1);

    //printf("SAMPLE_TEST:send steam thread %d return ...\n", pstVdecThreadParam->s32ChnId);
    fflush(stdout);
    if (pu8Buf != HI_NULL)
    {
        free(pu8Buf);
    }
    fclose(fpStrm);

    return (HI_VOID *)HI_SUCCESS;
}
HI_VOID HI3521_COMM_VDEC_StartSendStream(HI_S32 s32ChnNum, VdecThreadParam *pstVdecSend, pthread_t *pVdecThread)
{
    HI_S32  i;

    for(i=0; i<s32ChnNum; i++)
    {
        pthread_create(&pVdecThread[i], 0, HI3521_COMM_VDEC_SendStream, (HI_VOID *)&pstVdecSend[i]);
    }
}

SF_LIST_ST *HI3521_GetLocalSFList(HI_S32 s32Chn)
{
    return &g_stLocalSFList[s32Chn];
}



HI_S32 HI3531_GetCurrIFrameSeek(FILE_IFRAME_LIST_T *pstIFrameList, HI_U32 *pU32FrameSeek, time_t tFrameStartTime)
{
    HI_U32 u32IFrameTime = 0;
    HI_S32 s32IFrameCnt = 0;
    HI_S32 s32Ret = HI_FAILURE;
    HI_S32 s32FrameCnt = 0;
    STOR_REC_IFRAME_INFO *pstIFRAMEInfo = NULL;

    if ((NULL == pstIFrameList) || (NULL == pU32FrameSeek))
    {
        AV_PRINT("Input para invalid.");
        return HI_FAILURE;
    }

    s32FrameCnt = pstIFrameList->s32Counter;

    //AV_PRINT("s32FrameCnt(%d)", s32FrameCnt);
    if (s32FrameCnt > STOR_FILE_IFRAME_MAX_NUMS)
    {
        AV_PRINT("s32FrameCnt(%d), idxNew(%d), idxOld(%d)",
                 s32FrameCnt, pstIFrameList->s32NewestIndex, pstIFrameList->s32OldestIndex);
        return HI_FAILURE;
    }


    for (s32IFrameCnt = pstIFrameList->s32OldestIndex; s32IFrameCnt < s32FrameCnt; s32IFrameCnt++)
    {
        pstIFRAMEInfo = &(pstIFrameList->stRecIFrameInfo[s32IFrameCnt]);
        u32IFrameTime = STOR_CalTime1(pstIFRAMEInfo);
        //	AV_PRINT("s32IFrameCnt=%d, u32IFrameTime=%d, tFrameStartTime=%d", s32IFrameCnt, u32IFrameTime, (HI_U32)tFrameStartTime);
        if ((u32IFrameTime >= (HI_U32)tFrameStartTime) && (u32IFrameTime < (HI_U32)(tFrameStartTime + 3600)))
        {
            *pU32FrameSeek = pstIFRAMEInfo->frame_data_pos;
            //AV_PRINT("*pU32FrameSeek=%d, u32IFrameTime=%d, tFrameStartTime=%d", *pU32FrameSeek, u32IFrameTime, (HI_U32)tFrameStartTime);
            return HI_SUCCESS;
        }
        else
        {
            //		AV_PRINT("u32IFrameTime(%d), tFrameStartTime(%d)", u32IFrameTime, (HI_U32)tFrameStartTime);
        }
    }

    return s32Ret;
}


HI_U32 HI3521_GetIFrameTime(FILE* pFile, time_t tFrameStartTime)
{
    HI_U32 u32IFrameTime = 0;
    HI_S32 s32IFrameCnt = 0;
    HI_U32 s32Ret = HI_FAILURE;
    HI_S32 s32FrameCnt = 0;
    FILE_IFRAME_LIST_T *pstIFrameList = NULL;
    STOR_REC_IFRAME_INFO *pstIFRAMEInfo = NULL;

    if (NULL == pFile)
    {
        AV_PRINT("Input para invalid.");
        goto ERR_LABLE;
    }

    pstIFrameList = (FILE_IFRAME_LIST_T *)malloc(sizeof(FILE_IFRAME_LIST_T));
    if (NULL == pstIFrameList)
    {
        AV_PRINT("malloc err(%s)", ERR_STR);
        goto ERR_LABLE;
    }

    s32Ret = STOR_GetFileIFrameList(pFile, pstIFrameList);
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT("STOR_GetFileIFrameList failed.");
        goto ERR_LABLE;
    }

    s32FrameCnt = pstIFrameList->s32Counter;

    AV_PRINT("s32FrameCnt(%d)", s32FrameCnt);
    if (s32FrameCnt > STOR_FILE_IFRAME_MAX_NUMS)
    {
        AV_PRINT("s32FrameCnt(%d), idxNew(%d), idxOld(%d)",
                 s32FrameCnt, pstIFrameList->s32NewestIndex, pstIFrameList->s32OldestIndex);
        goto ERR_LABLE;
    }


    for (s32IFrameCnt = pstIFrameList->s32OldestIndex; s32IFrameCnt < s32FrameCnt; s32IFrameCnt++)
    {
        pstIFRAMEInfo = &(pstIFrameList->stRecIFrameInfo[s32IFrameCnt]);
        u32IFrameTime = STOR_CalTime1(pstIFRAMEInfo);
        //	AV_PRINT("s32IFrameCnt=%d, u32IFrameTime=%d, tFrameStartTime=%d", s32IFrameCnt, u32IFrameTime, (HI_U32)tFrameStartTime);
        if ((u32IFrameTime >= (HI_U32)tFrameStartTime) && (u32IFrameTime < (HI_U32)(tFrameStartTime + 3600)))
        {
            AV_PRINT("u32IFrameTime=%d, tFrameStartTime=%d", u32IFrameTime, (HI_U32)tFrameStartTime);
            s32Ret = u32IFrameTime;
            goto ERR_LABLE;
        }
        else
        {
            //		AV_PRINT("u32IFrameTime(%d), tFrameStartTime(%d)", u32IFrameTime, (HI_U32)tFrameStartTime);
        }
    }

ERR_LABLE:
    if (NULL != pstIFrameList)
    {
        free(pstIFrameList);
        pstIFrameList = NULL;
    }
    return s32Ret;
}

HI_VOID HI3521_ConvertTime2LocaltimeStr(time_t t, HI_CHAR *pTime)
{
    struct tm tmm;
    if (NULL == pTime)
    {
        AV_PRINT("Input NULL pointer");
        return;
    }
    memset(&tmm, 0, sizeof(struct tm));
    localtime_r(&t, &tmm);
    sprintf(pTime, "%04d-%02d-%02d %02d:%02d:%02d",
            tmm.tm_year+1900, tmm.tm_mon+1, tmm.tm_mday, tmm.tm_hour, tmm.tm_min, tmm.tm_sec);
}
HI_VOID HI3521_ConvertTime2LocaltimeStr2(HI_CHAR *pIdx, time_t oldTime, time_t newTime)
{
    HI_CHAR timeStr1[128] = {0};
    HI_CHAR timeStr2[128] = {0};
    struct tm tmm1;
    struct tm tmm2;

    memset(&tmm1, 0, sizeof(struct tm));
    memset(&tmm2, 0, sizeof(struct tm));
    localtime_r(&oldTime, &tmm1);
    localtime_r(&newTime, &tmm2);

    snprintf(timeStr1, sizeof(timeStr1), "%04d-%02d-%02d %02d:%02d:%02d",
             tmm1.tm_year+1900, tmm1.tm_mon+1, tmm1.tm_mday, tmm1.tm_hour, tmm1.tm_min, tmm1.tm_sec);
    snprintf(timeStr2, sizeof(timeStr2), "%04d-%02d-%02d %02d:%02d:%02d",
             tmm2.tm_year+1900, tmm2.tm_mon+1, tmm2.tm_mday, tmm2.tm_hour, tmm2.tm_min, tmm2.tm_sec);

    AV_PRINT("index(%s) oldTimeVal(%d) oldTimeStr(%s) newTimeVal(%d) newTimeStr(%s)",
             pIdx, (HI_S32)oldTime, timeStr1, (HI_S32)newTime, timeStr2);
    return;
}


HI_S32 HI3521_InitPBMutex(SF_PB_ST *pstSFPB)
{
    pthread_cond_init(&pstSFPB->cond, NULL);
    pthread_mutex_init(&pstSFPB->mutex, NULL);
    pstSFPB->bWaitSignal = HI_FALSE;
    pstSFPB->bSendSignal = HI_FALSE;
    pstSFPB->bActive = HI_FALSE;
    //AV_PRINT("tid(%d) event HI3531_InitPBMutex chn(%d) !", gettid(), pstSFPB->stPB.s32Cmd);
    return HI_SUCCESS;
}
HI_S32 HI3521_ExitPBMutex(SF_PB_ST *pstSFPB)
{
    pthread_cond_destroy(&pstSFPB->cond);
    pthread_mutex_destroy(&pstSFPB->mutex);
    //AV_PRINT("tid(%d) event HI3531_ExitPBMutex chn(%d) !", gettid(), pstSFPB->stPB.s32Cmd);
    return HI_SUCCESS;
}

HI_S32 HI3521_WaitPBCond(SF_PB_ST *pstSFPB)
{
    pthread_mutex_lock(&pstSFPB->mutex);
    pstSFPB->bWaitSignal = HI_FALSE;
    pthread_cond_wait(&pstSFPB->cond, &pstSFPB->mutex);
    pstSFPB->bWaitSignal = HI_TRUE;
    pthread_mutex_unlock(&pstSFPB->mutex);
    while (!pstSFPB->bSendSignal)
    {
        //usleep(100000);/*100MS*/
        TM_USleep(0,100000);
    }

    //AV_PRINT("tid(%d) event HI3531_WaitPBCond chn(%d) !", gettid(), pstSFPB->stPB.s32Cmd);
    return HI_SUCCESS;
}
HI_S32 HI3521_SignalPBCond(SF_PB_ST *pstSFPB)
{
    pstSFPB->bSendSignal = HI_FALSE;
    while (!pstSFPB->bWaitSignal)
    {
        /*signal discarded, will try again.*/
        pthread_mutex_lock(&pstSFPB->mutex);
        pthread_cond_signal(&pstSFPB->cond);
        pthread_mutex_unlock(&pstSFPB->mutex);
        //usleep(100000);/*100MS*/
        TM_USleep(0,100000);
    }
    AV_PRINT("tid(%d) event HI3531_SignalPBCond chn(%d)!", gettid(), pstSFPB->stPB.s32Cmd);
    pstSFPB->bSendSignal = HI_TRUE;

    return HI_SUCCESS;
}
HI_VOID HI3521_WaitTimePBCond(SF_PB_ST *pstSFPB)
{
    /*this func will be used for waiting a signal within a time slag.*/
    struct timeval now;
    struct timespec timeout;
    HI_S32 s32Ret;

    pthread_mutex_lock(&pstSFPB->mutex);
    gettimeofday(&now, NULL);
    timeout.tv_sec = now.tv_sec + 5;
    timeout.tv_nsec = now.tv_usec * 1000;
    s32Ret = 0;
    while (s32Ret != ETIMEDOUT)
    {
        s32Ret = pthread_cond_timedwait(&pstSFPB->cond, &pstSFPB->mutex, &timeout);
    }
    if (s32Ret == ETIMEDOUT)
    {
        /* timeout occurred */
    }

    pthread_mutex_unlock(&pstSFPB->mutex);
}



FILE *HI3521_OpenSF(SF_NAME_ST *pstSFName)
{
    FILE *pFile =NULL;
    HI_CHAR szFileName[FILE_NAME_LEN] = {0};

    memcpy(szFileName, pstSFName->szFileName, 32);

    if (access(szFileName, F_OK) != 0)
    {
        AV_PRINT("rec(%s) not exist", szFileName);
        return NULL;
    }

    pFile = fopen(szFileName, "r+");
    if (NULL == pFile)
    {
        return NULL;
    }

    return pFile;
}

HI_VOID HI3521_Setdecchn( HI_U8 chn)
{
    decchn = chn;
}
HI_S32 HI3521_GetPlayCtrlChn(HI_S32 chn)
{
    return g_stPbctrl[chn].lpbctrlstat;
}
HI_S32 HI3521_GetdecPause(HI_S32 chn)
{
    AV_PRINT("bPause :%d chn :%d ", g_stPbctrl[chn].bPause,chn);
    return g_stPbctrl[chn].bPause;
}
HI_S32 HI3521_GetdecSpeedUP(HI_S32 chn)
{
    return g_stPbctrl[chn].bSpeedUp;
}

HI_S32 HI3521_GetdecSpeeddown(HI_S32 chn)
{
    return g_stPbctrl[chn].bSpeedDown;
}
HI_BOOL HI3521_Getalldecexit()
{
    for(int i = 0; i < MAX_PLAY_BACK_CHN; i++)
    {
        if(g_stPbctrl[i].decexit == HI_TRUE)
            return  HI_TRUE;
    }
    return HI_FALSE;
}
HI_VOID HI3521_SetPlayCtrl(HI_S32 s32playctrl,HI_S32 chn)
{
    pthread_mutex_lock(&g_stPbctrl[chn].mutex);
    g_stPbctrl[chn].lpbctrlstat = s32playctrl;
    pthread_mutex_unlock(&g_stPbctrl[chn].mutex);
}
HI_VOID HI3521_SetAllPlayCtrl(HI_S32 s32playctrl)
{
    for(int i = 0; i < MAX_PLAY_BACK_CHN; i++)
    {
        HI3521_SetPlayCtrl(s32playctrl,i);
    }
}
HI_VOID HI3521_Setdecexit( HI_S32 chn)
{
    pthread_mutex_lock(&g_stPbctrl[chn].mutex);
    g_stPbctrl[chn].decexit = HI_TRUE;
    pthread_mutex_unlock(&g_stPbctrl[chn].mutex);
}
HI_VOID HI3521_Cleardecexit( HI_S32 chn)
{
    pthread_mutex_lock(&g_stPbctrl[chn].mutex);
    g_stPbctrl[chn].decexit = HI_FALSE;
    pthread_mutex_unlock(&g_stPbctrl[chn].mutex);
}

HI_VOID HI3521_SetdecSpeedUp(HI_S32 chn,HI_BOOL speed)
{
    pthread_mutex_lock(&g_stPbctrl[chn].mutex);
    g_stPbctrl[chn].bSpeedUp= speed;
    pthread_mutex_unlock(&g_stPbctrl[chn].mutex);
}
HI_VOID HI3521_SetdecSpeedDown(HI_S32 chn,HI_BOOL speed)
{
    pthread_mutex_lock(&g_stPbctrl[chn].mutex);
    g_stPbctrl[chn].bSpeedDown= speed;
    pthread_mutex_unlock(&g_stPbctrl[chn].mutex);
}
HI_VOID HI3521_SetdecPause(HI_S32 chn,HI_BOOL pause)
{
    pthread_mutex_lock(&g_stPbctrl[chn].mutex);
    g_stPbctrl[chn].bPause= pause;
    pthread_mutex_unlock(&g_stPbctrl[chn].mutex);
}
HI_VOID HI3521_InitPBCtrl()
{
    SF_PB_CTRL *pb_ctrl = NULL;
    for(int i  = 0; i < MAX_PLAY_BACK_CHN ; i++)
    {
        pb_ctrl  = &g_stPbctrl[i];
        pthread_mutex_init(&pb_ctrl->mutex, NULL);
        pb_ctrl->bPause = HI_TRUE;
        pb_ctrl->bSpeedUp= HI_FALSE;
        pb_ctrl->bSpeedDown= HI_FALSE;
        pb_ctrl->decexit= HI_FALSE;
        pb_ctrl->lpbctrlstat= -1;
    }
}
HI_VOID HI3521_deInitPBCtrl()
{
    for(int i  = 0; i < MAX_PLAY_BACK_CHN ; i++)
    {
        pthread_mutex_destroy(&g_stPbctrl[i].mutex);
        g_stPbctrl[i].bPause = HI_FALSE;
        g_stPbctrl[i].bSpeedUp= HI_FALSE;
        g_stPbctrl[i].bSpeedDown= HI_FALSE;
        g_stPbctrl[i].decexit= HI_FALSE;
        g_stPbctrl[i].lpbctrlstat= -1;
    }
}

/*****************************************************************************
 函 数 名  : HI3531_SndVdecData_V2
 功能描述  : 倒放函数
 输入参数  : HI_S32 currFileno
             HI_U32 *pcurrTime
             REC_PLAYBACK_T *pTmpPB
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年6月6日
    作    者   : zoulq
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 HI3531_SndVdecData_V2(HI_S32 currFileno, HI_U32 *pcurrTime, REC_PLAYBACK_T *pTmpPB)
{
    FILE *pFile = NULL;
    HI_S32 s32Ret = HI_FAILURE;
    HI_U32 u32FrameSeek = 0;
    HI_U32 u32FrameEnd = 0;
    HI_S32 s32Chn = 0,logicchn = 0;
    SF_NAME_ST stSFName;
    SF_LIST_ST *pstSFList = NULL;
    STOR_FILE_BLK_HEAD_T stFileBlkHead;
    HI_S32 s32RdLen = 0;
    HI_S32 s32ExitType = 0;	/*0正常解码结束退出，1外部事件导致中途退出*/
    HI_S32 s32PlayCtrlStat = -1;
    HI_BOOL bPausePlay = HI_FALSE;  /*播放暂停标志 */
    HI_S32 s32PauseTimes = 0;  /*暂停次数 */
    FRAME_HEAD_T *pstFrameHead = NULL;
    VDEC_STREAM_S stVideoStream;
//	HI_U32 u32CurFrameTime = 0;
    HI_U8 u8VdecBuf[307200] = {0};	/*300KB*/

    s32Chn = pTmpPB->s32Cmd;
    logicchn = s32Chn%4;
    pstSFList = HI3521_GetLocalSFList(s32Chn);
    AV_PRINT(" HI3531_VdecbyChnThd, chn(%d),s32FileSegNum(%d)",  s32Chn,pstSFList->s32FileSegNum);
    /*一段录像可能有N个时间段组成*/
    while ( currFileno>=0 )
    {
        memset(&stSFName, 0, sizeof(stSFName));
        memcpy(&stSFName.szFileName, pstSFList->stFileName[currFileno].szFileName, FILE_NAME_LEN);

        AV_PRINT("s32Chn(%d),start HI3531_OpenSF(%s) ",s32Chn,stSFName.szFileName);
        /*打开文件*/
        pFile = HI3521_OpenSF(&stSFName);
        if (NULL == pFile)
        {
            currFileno--;
            continue;
        }

        AV_PRINT("s32Chn(%d), HI3521_OpenSF(%s) success",s32Chn,stSFName.szFileName);

        *pcurrTime = MIN(*pcurrTime, (HI_U32)pstSFList->stFileTimeSeg[currFileno].tEndTime);

        /*获取开始倒放的第一个时间SEEK */
        s32Ret = STOR_GetIFrameSeek(pFile, &u32FrameSeek, *pcurrTime);
        if (HI_SUCCESS != s32Ret)
        {
            s32Ret = STOR_GetIFrameSeek(pFile, &u32FrameSeek, *pcurrTime-1);
            if (HI_SUCCESS != s32Ret)
            {
                AV_PRINT("STOR_GetIFrameSeek err");
                if(NULL != pFile)
                {
                    fclose(pFile);
                    pFile = NULL;
                }
                return 0;//相当于正常播放完毕的返回
            }
        }

        /*获取帧结束位置*/
        s32Ret = STOR_GetVideoFileHeadInfo(pFile, &stFileBlkHead, PART_INDEX_SEEK_HEAD);
        if (HI_SUCCESS != s32Ret)
        {
            if(NULL != pFile)
            {
                fclose(pFile);
                pFile = NULL;
            }
            continue;
        }

        /*文件的I帧起始位置 */
        u32FrameEnd = stFileBlkHead.frame_data_start_pos;

        /*读取数据发送 */
        while (HDD_IsLPBRunning())
        {
            AV_PRINT("s32Chn(%d),u32FrameSeek(%u),u32FrameEnd(%u)", s32Chn, u32FrameSeek, u32FrameEnd);
            /*是否结束*/
            if (u32FrameSeek <= u32FrameEnd)
            {
                AV_PRINT("chn(%d) frame play end.", s32Chn);
                if(NULL != pFile)
                {
                    fclose(pFile);
                    pFile = NULL;
                }
            }

            /*开锁、关录像、格式化时，停止解码*/
            if (DISK_STAT_WORKING != HDD_GetDiskFSM())
            {
                break;
            }

            /*回放控制 */
            s32PlayCtrlStat = HI3521_GetPlayCtrlChn(logicchn);
            switch (s32PlayCtrlStat)
            {
            case LPB_CTRL_SPEEDDOWN:
                /*倒放中收到倒放不处理 */
                AV_PRINT("LPB_CTRL_SPEEDDOWN chn: %d",logicchn);
                HI3521_SetPlayCtrl(-1,logicchn);
                break;
            case LPB_CTRL_PAUSE:  /*暂停 ,播放*/
                AV_PRINT("LPB_CTRL_PAUSE chn: %d",logicchn);
                if (HI3521_GetdecPause(logicchn))
                {
                    HI3521_SetdecPause(logicchn,HI_FALSE);
                    AV_PRINT("repaly again");
                    //                     HI3531_SwitchVdecPreview(0,0,0,0);
                }
                else
                {
                    if (s32PauseTimes == 0)
                    {
                        bPausePlay = HI_TRUE;
                        s32PauseTimes++;
                        AV_PRINT("stop play");
                    }
                    else
                    {
                        bPausePlay = HI_FALSE;
                        s32PauseTimes = 0;
                        AV_PRINT("start play");
                    }
                }
                HI3521_SetPlayCtrl(-1,logicchn);
                break;
            case LPB_CTRL_SPEEDUP: /*加速则退出回归正常处理 */
                AV_PRINT("LPB_CTRL_SPEEDUP chn: %d",logicchn);
                if(bPausePlay == HI_TRUE )
                {
                    AV_PRINT("downing stop --->up");
                    HI3521_SetPlayCtrl(LPB_CTRL_PAUSE,logicchn);
                }
                else
                    HI3521_SetPlayCtrl(-1,logicchn);
                s32ExitType = 1;
                if(NULL != pFile)
                {
                    fclose(pFile);
                    pFile = NULL;
                }
                AV_PRINT("SPEED NORMAL NOW");
                HI3521_SetdecSpeedDown(logicchn,HI_FALSE);
                return s32ExitType;
            case LPB_CTRL_STOP:  /*停止就是退出 */
                HI3521_SetdecPause(logicchn,HI_TRUE);
                s32ExitType = 2;
                HI3521_SetPlayCtrl(-1,logicchn);
//                    SAMPLE_VDEC_WaitDestroyVdecChn(0, VIDEO_MODE_FRAME, HI_FALSE);
//                   HI3531_SwitchVdecPreview(0,0,0,1);
                return s32ExitType;
            case LPB_CTRL_RETURN:
                HI3521_SetPlayCtrl(-1,logicchn);
                break;
            default:
                break;

            }

            /*暂停不放 */
            if (!bPausePlay)
            {
                /*获取当前帧头*/
                memset(u8VdecBuf, 0, sizeof(u8VdecBuf));
                pstFrameHead = (FRAME_HEAD_T *)u8VdecBuf;
                s32Ret = STOR_GetIFrameSeek(pFile, &u32FrameSeek, *pcurrTime);
                if (HI_SUCCESS != s32Ret)
                {
                    s32ExitType = 0;
                    AV_PRINT("STOR_GetIFrameSeek err");
                    break;
                }

                fseek(pFile, u32FrameSeek, SEEK_SET);
                s32RdLen = fread(pstFrameHead, 1, sizeof(FRAME_HEAD_T), pFile);
                if (sizeof(FRAME_HEAD_T) != s32RdLen)
                {
                    AV_PRINT("fread err(%s)", ERR_STR);
                    s32ExitType = 0;
                    break;
                }

                /*录像有问题，长度过大 */
                if (pstFrameHead->len > 1024*1024)
                {
                    //   u32CurFrameTime = STOR_CalTime2(pstFrameHead);
                    s32Ret = STOR_GetIFrameSeek(pFile, &u32FrameSeek, *pcurrTime-1);
                    if (HI_SUCCESS != s32Ret)
                    {
                        s32ExitType = 0;
                        AV_PRINT("STOR_GetIFrameSeek err");
                        break;
                    }
                }

                //AV_PRINT("read frame len is (%d)", pstFrameHead->len);

                /*读取当前帧数据*/
                s32RdLen = fread(u8VdecBuf + sizeof(FRAME_HEAD_T), 1, pstFrameHead->len, pFile);
                if ((s32RdLen < 0) || (pstFrameHead->len != (HI_U32)s32RdLen))
                {
                    AV_PRINT("fread err(%s)", ERR_STR);
                    continue;
                }

                /*只丢I帧 */
                if (DATA_VIDEO_IFRAME == pstFrameHead->type)
                {
                    stVideoStream.pu8Addr = u8VdecBuf + sizeof(FRAME_HEAD_T);
                    stVideoStream.u32Len = pstFrameHead->len;
                    stVideoStream.u64PTS += 40000;
                    stVideoStream.bEndOfStream = HI_FALSE;
                    stVideoStream.bEndOfFrame = HI_TRUE;
                    stVideoStream.u64PTS = 0;
                    s32Ret = HI_MPI_VDEC_SendStream(logicchn, &stVideoStream, -1);
                    if (HI_SUCCESS != s32Ret)
                    {
                        AV_PRINT("Send to vdec error 0x%x", s32Ret);
                        s32Ret = STOR_GetIFrameSeek(pFile, &u32FrameSeek, *pcurrTime-1);
                        if (HI_SUCCESS != s32Ret)
                        {
                            AV_PRINT("STOR_GetIFrameSeek err");
                        }

                        continue;
                    }
                }

                *pcurrTime = *pcurrTime -1;
                //AV_PRINT("TIME IS %d --%d--%d", *pcurrTime, pstSFList->stFileTimeSeg[currFileno].tStartTime, pTmpPB->tStartTime);
                if (*pcurrTime <= (HI_U32)MAX(pstSFList->stFileTimeSeg[currFileno].tStartTime, pTmpPB->tStartTime))
                {
                    //       memset(&stVideoStream, 0, sizeof(VDEC_STREAM_S) );
                    //       stVideoStream.bEndOfStream = HI_TRUE;
                    //        HI_MPI_VDEC_SendStream(logicchn, &stVideoStream, -1);
                    if(NULL != pFile)
                    {
                        fclose(pFile);
                        pFile = NULL;
                    }
                    AV_PRINT("back play over :%d",logicchn);
                    break;
                }

                //usleep(250000);
                TM_USleep(0,250000);
            }
            else
            {
                //usleep(300000);
                TM_USleep(0,300000);
            }
            //usleep(35000);
        }

        currFileno--;
    }

    /*正常倒放结束 */
    s32ExitType = 0;
    if (!HDD_IsLPBRunning())
    {
        s32ExitType = 2;
        if(NULL != pFile)
        {
            fclose(pFile);
            pFile = NULL;
        }
    }
    return s32ExitType;
}

HI_VOID HI3521_PrintFrameType(HI_S32 s32Chn, DATA_TYPE_E eDataType)
{
    if (DATA_VIDEO_IFRAME == eDataType)
    {
        AV_PRINT("chn(%d) FrameType(I)", s32Chn);
    }
    else if (DATA_VIDEO_PFRAME == eDataType)
    {
        AV_PRINT("chn(%d) FrameType(P)", s32Chn);
    }
    else if (DATA_AUDIO == eDataType)
    {
        AV_PRINT("chn(%d) FrameType(Audio)", s32Chn);
    }

    return;
}

/*****************************************************************************
 函 数 名  : HI3521_SndFrameData2Vdec
 功能描述  : 发送帧数据到解码库
 输入参数  : FILE *pFile
             HI_S32 s32Chn
             HI_U32 u32FrameSeek
             HI_U32 u32FrameEnd
             REC_PLAYBACK_T *pstPB
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年6月6日
    作    者   : zoulq
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 HI3521_SndFrameData2Vdec(FILE *pFile, HI_S32 s32Chn, HI_S32 fileseg,
                                HI_U32 u32FrameSeek, HI_U32 u32FrameEnd, REC_PLAYBACK_T *pstPB, REC_PLAYBACK_T *pstTmpPB)
{
    HI_S32 s32Ret = HI_FAILURE;
    HI_S32 s32RdLen = 0;
    HI_S32 s32ExitType = 0,logcalchn = 0;
    HI_S32 s32PlayCtrlStat = -1;
    HI_BOOL bPausePlay = HI_FALSE;  /*播放暂停标志 */
    HI_S32 s32PauseTimes = 0;  /*暂停次数 */
    FRAME_HEAD_T *pstFrameHead = NULL;
    VDEC_STREAM_S stVideoStream;
    AUDIO_STREAM_S stAudioStream;
    HI_U32 u32CurFrameTime = 0;
    HI_U32 u32LastFrameTime = 0;
    HI_U8 u8VdecBuf[307200] = {0};	/*300KB*/
    time_t tPBStartTime = 0;
    REC_PLAYBACK_T tmpstPB;
    FILE_IFRAME_LIST_T *pstIFrameList = NULL;

    if ((NULL == pFile) || (NULL == pstPB))
    {
        AV_PRINT("HI3531_SndFrameData2Vdec check para err");
        return HI_FAILURE;
    }

    logcalchn =  s32Chn%4;
    memcpy(&tmpstPB, pstTmpPB, sizeof(REC_PLAYBACK_T));  /*PB做一个备份用于倒放 */
    memset(&stVideoStream, 0, sizeof(VDEC_STREAM_S));
    memset(&stAudioStream, 0, sizeof(AUDIO_STREAM_S));

    /*获取I帧数据列表 */
    pstIFrameList = (FILE_IFRAME_LIST_T *)malloc(sizeof(FILE_IFRAME_LIST_T));
    if (NULL == pstIFrameList)
    {
        AV_PRINT("malloc err(%s)", ERR_STR);
        return HI_FAILURE;
    }

    s32Ret = STOR_GetFileIFrameList(pFile, pstIFrameList);
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT("STOR_GetFileIFrameList failed.");
        free(pstIFrameList);
        pstIFrameList = NULL;
        return HI_FAILURE;
    }

    tPBStartTime = HI3521_GetIFrameTime(pFile, pstPB->tStartTime);

    HI3521_ConvertTime2LocaltimeStr2((HI_CHAR *)"seek time and PB time ", pstPB->tStartTime, tPBStartTime);

    /*开始进行回放 */
    while (HDD_IsLPBRunning())
    {
        /*是否结束*/
        if (u32FrameSeek >= u32FrameEnd)
        {
            AV_PRINT("chn(%d) frame play end.", s32Chn);
            s32ExitType = 0;
            break;
        }

        /*开锁、关录像、格式化时，停止解码*/
        if (DISK_STAT_WORKING != HDD_GetDiskFSM())
        {
            s32ExitType = 1;
            break;
        }

        /*回放控制 */
        s32PlayCtrlStat = HI3521_GetPlayCtrlChn(logcalchn);
        switch (s32PlayCtrlStat)
        {
        case LPB_CTRL_SPEEDDOWN:
            AV_PRINT("LPB_CTRL_SPEEDDOWN chn: %d",logcalchn);
            //       if(!bPausePlay)
            //       {
            /*暂停下加速减速按键无线 */
            if (HI3521_GetdecSpeedUP(logcalchn))
            {
                HI3521_SetdecSpeedUp(logcalchn,HI_FALSE);
            }
            else
            {
                /*减速 */
                HI3521_SetdecSpeedDown(logcalchn, HI_TRUE);
            }
            //        }
            HI3521_SetPlayCtrl(-1,logcalchn);
            break;
        case LPB_CTRL_PAUSE:  /*暂停 ,播放*/
            AV_PRINT("LPB_CTRL_PAUSE chn: %d",logcalchn);
            if (HI3521_GetdecPause(logcalchn))
            {
                /*如果之前停止了播放 */
                HI3521_SetdecPause(logcalchn,HI_FALSE);
                AV_PRINT("repaly again");
                //                  HI3531_SwitchVdecPreview(0,0,0,0);
            }
            else
            {
                /*正常播放 */
                if (s32PauseTimes == 0)
                {
                    bPausePlay = HI_TRUE;
                    s32PauseTimes++;
                    AV_PRINT("pause play");
                }
                else
                {
                    bPausePlay = HI_FALSE;
                    s32PauseTimes = 0;
                    AV_PRINT("go play");
                }
            }
            HI3521_SetPlayCtrl(-1,logcalchn);
            break;
        case LPB_CTRL_SPEEDUP: /*加倍数，只播放I帧 */
            AV_PRINT("LPB_CTRL_SPEEDUP chn: %d",logcalchn);
            //         if(!bPausePlay)
            //         {
            HI3521_SetdecSpeedUp(logcalchn,HI_TRUE);
            //         }
            HI3521_SetPlayCtrl(-1,logcalchn);
            break;
        case LPB_CTRL_STOP:  /*停止就是退出 */
            s32ExitType = 2;
            HI3521_SetPlayCtrl(-1,logcalchn);;
//               SAMPLE_VDEC_WaitDestroyVdecChn(0, VIDEO_MODE_FRAME, HI_FALSE);
//               HI3531_SwitchVdecPreview(0,0,0,1);
            return s32ExitType;
        case LPB_CTRL_RETURN:
            HI3521_SetPlayCtrl(-1,logcalchn);
            break;
        default:
            break;

        }


        if (HI3521_GetdecSpeeddown(logcalchn))
        {
            if(bPausePlay)
            {
                //usleep(10000);
                TM_USleep(0,10000);
                continue;
            }
            /*如果减速播放 */
            s32Ret = HI3531_SndVdecData_V2(fileseg, &u32CurFrameTime, &tmpstPB);
            if (s32Ret == 2 || s32Ret == 0)
            {
                /*退出 */
                if(s32Ret == 2)
                    s32Ret = 1;
                s32ExitType = s32Ret;
                return s32ExitType;
            }

            /*返回1 则是加速回放退出，这里要重新定位u32FrameSeek */
            s32Ret = HI3531_GetCurrIFrameSeek(pstIFrameList, &u32FrameSeek, u32CurFrameTime);
            if (HI_SUCCESS != s32Ret)
            {
                s32ExitType = 0;
                AV_PRINT("STOR_GetIFrameSeek err");
                break;
            }
			//BEGIN: Added by wangxw, 2018/4/18 DES:
			//修复单击快退播放后,第二次单击快进播放时间点不对问题
			pstPB->tStartTime=(time_t)u32CurFrameTime;
			u32LastFrameTime=(time_t)u32CurFrameTime;
        }
        else
        {
            /*暂停不放 */
            if (!bPausePlay)
            {
                /*加速根据时间定位I帧*/
                if(HI3521_GetdecSpeedUP(logcalchn))
                {
                    /*获取当前帧头*/
                    memset(u8VdecBuf, 0, sizeof(u8VdecBuf));
                    pstFrameHead = (FRAME_HEAD_T *)u8VdecBuf;
                    s32Ret = HI3531_GetCurrIFrameSeek(pstIFrameList, &u32FrameSeek, pstPB->tStartTime+1);
                    if (HI_SUCCESS != s32Ret)
                    {
                        s32ExitType = 0;
                        AV_PRINT("STOR_GetIFrameSeek err");
                        break;
                    }

                    fseek(pFile, u32FrameSeek, SEEK_SET);
                    s32RdLen = fread(pstFrameHead, 1, sizeof(FRAME_HEAD_T), pFile);
                    if (sizeof(FRAME_HEAD_T) != s32RdLen)
                    {
                        AV_PRINT("fread err(%s)", ERR_STR);
                        s32ExitType = 0;
                        break;
                    }
                }
                else
                {
                    memset(u8VdecBuf, 0, sizeof(u8VdecBuf));
                    pstFrameHead = (FRAME_HEAD_T *)u8VdecBuf;

                    fseek(pFile, u32FrameSeek, SEEK_SET);
                    s32RdLen = fread(pstFrameHead, 1, sizeof(FRAME_HEAD_T), pFile);
                    if (sizeof(FRAME_HEAD_T) != s32RdLen)
                    {
                        AV_PRINT("fread err(%s)", ERR_STR);
                        s32ExitType = 0;
                        break;
                    }
                }

                /*录像有问题，长度过大 */
                if (pstFrameHead->len > 1024*1024)
                {
                    u32CurFrameTime = STOR_CalTime2(pstFrameHead);
                    s32Ret = HI3531_GetCurrIFrameSeek(pstIFrameList, &u32FrameSeek, u32CurFrameTime+1);
                    if (HI_SUCCESS != s32Ret)
                    {
                        s32ExitType = 0;
                        AV_PRINT("STOR_GetIFrameSeek err");
                        break;
                    }
                }

                if (!HI3521_GetdecSpeedUP(logcalchn))
                {
                    /*获取下帧位置*/
                    u32FrameSeek = u32FrameSeek + pstFrameHead->len + sizeof(FRAME_HEAD_T);
                }

                //AV_PRINT("read frame len is (%d)", pstFrameHead->len);

                /*读取当前帧数据*/
                s32RdLen = fread(u8VdecBuf + sizeof(FRAME_HEAD_T), 1, pstFrameHead->len, pFile);
                if ((s32RdLen < 0) || (pstFrameHead->len != (HI_U32)s32RdLen))
                {
                    AV_PRINT("fread err(%s)", ERR_STR);
                    continue;
                }

                /*解码播放AUDIO 和 VIDEO*/
                if((DATA_VIDEO_IFRAME == pstFrameHead->type)||
                        ((DATA_VIDEO_PFRAME == pstFrameHead->type)&&(!HI3521_GetdecSpeedUP(logcalchn))))
                {
                    stVideoStream.pu8Addr = u8VdecBuf + sizeof(FRAME_HEAD_T);
                    stVideoStream.u32Len = pstFrameHead->len;
                    stVideoStream.u64PTS += 40000;
                    stVideoStream.bEndOfStream = HI_FALSE;
                    stVideoStream.bEndOfFrame = HI_TRUE;
                    stVideoStream.u64PTS = 0;
                    //AV_PRINT("Vdec pstFrameHead--PTS [%llu]", pstFrameHead->ts);
                    //	HI3521_PrintFrameType(s32Chn, pstFrameHead->type);
                    s32Ret = HI_MPI_VDEC_SendStream(logcalchn, &stVideoStream, -1);//-1阻塞0非阻塞+ 超时时间
                    if (HI_SUCCESS != s32Ret)
                    {
                        AV_PRINT("Send to vdec error 0x%x", s32Ret);
                        s32Ret = HI3531_GetCurrIFrameSeek(pstIFrameList, &u32FrameSeek, pstPB->tStartTime + 1);
                        if (HI_SUCCESS != s32Ret)
                        {
                            AV_PRINT("STOR_GetIFrameSeek err");
                        }

                        pstPB->tStartTime += 1;
                        continue;
                    }
                }
                else if((pstFrameHead->type == DATA_AUDIO) && (!HI3521_GetdecSpeedUP(logcalchn)))
                {

                    stAudioStream.pStream = u8VdecBuf + sizeof(FRAME_HEAD_T);
                    stAudioStream.u32Len = pstFrameHead->len;
                    stAudioStream.u64TimeStamp = 1;
                    stAudioStream.u32Seq = 1;
                    if(logcalchn == decchn)
                    {
                        s32Ret = HI_MPI_ADEC_SendStream(0, &stAudioStream, (HI_BOOL)-1);
                        if (HI_SUCCESS != s32Ret)
                        {
                            AV_PRINT("HI_MPI_ADEC_SendStream %#x",s32Ret);
                        }
                    }
                    else
                        //usleep(18*1000);//20 左右都差不多
                        TM_USleep(0,18*1000);
                    //	HI3521_PrintFrameType(s32Chn, pstFrameHead->type);
                }

                /* 判断该文件是否结束 */
                if (u32CurFrameTime > (HI_U32)pstPB->tEndTime)
                {
                    //          memset(&stVideoStream, 0, sizeof(VDEC_STREAM_S) );
                    //          stVideoStream.bEndOfStream = HI_TRUE;
                    //           HI_MPI_VDEC_SendStream(logcalchn, &stVideoStream, -1);
                    s32ExitType = 0;
                    break;
                }

                /*更新时间*/
                u32CurFrameTime = STOR_CalTime2(pstFrameHead);
                if (u32CurFrameTime > u32LastFrameTime)
                {
                    u32LastFrameTime = u32CurFrameTime;
                    pstPB->tStartTime = u32CurFrameTime;

                }
                else
                {

                    u32CurFrameTime = pstPB->tStartTime;
                }

                /*快进速度控制 */
                if(HI3521_GetdecSpeedUP(logcalchn))
                {
                    //usleep(250000);
                    TM_USleep(0,250000);
                }

            }
            else
            {
                //usleep(10000);
                TM_USleep(0,10000);
            }

        }
        //usleep(35000);
    }

    if (!HDD_IsLPBRunning())
    {
        s32ExitType = 1;
    }

    if(pstIFrameList != NULL)
    {
        free(pstIFrameList);
        pstIFrameList = NULL;
    }

    return s32ExitType;
}
HI_VOID AV_SetVideoSnapShotChn(HI_U16 u16SnapChn)

{

    Cam_Snap_Chn = u16SnapChn - 1;

    AV_PRINT("para Cam_Snap_Chn is (%d)", Cam_Snap_Chn);

}

HI_VOID AV_SetVideoRecChn(HI_U16 u16RecChn)

{
    /*打开文件*/
    Cam_En_Flag = ((u16RecChn<<8)&0xFF00)|((u16RecChn>>8)&0x0FF);
    AV_PRINT("para Cam_En_Flag is (0x%x)", Cam_En_Flag);

}



/*****************************************************************************
 函 数 名  : APP_GetIndexHeadDate
 功能描述  : 获取文件块索引指定通道头部数据
 输入参数  : FILE* pFile
             HI_S32 iChn
             STOR_BLK_IDX_CHN_HEAD_T *pstBlkIdxEntryHead
 输出参数  : 无
 返 回 值  :

 修改历史      :
  1.日    期   : 2014年5月17日
    作    者   : zhangjinlei
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 APP_GetIndexHeadDate(FILE* pFile, HI_S32 iChn,
                            STOR_BLK_IDX_CHN_HEAD_T *pstBlkIdxEntryHead)
{
    HI_S32 iSeekPos = 0;
    HI_S32 s32Ret = 0;

    iSeekPos = STOR_IDX_BLK_CHN_SEEK(iChn);
    s32Ret = fseek(pFile, iSeekPos, SEEK_SET);
    if (0 != s32Ret)
    {
        if (NULL != pFile)
        {
            fclose(pFile);
        }

        return HI_FAILURE;
    }

    s32Ret = fread(pstBlkIdxEntryHead,
                   sizeof(STOR_BLK_IDX_CHN_HEAD_T),
                   1,
                   pFile);
    if (s32Ret <= 0)
    {
        if (NULL != pFile)
        {
            fclose(pFile);
        }
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}


/* END:   Added by mabuyun, 2015/6/10   PN:20150608 */

HI_S32 HI3531_ChkSearchTime(time_t *tStartTime, time_t *tEndTime, bool bLocal)
{
    HI_S32 iTmptime = 0;

    if ( *tEndTime <= *tStartTime )
    {
        AV_PRINT("search time error,tStartTime=%ld,tEndTime=%ld", *tStartTime, *tEndTime);
        return HI_FAILURE;
    }
    iTmptime = *tEndTime - *tStartTime;
    AV_PRINT("search time,tStartTime=%ld,tEndTime=%ld,iTmptime= %d", *tStartTime, *tEndTime,iTmptime);
    if (1 == bLocal)
    {
        /*本地搜索 */
        if ( LOCAL_SEARCH_MAX_TIME < iTmptime )
        {
            *tEndTime = *tStartTime + LOCAL_SEARCH_MAX_TIME;
        }
        else if ( iTmptime < LOCAL_SEARCH_MIN_TIME)
        {
            AV_PRINT("search time too short,tStartTime=%ld,tEndTime=%ld", *tStartTime, *tEndTime);
            return HI_FAILURE;
        }
    }
    else
    {
        /*远程搜索 */
        if ( SEARCH_FILE_MAX_TIME < iTmptime )
        {
            *tEndTime = *tStartTime + SEARCH_FILE_MAX_TIME;
        }
        else if ( iTmptime < SEARCH_FILE_MIN_TIME)
        {
            AV_PRINT("search time too short,tStartTime=%ld,tEndTime=%ld", *tStartTime, *tEndTime);
            return HI_FAILURE;
        }
    }

    return HI_SUCCESS;
}


/*****************************************************************************
 函 数 名  : APP_ReadOneChnPlaybackName
 功能描述  : 获取单个通道文件
 输入参数  : REC_PLAYBACK_T *pstPlaybackTime
             HI_CHAR *pcFileName
             HI_S32 *piFileCnt
             HI_S32 iChn
             REC_PLAYBACK_PART_T *pstPart
             HI_S32 iFileStatue
 输出参数  : 无
 返 回 值  :

 修改历史      :
  1.日    期   : 2014年5月17日
    作    者   : zhangjinlei
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 APP_ReadOneChnPlaybackName(REC_PLAYBACK_T *pstPlaybackTime,
                                  HI_CHAR *pcFileName,
                                  HI_S32 *piFileCnt,
                                  HI_S32 iChn,
                                  REC_PLAYBACK_PART_T *pstPart,
                                  HI_S32 iFileStatue)
{
    //REC_PLAYBACK_T *pstPlaybackTimes = NULL;
    //HI_CHAR *pcFileNames = NULL;
    HI_S32 s32Ret = 0;
    FILE* pFile = NULL;
    HI_CHAR cFileName[APP_PLAYBACK_FILE_LEN]= {0};
    STOR_BLK_IDX_CHN_HEAD_T stBlkIdxHead;
    STOR_BLK_IDX_ENTRY_T *pstBufferDate = NULL;
    STOR_BLK_IDX_ENTRY_T *pTmp = NULL;
//    STOR_BLK_IDX_ENTRY_T *pTmpNext = NULL;
    HI_S32 i = 0;
    //HI_S32 s32Index = 0;
    HI_S32 iFileBlockCnt = 0;
    HI_S32 iFirstIndex = 0;
    //HI_S32 iSecondIndex = 0;
    HI_S32 iFileCnt = 0;
    HI_S32 s32FirstPartBlockCnt = 0;
    //HI_S32 iFileNumMax = 0;

    if ((NULL == pstPlaybackTime) ||
            (NULL == pcFileName) ||
            (NULL == piFileCnt) ||
            (NULL == pstPart))
    {
        AV_PRINT( " Input NUll pointer, p1=%d,p2=%d,p3=%d,p4=%d",
                  (HI_S32)pstPlaybackTime, (HI_S32)pcFileName,(HI_S32)piFileCnt, (HI_S32)pstPart);

        return HI_FAILURE;
    }

    //iFileNumMax = APP_OnePartFileNum();
    pstBufferDate = (STOR_BLK_IDX_ENTRY_T *)malloc((STOR_RECFILE_MAX_NUMS*2)*sizeof(STOR_BLK_IDX_ENTRY_T));
    if (NULL == pstBufferDate)
    {
        AV_PRINT( " APP_GetIndexHeadDate");

        return HI_FAILURE;
    }

    AV_PRINT( " APP_GetIndexHeadDate,iFileStatue=%d",iFileStatue);

    /* 根据文件类型进行处理 */
    if ((APP_PLAYBACK_BOTH_FIRST == iFileStatue) ||
            (APP_PLAYBACK_ONLY_FIRST == iFileStatue) ||
            (APP_PLAYBACK_BOTH_FIRST_SECOND == iFileStatue) ||
            (APP_PLAYBACK_ONLY_SECOND == iFileStatue))
    {
        memset(cFileName, 0, sizeof(cFileName));
        if (APP_PLAYBACK_ONLY_SECOND == iFileStatue)
        {
            sprintf(cFileName, "%s%d%s%s","/mnt/a", pstPart->s32SecondPart+5,"/","index00.tmm");
        }
        else
        {
            sprintf(cFileName, "%s%d%s%s","/mnt/a",pstPart->s32FirstPart+5,"/","index00.tmm");
        }

        AV_PRINT( " cFileName=%s", cFileName);

        pFile = fopen(cFileName, "rb");
        if (NULL == pFile)
        {
            AV_PRINT( " cFileName=%s", cFileName);
            if (NULL != pstBufferDate)
            {
                free(pstBufferDate);
            }

            return HI_FAILURE;
        }


        s32Ret = APP_GetOneChnAllBlockInfo(pFile, (HI_CHAR *)pstBufferDate, &stBlkIdxHead, iChn);
        if (HI_SUCCESS != s32Ret)
        {
            AV_PRINT( " APP_GetIndexHeadDate");
            if (NULL != pFile)
            {
                fclose(pFile);
            }
            if (NULL != pstBufferDate)
            {
                free(pstBufferDate);
            }

            return HI_FAILURE;
        }

        iFileBlockCnt = iFileBlockCnt + stBlkIdxHead.block_total_num;
        s32FirstPartBlockCnt = iFileBlockCnt;

        AV_PRINT( " iFileBlockCnt=%d", iFileBlockCnt);

        if (NULL != pFile)
        {
            fclose(pFile);
        }

    }
    else
    {
        AV_PRINT( " APP_GetIndexHeadDate,iFileStatue=%d",iFileStatue);
    }

    if (APP_PLAYBACK_BOTH_FIRST_SECOND == iFileStatue)
    {
        memset(cFileName, 0, sizeof(cFileName));
        sprintf(cFileName, "%s%d%s%s","/mnt/a", pstPart->s32SecondPart+5,"/","index00.tmm");
        pFile = fopen(cFileName, "rb");
        if (NULL == pFile)
        {
            AV_PRINT( " cFileName=%s", cFileName);
            if (NULL != pstBufferDate)
            {
                free(pstBufferDate);
            }

            return HI_FAILURE;
        }

        //s32Ret = APP_GetOneChnAllBlockInfo(pFile,
        //                                    pstBufferDate+(stBlkIdxHead.block_total_num*sizeof(STOR_BLK_IDX_ENTRY_T)),
        //                                    &stBlkIdxHead, iChn);
        s32Ret = APP_GetOneChnAllBlockInfo(pFile,
                                           (HI_CHAR *)(pstBufferDate + stBlkIdxHead.block_total_num),
                                           &stBlkIdxHead, iChn);
        if (HI_SUCCESS != s32Ret)
        {
            AV_PRINT( " APP_GetIndexHeadDate");
            if (NULL != pFile)
            {
                fclose(pFile);
            }

            if (NULL != pstBufferDate)
            {
                free(pstBufferDate);
            }

            return HI_FAILURE;
        }

        iFileBlockCnt+= stBlkIdxHead.block_total_num;
        AV_PRINT( " iFileBlockCnt=%d", iFileBlockCnt);

        if (NULL != pFile)
        {
            fclose(pFile);
        }
    }

    if ((APP_PLAYBACK_BOTH_FIRST == iFileStatue) ||
            (APP_PLAYBACK_ONLY_FIRST == iFileStatue) ||
            (APP_PLAYBACK_BOTH_FIRST_SECOND == iFileStatue))
    {
        for ( i = 0; i < iFileBlockCnt; i++ )
        {
            pTmp = pstBufferDate + i;
            //         pTmpNext = pstBufferDate + i + 1;
            if ((pTmp->end_time > (HI_U32)pstPlaybackTime->tStartTime)||
                    (pTmp->start_time == (HI_U32)pstPlaybackTime->tStartTime)||
                    (pTmp->end_time > (HI_U32)pstPlaybackTime->tEndTime))
            {
                iFirstIndex = i;
                break;
            }
        }

        if (iFileBlockCnt == i)
        {
            iFirstIndex = iFileBlockCnt - 1;
        }

        AV_PRINT("iFirstIndex=%d", iFirstIndex);

        for ( i = iFirstIndex; (i < iFileBlockCnt)&&(iFileCnt < 10); i++ )
        {
            pTmp = pstBufferDate + i;
            memset(cFileName, 0, sizeof(cFileName));
            if (i < s32FirstPartBlockCnt)
            {
                sprintf(cFileName, "%s%d%s%s%s","/mnt/a", pstPart->s32FirstPart+5,"/",pTmp->blockname, ".tmm");
            }
            else
            {
                sprintf(cFileName, "%s%d%s%s%s","/mnt/a", pstPart->s32SecondPart+5,"/",pTmp->blockname, ".tmm");
            }

            //AV_PRINT( " cFileName=%s, ", cFileName);
            AV_PRINT("pTmp->start_time=%u, pTmp->end_time=%u,pstPlaybackTime->start_time=%u, pstPlaybackTime->end_time=%u,i=%d",
                     (HI_S32)pTmp->start_time, (HI_S32)pTmp->end_time,
                     (HI_S32)pstPlaybackTime->tStartTime,
                     (HI_S32)pstPlaybackTime->tEndTime, i);
            if ((pTmp->end_time > (HI_U32)pstPlaybackTime->tStartTime)||
                    (pTmp->start_time == (HI_U32)pstPlaybackTime->tStartTime)||
                    (pTmp->end_time > (HI_U32)pstPlaybackTime->tEndTime))
            {
                memcpy(pcFileName+iFileCnt*APP_PLAYBACK_FILE_LEN, cFileName, sizeof(cFileName));
                AV_PRINT("cFileName=%s, iFileCnt=%d", pcFileName+iFileCnt*APP_PLAYBACK_FILE_LEN, iFileCnt);
                AV_PRINT("cFileName=%s", cFileName);
                iFileCnt++;
            }

            if (pTmp->end_time > (HI_U32)pstPlaybackTime->tEndTime)
            {
                break;
            }
        }
    }
    else if (APP_PLAYBACK_ONLY_SECOND == iFileStatue)
    {
        iFirstIndex = 0;
        for ( i = iFirstIndex; (i < iFileBlockCnt)&&(iFileCnt < 10); i++ )
        {
            pTmp = pstBufferDate + i;
            memset(cFileName, 0, sizeof(cFileName));
            sprintf(cFileName, "%s%d%s%s%s","/mnt/a", pstPart->s32SecondPart+5,"/",pTmp->blockname, ".tmm");
            //AV_PRINT( " cFileName=%s", cFileName);
            memcpy(pcFileName+iFileCnt*APP_PLAYBACK_FILE_LEN, cFileName, sizeof(cFileName));
            //AV_PRINT( " cFileName=%s, iFileCnt=%d", pcFileName+iFileCnt*APP_PLAYBACK_FILE_LEN,
            //    iFileCnt);
            iFileCnt++;
            if (pTmp->end_time > (HI_U32)pstPlaybackTime->tEndTime)
            {
                break;
            }
        }
    }

    *piFileCnt = iFileCnt;

    if (NULL != pstBufferDate)
    {
        free(pstBufferDate);
    }

    return HI_SUCCESS;
}



/*****************************************************************************
 函 数 名  : APP_GetRecodeFileStatue
 功能描述  : 获取录像文件状态
 输入参数  : REC_PLAYBACK_T *pstPlaybackTime
             HI_S32 *piFileStatue
 输出参数  : 无
 返 回 值  :

 修改历史      :
  1.日    期   : 2014年5月16日
    作    者   : zhangjinlei
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 APP_GetRecodeFileStatue(REC_PLAYBACK_T *pstPlaybackTime,
                               HI_S32 *piFileStatue, HI_S32 Chn,
                               REC_PLAYBACK_PART_T *pstPlaybackPart)
{
    HI_S32 s32Ret = 0;
    time_t ulNewestTime[APP_DISK_PART_NUM] = {0};
    time_t ulOldestTime[APP_DISK_PART_NUM] = {0};
    HI_S32 i = 0;
    HI_CHAR cFileName[64]= {0};

    HI_S32 iRecodeFileState = 0;
    REC_PLAYBACK_T *pstRecodePlay = NULL;
    HI_S32 iPlayBackStatue = 0;
    HI_S32 m = 6;
    HI_S32 n = 6;
    time_t ulNewestTimeTmp[APP_DISK_PART_NUM + 1] = {0};
    time_t ulOldestTimeTmp[APP_DISK_PART_NUM + 1] = {0};
    HI_S32 s32CurPart = 0;

    pstRecodePlay = pstPlaybackTime;

    for ( i = 0; i < APP_DISK_PART_NUM; i++ )
    {
        memset(cFileName, 0, sizeof(cFileName));
        sprintf(cFileName, "%s%d%s%s","/mnt/a",i+5,"/","index00.tmm");

        s32Ret = STOR_GetDiskNewestOldestTime(cFileName, Chn, ulNewestTime+i, ulOldestTime+i);
        if (HI_SUCCESS != s32Ret)
        {
            AV_PRINT("STOR_GetDiskNewestOldestTime,cFileName = %s", cFileName);
            return HI_FAILURE;
        }

        AV_PRINT("STOR_GetDiskNewestOldestTime, cFileName = %s,ulNewestTime=%d, ulOldestTime = %d",
                 cFileName, (HI_S32)ulNewestTime[i], (HI_S32)ulOldestTime[i]);

    }

    /* 获取当前使用硬盘分区 */
    s32Ret = STOR_GetRecPartId();
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT( " STOR_GetRecPartId failed");

        return HI_FAILURE;
    }

    //s32CurPart = g_store_thread_info.prt - 1;

    ulNewestTimeTmp[0] = ulOldestTime[(s32CurPart + 1)%APP_DISK_PART_NUM];
    ulOldestTimeTmp[0] = ulOldestTime[s32CurPart];
    if (ulNewestTimeTmp[0] < ulOldestTimeTmp[0])
    {
        ulNewestTimeTmp[0] = ulOldestTimeTmp[0];
    }

    for ( i = 1; i < APP_DISK_PART_NUM; i++ )
    {
        ulNewestTimeTmp[i] = ulNewestTime[(s32CurPart + i)%APP_DISK_PART_NUM];
        ulOldestTimeTmp[i] = ulOldestTime[(s32CurPart + i)%APP_DISK_PART_NUM];
    }

    ulNewestTimeTmp[4] = ulNewestTime[(s32CurPart + 4)%APP_DISK_PART_NUM];
    if (0 == ulNewestTime[(s32CurPart + 3)%APP_DISK_PART_NUM])
    {
        ulOldestTimeTmp[4] = ulOldestTime[s32CurPart];
    }
    /* BEGIN: Added by zhangjinlei, 2014/8/5  DES:解决本地回放查找文件失败问题  PN:140805001 */
    else
    {
        ulOldestTimeTmp[4] = ulNewestTime[(s32CurPart + 3)%APP_DISK_PART_NUM];
    }
    /* END:   Added by zhangjinlei, 2014/8/5   PN:140805001 */

    for ( i = 0; i < APP_DISK_PART_NUM + 1; i++ )
    {
        if ((pstRecodePlay->tStartTime >= ulOldestTimeTmp[i])&&
                (pstRecodePlay->tStartTime < ulNewestTimeTmp[i]))
        {
            iRecodeFileState = iRecodeFileState|0x01;
            m = i;
            pstPlaybackPart->s32FirstPart = (s32CurPart + i)%APP_DISK_PART_NUM;

            AV_PRINT( " i=%d, pstRecodePlay->tStartTime = %d,ulNewestTime=%d, ulOldestTime = %d",
                      i, (HI_S32)pstRecodePlay->tStartTime, (HI_S32)ulNewestTimeTmp[i], (HI_S32)ulOldestTimeTmp[i]);
        }

        if ((pstRecodePlay->tEndTime > ulOldestTimeTmp[i])&&
                (pstRecodePlay->tEndTime <= ulNewestTimeTmp[i]))
        {
            iRecodeFileState = iRecodeFileState|0x02;
            n = i;
            pstPlaybackPart->s32SecondPart = (s32CurPart + i)%APP_DISK_PART_NUM;

            AV_PRINT( " i=%d, pstRecodePlay->tEndTime = %d,ulNewestTime=%d, ulOldestTime = %d",
                      i, (HI_S32)pstRecodePlay->tEndTime, (HI_S32)ulNewestTimeTmp[i], (HI_S32)ulOldestTimeTmp[i]);
        }
    }

    if (iRecodeFileState)
    {
        /* 判断文件构成形式 */
        /*****************************************
        文件构成形式
           分区1                分区2
         ____________       ____________
        |            |     |            |
        |____________|     |____________|

         only first    0x30  0011 0000B
               __________
              |__________|

         only second   0x0C  0000 1100B
                         ________
                        |________|

         both first second  0x3C   0011 1100B
               __________________
              |__________________|

         both first    0x60    0110 0000B
             _____
            |_____|

                            0x81  1000 0001B
        __________________________________________
        |__________________________________________|

        ******************************************/
        if (0x01 == (iRecodeFileState&0x03))
        {
            iPlayBackStatue = APP_PLAYBACK_ONLY_FIRST;
        }
        else if (0x02 == (iRecodeFileState&0x03))
        {
            iPlayBackStatue = APP_PLAYBACK_ONLY_SECOND;
        }
        else if (0x03 == (iRecodeFileState&0x03))
        {
            if ((m == n)&&(m != 6))
            {
                iPlayBackStatue = APP_PLAYBACK_BOTH_FIRST;
            }
            else
            {
                iPlayBackStatue = APP_PLAYBACK_BOTH_FIRST_SECOND;
            }
        }
    }
    else
    {
        AV_PRINT( " APP_GetRecodeFileStatue,iRecodeFileState = %d", iRecodeFileState);
        return HI_FAILURE;
    }

    *piFileStatue = iPlayBackStatue;
    AV_PRINT( " STOR_GetDiskNewestOldestTime,iPlayBackStatue = 0x%x,ulNewestTime=%d, ulOldestTime = %d",
              iPlayBackStatue, (HI_S32)ulNewestTimeTmp[i-1], (HI_S32)ulOldestTimeTmp[i-1]);
    return HI_SUCCESS;

}


/*****************************************************************************
 函 数 名  : APP_GetRecFileByChn
 功能描述  : 或一个通道录像文件
 输入参数  : REC_PLAYBACK_T *pstPlaybackTime
             HI_CHAR *pcFileName
             HI_S32 *piFileCnt
             HI_S32 iCHn
 输出参数  : 无
 返 回 值  :

 修改历史      :
  1.日    期   : 2014年5月20日
    作    者   : zhangjinlei
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 APP_GetRecFileByChn(REC_PLAYBACK_T *pstPlaybackTime, HI_CHAR *pcFileName,
                           HI_S32 *piFileCnt,HI_S32 iCHn)
{
    HI_S32 s32Ret = 0;
    HI_CHAR *pFileBuffer = NULL;
    HI_S32 iFileStatue = 0;
    REC_PLAYBACK_PART_T stPlaybackPart;
    HI_S32 i = 0;

    if ((NULL == pcFileName)|| (NULL == piFileCnt))
    {
        AV_PRINT("Input NUll pointer");

        return HI_FAILURE;
    }

    pFileBuffer = pcFileName;

    s32Ret = APP_GetRecodeFileStatue(pstPlaybackTime, &iFileStatue, iCHn, &stPlaybackPart);
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT( " APP_GetRecodeFileStatue");

        return HI_FAILURE;
    }

    AV_PRINT( " APP_GetRecodeFileStatue,iFileStatue=0x%x", iFileStatue);

    s32Ret = APP_ReadOneChnPlaybackName(pstPlaybackTime, pFileBuffer, piFileCnt, iCHn,
                                        &stPlaybackPart, iFileStatue);
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT( " APP_ReadOneChnPlaybackName");

        return HI_FAILURE;
    }

    for ( i = 0; i < *piFileCnt; i++ )
    {
        AV_PRINT( " PlaybackName=%s, fileCnt=%d, iCHn=%d",
                  pFileBuffer + i*64, i, iCHn);
    }

    return HI_SUCCESS;

}


/*****************************************************************************
 函 数 名  : APP_GetRecodeFileName
 功能描述  : 获取录像文件名
 输入参数  : REC_PLAYBACK_T *pstPlaybackTime
             HI_CHAR *pcFileName
             HI_S32 iFileCnt
 输出参数  : 无
 返 回 值  :

 修改历史      :
  1.日    期   : 2014年5月16日
    作    者   : zhangjinlei
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 APP_GetRecodeFileName(REC_PLAYBACK_T *pstPlaybackTime,HI_CHAR *pcFileName,
                             HI_S32 *piFileCnt,HI_S32 iPlayBackMode)
{
    HI_S32 s32Ret = 0;
    HI_CHAR *pFileBuffer = NULL;
    HI_S32 i = 0;
    HI_S32 iTmp = 0;

    if ((NULL == pcFileName)||(NULL == piFileCnt))
    {
        AV_PRINT("Input NUll pointer");
        return HI_FAILURE;
    }

    pFileBuffer = pcFileName;

    /* 获取需要获取时间段内文件跨度分区情况 */
    g_s32PBChnStat = 0;
    if (PLAY_BACK_1TO4CHN == iPlayBackMode)
    {
        for(i=0; i<APP_VDEC_CHNCNT/2; i++)
        {
            iTmp = 640*i;
            s32Ret = APP_GetRecFileByChn(pstPlaybackTime, pFileBuffer + iTmp, piFileCnt + i, i);
            if (HI_SUCCESS != s32Ret)
            {
                AV_PRINT("APP_GetRecFileByChn,i=%d,p2=%d",i, iTmp);
            }
            else
            {
                g_s32PBChnStat = (g_s32PBChnStat|(0x01<<i));
            }

        }
    }
    else if (PLAY_BACK_4TO8CHN == iPlayBackMode)
    {
        for(i=APP_VDEC_CHNCNT/2; i<APP_VDEC_CHNCNT; i++)
        {
            iTmp = 640*i;
            s32Ret = APP_GetRecFileByChn(pstPlaybackTime, pFileBuffer + iTmp, piFileCnt + i, i);
            if (HI_SUCCESS != s32Ret)
            {
                AV_PRINT( " APP_GetRecFileByChn,i=%d,p2=%d",
                          i, iTmp);

            }
            else
            {
                g_s32PBChnStat = (g_s32PBChnStat|(0x01<<i));
            }
        }
    }

    if (0 == g_s32PBChnStat)
    {
        return HI_FAILURE;
    }
    //sleep(100);
    return HI_SUCCESS;

}



/*****************************************************************************
 函 数 名  : APP_GetOneIndexDate
 功能描述  : 获取指定通道视频块索引
 输入参数  : FILE* pFile
             HI_S32 iChn
             HI_S32 s32Index
             STOR_BLK_IDX_ENTRY_T *pstBlkIdxEntry
 输出参数  : 无
 返 回 值  :

 修改历史      :
  1.日    期   : 2014年5月16日
    作    者   : zhangjinlei
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 APP_GetOneIndexDate(FILE* pFile, HI_S32 iChn, HI_S32 s32Index,
                           STOR_BLK_IDX_ENTRY_T *pstBlkIdxEntry)
{
    HI_S32 s32Ret = 0;
    HI_S32 iSeekPos = 0;

    iSeekPos = STOR_IDX_BLK_CHN_SEEK(iChn)
               + sizeof(STOR_BLK_IDX_CHN_HEAD_T)
               + s32Index * sizeof(STOR_BLK_IDX_ENTRY_T);
    s32Ret = fseek(pFile, iSeekPos, SEEK_SET);
    if (0 != s32Ret)
    {
        if (NULL != pFile)
        {
            fclose(pFile);
        }
        return HI_FAILURE;
    }
    s32Ret = fread(pstBlkIdxEntry,
                   sizeof(STOR_BLK_IDX_ENTRY_T), 1,
                   pFile);
    if (s32Ret <= 0)
    {
        if (NULL != pFile)
        {
            fclose(pFile);
        }

        return HI_FAILURE;
    }

    return HI_SUCCESS;
}


/*****************************************************************************
 函 数 名  : APP_SendEos
 功能描述  :
 输入参数  : VDEC_CHN VdChn
 输出参数  : 无
 返 回 值  : HI_VOID



 修改历史      :
  1.日    期   : 2013年8月23日
    作    者   : wsm
    修改内容   : 新生成函数

*****************************************************************************/
static HI_VOID APP_SendEos(VDEC_CHN VdChn)
{
    VDEC_STREAM_S stStreamEos;
    HI_U32 au32EOS[2] = {0x01000000, 0x0100000b};

    stStreamEos.pu8Addr = (HI_U8 *)au32EOS;
    stStreamEos.u32Len = sizeof(au32EOS);
    stStreamEos.u64PTS = 0;

    (HI_VOID)HI_MPI_VDEC_SendStream(VdChn, &stStreamEos, HI_IO_BLOCK);

    return;
}


/*****************************************************************************
 函 数 名  : APP_GetOneChnAllBlockInfo
 功能描述  : 获取单个通道所有视频文件块索引信息
 输入参数  : FILE* pFile
             HI_CHAR *pcBuffer
             STOR_BLK_IDX_CHN_HEAD_T *pstIndexHead
             HI_S32 iChn
 输出参数  : 无
 返 回 值  :

 修改历史      :
  1.日    期   : 2014年6月11日
    作    者   : zhangjinlei
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 APP_GetOneChnAllBlockInfo(FILE* pFile, HI_CHAR *pcBuffer, STOR_BLK_IDX_CHN_HEAD_T *pstIndexHead, HI_S32 iChn)
{
    HI_S32 s32Ret = 0;
    HI_S32 s32OldIndex = 0;
    HI_S32 s32NewIndex = 0;
    HI_S32 s32IndexCounter = 0;
    HI_S32 i = 0;
    HI_S32 s32Index = 0;
    STOR_BLK_IDX_ENTRY_T *pstBlkIdxEntry = NULL;
    //HI_S32 iFileNumMax = 0;

    /* 打开该分区索引表 */
    s32Ret = APP_GetIndexHeadDate(pFile, iChn, pstIndexHead);
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT( " APP_GetIndexHeadDate");

        return HI_FAILURE;
    }

    s32OldIndex = pstIndexHead->block_oldest_index;
    s32NewIndex = pstIndexHead->block_newest_index;
    s32IndexCounter = pstIndexHead->block_total_num;

    AV_PRINT( " s32OldIndex=%d,s32NewIndex=%d,s32IndexCounter=%d",
              s32OldIndex, s32NewIndex, s32IndexCounter);

    //iFileNumMax = APP_OnePartFileNum();
    for ( i = 0; i < s32IndexCounter; i++ )
    {
        s32Index = (s32OldIndex + i)%STOR_RECFILE_MAX_NUMS;
        pstBlkIdxEntry = (STOR_BLK_IDX_ENTRY_T *)(pcBuffer + i * sizeof(STOR_BLK_IDX_ENTRY_T));
        /* 获取索引块数据 */
        s32Ret = APP_GetOneIndexDate(pFile, iChn, s32Index, pstBlkIdxEntry);
        if (HI_SUCCESS != s32Ret)
        {
            AV_PRINT( " APP_GetIndexHeadDate");

            return HI_FAILURE;
        }
    }

    return HI_SUCCESS;

}


/*****************************************************************************
 函 数 名  : APP_SortSearchFileSeg
 功能描述  : 对搜索的文件按照时间先后顺序排序
 输入参数  : SF_LIST_ST *pstSFList
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年9月15日
    作    者   : zoulq
    修改内容   : 新生成函数

*****************************************************************************/
HI_VOID APP_SortSearchFileSeg(SF_LIST_ST *pstSFList)
{
    HI_S32 i;
    HI_S32 j;
    HI_S32 iTempTimeI;
    HI_S32 iTempTimeJ;
    SF_TIMESEG_ST stSFTimeSeg;
    /*实际搜索出来的文件片段个数  */
    HI_S32 iFileCnt = pstSFList->s32FileSegNum;

    for (i = 0; i <iFileCnt-1 ; i++)
    {
        for ( j = i+1 ; j < iFileCnt; j++ )
        {
            iTempTimeI = pstSFList->stFileTimeSeg[i].tStartTime;
            iTempTimeJ = pstSFList->stFileTimeSeg[j].tStartTime;

            if (iTempTimeI > iTempTimeJ)
            {
                memcpy(&stSFTimeSeg, &pstSFList->stFileTimeSeg[i], sizeof(SF_TIMESEG_ST));
                memcpy(&pstSFList->stFileTimeSeg[i], &pstSFList->stFileTimeSeg[j], sizeof(SF_TIMESEG_ST));
                memcpy(&pstSFList->stFileTimeSeg[j], &stSFTimeSeg,sizeof(SF_TIMESEG_ST));
            }
        }
    }

}

HI_VOID APP_SortByTimeSeg(SF_TIMESEG_ST *pBuf, HI_S32 s32FileNums)
{
    HI_S32 i;
    HI_S32 j;
    HI_S32 iTempTimeI;
    HI_S32 iTempTimeJ;
    SF_TIMESEG_ST tmp;
    SF_TIMESEG_ST stSFTimeSeg1;
    SF_TIMESEG_ST stSFTimeSeg2;
    /*实际搜索出来的文件片段个数  */
    HI_S32 iFileCnt = s32FileNums;

    for (i = 0; i <iFileCnt-1 ; i++)
    {
        AV_PRINT("111111111111i%d", i);
        for ( j = i+1 ; j < iFileCnt; j++ )
        {
            memcpy(&stSFTimeSeg1, pBuf+i, sizeof(SF_TIMESEG_ST));
            iTempTimeI = stSFTimeSeg1.tStartTime;
            memcpy(&stSFTimeSeg2, pBuf+j, sizeof(SF_TIMESEG_ST));
            iTempTimeJ = stSFTimeSeg2.tStartTime;
            AV_PRINT("###################J %d--%d", iTempTimeI, iTempTimeJ);
            if (iTempTimeI > iTempTimeJ)
            {
                memcpy(&tmp, pBuf+i, sizeof(SF_TIMESEG_ST));
                memcpy(pBuf+i, pBuf+j, sizeof(SF_TIMESEG_ST));
                memcpy(pBuf+j, &tmp,sizeof(SF_TIMESEG_ST));
            }
        }
    }

}



/*****************************************************************************
 函 数 名  : HI3531_ConvertTime2LocaltimeStr
 功能描述  : 将时间转换为字符串输出
 输入参数  : time_t t
             HI_CHAR *pTime
 输出参数  : 无
 返 回 值  :

 修改历史      :
  1.日    期   : 2014年08月27日
    作    者   : zoulq
    修改内容   : 新生成函数

*****************************************************************************/


/*****************************************************************************
 函 数 名  : APP_SendOldRecodeToMem
 功能描述  : 将从录像文件中获取到的视频帧数据写入到远程预览公共内存
 输入参数  : DSKFRAMEHEADER *pstFrameHead
             HI_CHAR *pcData
             len1..len2 发送的数据长度
 输出参数  : 无
 返 回 值  :

 修改历史      :
  1.日    期   : 2014年2月27日
    作    者   : zhangjinlei
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 APP_SndFrame2Shm(HI_S32 iChn, FRAME_HEAD_T *pstFrameHead, HI_CHAR *pcData,
                        HI_S32 len1, HI_S32 len2)
{
    FRAME_HEAD_T *pstFrame = NULL;
    HI_CHAR *pRecData = NULL;
    SHM_HEAD_T *pstShmHead = NULL;
    static HI_S32 iFrameID[8] = {0};
    HI_CHAR *pcVIPframe = NULL;
    HI_S32 s32Ret = 0;
    HI_S32 s32Len = 0;
    SHM_SEM_MNG_T *pstShmSemMng = NULL;
    SEM_MNG_T *pstSemMng = NULL;
    SHM_MNG_T *pstShmMng = NULL;
    SHM_ATTR_T *pstShmAttr = NULL;

    if ((NULL == pstFrameHead) || (NULL == pcData))
    {
        AV_PRINT( " Input NULL pointer");
        return HI_FAILURE;
    }

    HI3531_GetShmSemMng(IPC_VI_PREV_E, &pstShmSemMng);
    if (NULL == pstShmSemMng)
    {
        AV_PRINT( " HI3531_GetShmSemMng err! ");
        return HI_FAILURE;
    }
    pstSemMng = &pstShmSemMng->semMng;
    pstShmMng = &pstShmSemMng->shmMng;
    pstShmAttr = &pstShmMng->shmAttr;

    pstShmHead = (SHM_HEAD_T *)pcData;
    pRecData = pcData;
    pstFrame = pstFrameHead;

    //  pstShmHead->dataSize = pstFrame->len - sizeof(FRAME_HEAD_T);
    pstShmHead->dataSize = pstFrame->len+sizeof(SHM_HEAD_T)+ len1;

    pstShmHead->mediaType = 1;
    pstShmHead->video.byChnIndex = iChn;
    pstShmHead->video.byMediaType = 98;
    pstShmHead->video.byLastSnd = 0;

    //  pstShareMemHead->video.dwlen = pstFrame->len - sizeof(FRAME_HEAD_T);
    pstShmHead->video.dwlen = pstFrame->len+len1;

    pstShmHead->video.FrameID = iFrameID[iChn]++;
    pstShmHead->video.timeStamp = pstFrame->ts;
    //  AV_PRINT("--frame-ts-%ld and timstamp is %d.\n",pstFrame->ts,pstShareMemHead->video.timeStamp);

    /* 获取关键帧 */
    /* 关键帧前道码 00 00 00 01 67 */
    pcVIPframe = pRecData + sizeof(SHM_HEAD_T) + len1 + 4;
    if (APP_VIDEO_VIP_FRAME == *pcVIPframe)
    {
        pstShmHead->video.byFrameType = 3;
    }
    else
    {
        pstShmHead->video.byFrameType = 0;
    }

    s32Len = pstShmHead->dataSize;
    //  AV_PRINT( " send av frame s32Len is %d",s32Len);
    if (s32Len > pstShmAttr->s32ItemSize)
    {
        AV_PRINT( " Write data biger than zero! s32Len=%d, s32ItemSize=%d",
                  s32Len , pstShmAttr->s32ItemSize);
        s32Len = pstShmAttr->s32ItemSize;
    }

    s32Ret = MEM_WrShm(pstShmAttr, pcData, s32Len, pstSemMng->sysSemId);

#if 0
    if(iChn == 0)
    {
        tm_save_data_to_file("/mnt/nand/sendfilelocalfile0.dat", pcData+sizeof(SHM_HEAD_T), s32Len-sizeof(SHM_HEAD_T)
                             , iSeek[0]);
        iSeek[0] += (s32Len-sizeof(SHM_HEAD_T));
    }
    if(iChn == 1)
    {
        tm_save_data_to_file("/mnt/nand/sendfilelocalfile1.dat", pcData+sizeof(SHM_HEAD_T), s32Len-sizeof(SHM_HEAD_T)
                             , iSeek[1]);
        iSeek[1] += (s32Len-sizeof(SHM_HEAD_T));
    }
    if(iChn == 2)
    {
        tm_save_data_to_file("/mnt/nand/sendfilelocalfile2.dat", pcData+sizeof(SHM_HEAD_T), s32Len-sizeof(SHM_HEAD_T)
                             , iSeek[2]);
        iSeek[2] += (s32Len-sizeof(SHM_HEAD_T));
    }
    if(iChn == 3)
    {
        tm_save_data_to_file("/mnt/nand/sendfilelocalfile3.dat", pcData+sizeof(SHM_HEAD_T), s32Len-sizeof(SHM_HEAD_T)
                             , iSeek[3]);
        iSeek[3] += (s32Len-sizeof(SHM_HEAD_T));
    }
#endif
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT( " MEM_WrShm err!");

        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

/*****************************************************************************
 函 数 名  : APP_GetSearchFileTimeSeg
 功能描述  : 获取搜索时间的片段
 输入参数  : SF_REQ_CMD_ST *pstSearchFile
             SF_LIST_ST *pstSFList
             REC_PLAYBACK_PART_T *pstPart
             HI_S32 iChn
             HI_S32 iFileStatue
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年9月27日
    作    者   : zoulq
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 APP_GetSearchFileTimeSeg(SF_REQ_CMD_ST *pstSearchFile,
                                SF_LIST_ST *pstSFList,
                                REC_PLAYBACK_PART_T *pstPart,
                                HI_S32 iChn,
                                HI_S32 iFileStatue)
{
    HI_S32 s32Ret = 0;
    FILE* pFile = NULL;
    HI_CHAR cFileName[APP_PLAYBACK_FILE_LEN]= {0};
    HI_CHAR *pBlockFilebuff = NULL;
    STOR_BLK_IDX_CHN_HEAD_T stBlkIdxHead;
    STOR_BLK_IDX_ENTRY_T stBlockIdxData;
    STOR_BLK_IDX_ENTRY_T *pTmp = &stBlockIdxData;
    HI_S32 i = 0;
    HI_S32 iFileBlockCnt = 0;
    HI_S32 iFirstIndex = 0;

    HI_S32 iFileCnt = 0;
    HI_S32 s32FirstPartBlockCnt = 0;
    SF_TIMESEG_ST stFileTimeSeg;

    if ((NULL == pstSearchFile) ||
            (NULL == pstSFList) ||
            (NULL == pstPart))
    {
        AV_PRINT( " Input NUll pointer, p1=%d,p2=%d,p3=%d",
                  (HI_S32)pstSearchFile, (HI_S32)pstSFList, (HI_S32)pstPart);

        return HI_FAILURE;
    }

    /*分配一个缓冲用来存放符合条件的分区数据块所有数据 ，2个分区 */
    pBlockFilebuff = (HI_CHAR *)malloc((STOR_FILE_BLK_NUMS*2)*sizeof(STOR_BLK_IDX_ENTRY_T));
    if (NULL == pBlockFilebuff)
    {
        AV_PRINT("APP_GetChanIndexHeadData");
        return HI_FAILURE;
    }

    memset((HI_CHAR*)&stFileTimeSeg, 0 , sizeof(stFileTimeSeg));
    /* 根据文件类型进行处理 */
    if ((APP_PLAYBACK_BOTH_FIRST == iFileStatue) ||
            (APP_PLAYBACK_ONLY_FIRST == iFileStatue) ||
            (APP_PLAYBACK_BOTH_FIRST_SECOND == iFileStatue) ||
            (APP_PLAYBACK_ONLY_SECOND == iFileStatue))
    {
        memset(cFileName, 0, sizeof(cFileName));
        if (APP_PLAYBACK_ONLY_SECOND == iFileStatue)
        {
            sprintf(cFileName, "%s%d%s%s","/mnt/a", pstPart->s32SecondPart+5,"/","index00.tmm");
        }
        else
        {
            sprintf(cFileName, "%s%d%s%s","/mnt/a",pstPart->s32FirstPart+5,"/","index00.tmm");
        }

        AV_PRINT( " cFileName=%s", cFileName);

        pFile = fopen(cFileName, "rb");
        if (NULL == pFile)
        {
            AV_PRINT( " cFileName=%s", cFileName);
            if (NULL != pBlockFilebuff)
            {
                free(pBlockFilebuff);
                pBlockFilebuff = NULL;
            }

            return HI_FAILURE;
        }


        s32Ret = APP_GetOneChnAllBlockInfo(pFile, pBlockFilebuff, &stBlkIdxHead, iChn);
        if (HI_SUCCESS != s32Ret)
        {
            AV_PRINT( " APP_GetChanIndexHeadData");
            if (NULL != pFile)
            {
                fclose(pFile);
            }
            if (NULL != pBlockFilebuff)
            {
                free(pBlockFilebuff);
                pBlockFilebuff = NULL;
            }

            return HI_FAILURE;
        }

        iFileBlockCnt = iFileBlockCnt + stBlkIdxHead.block_total_num;
        s32FirstPartBlockCnt = iFileBlockCnt;

        AV_PRINT( " iFileBlockCnt=%d", iFileBlockCnt);

        if (NULL != pFile)
        {
            fclose(pFile);
            pFile = NULL;
        }

    }
    else
    {
        AV_PRINT( " APP_GetChanIndexHeadData,iFileStatue=%d",iFileStatue);
    }

    if (APP_PLAYBACK_BOTH_FIRST_SECOND == iFileStatue)
    {
        memset(cFileName, 0, sizeof(cFileName));
        sprintf(cFileName, "%s%d%s%s","/mnt/a", pstPart->s32SecondPart+5,"/","index00.tmm");
        pFile = fopen(cFileName, "rb");
        if (NULL == pFile)
        {
            AV_PRINT( " cFileName=%s", cFileName);
            if (NULL != pBlockFilebuff)
            {
                free(pBlockFilebuff);
                pBlockFilebuff = NULL;
            }

            return HI_FAILURE;
        }

        s32Ret = APP_GetOneChnAllBlockInfo(pFile,
                                           pBlockFilebuff+(stBlkIdxHead.block_total_num*sizeof(STOR_BLK_IDX_ENTRY_T)),
                                           &stBlkIdxHead, iChn);
        if (HI_SUCCESS != s32Ret)
        {
            AV_PRINT( " APP_GetChanIndexHeadData");
            if (NULL != pFile)
            {
                fclose(pFile);
            }

            if (NULL != pBlockFilebuff)
            {
                free(pBlockFilebuff);
                pBlockFilebuff = NULL;

            }

            return HI_FAILURE;
        }

        iFileBlockCnt+= stBlkIdxHead.block_total_num;
        AV_PRINT( "iFileBlockCnt=%d", iFileBlockCnt);

        if (NULL != pFile)
        {
            fclose(pFile);
        }
    }

    if ((APP_PLAYBACK_BOTH_FIRST == iFileStatue) ||
            (APP_PLAYBACK_ONLY_FIRST == iFileStatue) ||
            (APP_PLAYBACK_BOTH_FIRST_SECOND == iFileStatue))
    {
        for ( i = iFirstIndex; (i < iFileBlockCnt)&&(iFileCnt < SF_MAX_NUM); i++ )
        {
            memcpy(pTmp ,pBlockFilebuff + i*sizeof(STOR_BLK_IDX_ENTRY_T),sizeof(STOR_BLK_IDX_ENTRY_T));
            memset(cFileName, 0, sizeof(cFileName));
            /*出现了开始时间和结束时间一样的文件屏蔽掉，不给搜索显示，*/
            if (pTmp->end_time == pTmp->start_time)
            {
//                AV_PRINT("(start == end,current write file)pTmp->start_time=%u, pTmp->end_time=%u,",
//                    pTmp->start_time ,pTmp->end_time);
                continue;
            }

            if (((HI_U32)pstSearchFile->tStartTime < pTmp->end_time) && ((HI_U32)pstSearchFile->tEndTime > pTmp->start_time))
            {
                if (i < s32FirstPartBlockCnt)
                {
                    sprintf(cFileName, "%s%d%s%s%s","/mnt/a", pstPart->s32FirstPart+5,"/",pTmp->blockname, ".tmm");
                    memcpy(&pstSFList->stFileName[iFileCnt].szFileName[0], cFileName, FILE_NAME_LEN);
                }
                else
                {
                    sprintf(cFileName, "%s%d%s%s%s","/mnt/a", pstPart->s32SecondPart+5,"/",pTmp->blockname, ".tmm");
                    memcpy(&pstSFList->stFileName[iFileCnt].szFileName[0], cFileName, FILE_NAME_LEN);
                }
                HI3521_ConvertTime2LocaltimeStr2("search time",(time_t)(pTmp->start_time),(time_t)(pTmp->end_time));
                HI3521_ConvertTime2LocaltimeStr2("rec file time",(time_t)(pstSearchFile->tStartTime),(time_t)(pstSearchFile->tEndTime));
                stFileTimeSeg.tEndTime = (time_t)(pTmp->end_time);
                stFileTimeSeg.tStartTime = (time_t)(pTmp->start_time);
                AV_PRINT("search file is %s.\n",cFileName);
                memcpy(&pstSFList->stFileTimeSeg[iFileCnt], &stFileTimeSeg, sizeof(stFileTimeSeg));
                pstSFList->stFileName[iFileCnt].s32FileType = iFileStatue;
                iFileCnt++;
            }
        }
    }
    else if (APP_PLAYBACK_ONLY_SECOND == iFileStatue)
    {
        iFirstIndex = 0;
        for (i = iFirstIndex; (i < iFileBlockCnt) && (iFileCnt < SF_MAX_NUM); i++)
        {
            memcpy(pTmp ,pBlockFilebuff + i*sizeof(STOR_BLK_IDX_ENTRY_T),sizeof(STOR_BLK_IDX_ENTRY_T));
            memset(cFileName, 0, sizeof(cFileName));

            /*出现了开始时间和结束时间一样的文件屏蔽掉，不给搜索显示，*/
            if (pTmp->end_time == pTmp->start_time)
            {
                AV_PRINT("(start == end,current write file)pTmp->start_time=%u, pTmp->end_time=%u,",
                         pTmp->start_time ,pTmp->end_time);
                continue;
            }
            if (((HI_U32)pstSearchFile->tStartTime < pTmp->end_time) && ((HI_U32)pstSearchFile->tEndTime > pTmp->start_time))
            {
                sprintf(cFileName, "%s%d%s%s%s","/mnt/a", pstPart->s32SecondPart+5,"/",pTmp->blockname, ".tmm");
                memcpy(&pstSFList->stFileName[iFileCnt].szFileName[1], cFileName, FILE_NAME_LEN);

                stFileTimeSeg.tEndTime = pTmp->end_time;
                stFileTimeSeg.tStartTime = pTmp->start_time;
                memcpy(&pstSFList->stFileTimeSeg[iFileCnt], &stFileTimeSeg, sizeof(stFileTimeSeg));
                pstSFList->stFileName[iFileCnt].s32FileType = iFileStatue;
                iFileCnt++;
            }
        }
    }

    pstSFList->s32FileSegNum = iFileCnt;

    /*搜索出来的文件片段排序，更方便的兼容SD卡的搜索*/
    APP_SortSearchFileSeg(pstSFList);
    if (NULL != pBlockFilebuff)
    {
        free(pBlockFilebuff);
        pBlockFilebuff = NULL;

    }

    return HI_SUCCESS;

}


/*****************************************************************************
 函 数 名  : APP_ChkFileForm
 功能描述  : 获取文件的形态，夸分区和不夸分区等
 输入参数  : HI_S32 iSearchFileForm
             HI_S32 iFriPart
             HI_S32 iSecPart
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年8月13日
    作    者   : zoulq
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 APP_ChkFileForm(HI_S32 iSearchFileForm, HI_S32 iFriPart, HI_S32 iSecPart)
{
    HI_S32 iFileForm = 0;

    AV_PRINT( "  iFriPart =%d, iSecPart=%d", iFriPart, iSecPart);

    if (iSearchFileForm)
    {
        /* 判断文件构成形式 */
        /*****************************************
        文件构成形式
           文件1                文件2
         ____________       ____________
        |            |     |            |
        |____________|     |____________|

         only first    0x30  0011 0000B
               __________
              |__________|

         only second   0x0C  0000 1100B
                         ________
                        |________|

         both first second  0x3C   0011 1100B
               __________________
              |__________________|

         both first    0x60    0110 0000B
             _____
            |_____|

        ******************************************/
        if (0x01 == (iSearchFileForm&0x03))
        {
            iFileForm = APP_PLAYBACK_ONLY_FIRST;
        }
        else if (0x02 == (iSearchFileForm&0x03))
        {
            iFileForm = APP_PLAYBACK_ONLY_SECOND;
        }
        else if (0x03 == (iSearchFileForm&0x03))
        {
            if (iFriPart == iSecPart)
            {
                iFileForm = APP_PLAYBACK_BOTH_FIRST;
            }
            else
            {
                iFileForm = APP_PLAYBACK_BOTH_FIRST_SECOND;
            }
        }
    }
    else
    {
        AV_PRINT("APP_ChkFileForm,iSearchFileForm = 0x%x", iSearchFileForm);
        return HI_FAILURE;
    }

    return iFileForm;
}


/*****************************************************************************
 函 数 名  : APP_GetRecFileStat2
 功能描述  : 获取函数的形态，只有两种，1种夸分区和不夸分区
 输入参数  : time_t tStartTime
             time_t tEndTime
             HI_S32 *piFileStatue
             HI_S32 Chn
             REC_PLAYBACK_PART_T *pstPlaybackPart
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年9月27日
    作    者   : zoulq
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 APP_GetRecFileStat2(time_t tStartTime, time_t tEndTime,HI_S32 *piFileStatue, HI_S32 Chn,
                           REC_PLAYBACK_PART_T *pstPlaybackPart)
{
    HI_S32 s32Ret = 0;
    HI_S32 i = 0;
    HI_S32 iSearchFileForm = 0;
    HI_S32 iTmpForm = 0;
    HI_S32 m = -1;
    HI_S32 n = -1;
    HI_S32 s32CurPart = 0;
    HI_S32 iCurrOverwritePart = -1; /*当前录像循环写到的分区 */
    bool bOverWrite = FALSE;
    HI_CHAR szPartFileIndex[32]= {0};
    time_t ulNewestTime[APP_DISK_PART_NUM];
    time_t ulOldestTime[APP_DISK_PART_NUM];
    time_t ulNewestTimeTmp[APP_DISK_PART_NUM + 1];
    time_t ulOldestTimeTmp[APP_DISK_PART_NUM + 1];

    HI_S32 iMatchPartNums = 0;  /*找到的匹配的分区个数，HDD情况下每个硬盘的个数比较多，搜索的录像时间最多夸2个分区 */

    memset(ulNewestTime, 0, sizeof(ulNewestTime));
    memset(ulOldestTime, 0, sizeof(ulOldestTime));
    memset(ulNewestTimeTmp, 0, sizeof(ulNewestTimeTmp));
    memset(ulOldestTimeTmp, 0, sizeof(ulOldestTimeTmp));

    /* 获取各个分区该通道最老和最新的文件起始时间
       每一个分区a满了之后，切到下一个分区b，a分区的NEW和OLD时间会进行相应更新，*/
    for ( i = 0; i < APP_DISK_PART_NUM; i++ )
    {
        memset(szPartFileIndex, 0, sizeof(szPartFileIndex));
        sprintf(szPartFileIndex, "%s%d%s%s","/mnt/a",i+5,"/","index00.tmm");

        s32Ret = STOR_GetDiskNewestOldestTime(szPartFileIndex, Chn, &ulNewestTime[i], &ulOldestTime[i]);
        if (HI_SUCCESS != s32Ret)
        {
            AV_PRINT("STOR_GetDiskNewestOldestTime ERROR,FileindexName = %s", szPartFileIndex);

            return HI_FAILURE;
        }

        HI3521_ConvertTime2LocaltimeStr2(szPartFileIndex, ulOldestTime[i], ulNewestTime[i]);
    }

    /* 获取当前使用硬盘分区从0开始 */
    s32Ret = STOR_GetRecPartId();
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT("STOR_GetRecPartId failed");
        return HI_FAILURE;
    }

    /*基于录像写文件方式，我们现在来将录像写入进行一个情况划分，
      且假设搜索的文件只会存在夸2个分区的情况，
      1. 顺序录像无覆盖模式,
      2. 录像满了，覆盖模式，*/
    bOverWrite = STOR_IsRecOverWrite();
    /*非覆盖模式 直接每个分区按照时间做比较 */
    if (!bOverWrite)
    {
        for (i = 0; i < APP_DISK_PART_NUM; i++)
        {
            AV_PRINT("tStartTime(%d), tEndTime(%d), ulNewestTime[%d](%d), ulOldestTime[%d](%d)",
                     (HI_S32)tStartTime, (HI_S32)tEndTime, i, (HI_S32)ulNewestTime[i], i, (HI_S32)ulOldestTime[i]);
            if (tStartTime < ulNewestTime[i]&& tEndTime > ulOldestTime[i])
            {
                iMatchPartNums++;
                if(iMatchPartNums == 1)
                {
                    iTmpForm = iTmpForm|0x01;
                    pstPlaybackPart->s32FirstPart = i;
                    m = i;
                }
                else if(iMatchPartNums == 2)
                {
                    iTmpForm = iTmpForm|0x02;
                    pstPlaybackPart->s32SecondPart = i;
                    n = i;
                }

                AV_PRINT("prt=%d,tStartTime = %d,tEndTime = %d, ulNewestTime=%d, ulOldestTime = %d",
                         i, (HI_S32)tStartTime, (HI_S32)tEndTime, (HI_S32)ulNewestTime[i], (HI_S32)ulOldestTime[i]);
                /*寻找到跨分区情况退出 */
                if(iMatchPartNums == 2)
                {
                    break;
                }
            }
        }

        iSearchFileForm = APP_ChkFileForm(iTmpForm, m, n);
    }
    else
    {
        for(i = 0; i < APP_DISK_PART_NUM; i++)
        {
            /*判断当前操作覆盖的分区到哪里了 */
            if ((APP_DISK_PART_NUM -1 )!= i-1)
            {
                if (ulNewestTime[i] > ulNewestTime[i+1])
                {
                    iCurrOverwritePart = i;
                    break;
                }
            }
            else
            {
                if (ulOldestTime[i] < ulOldestTime[i-1])
                {
                    iCurrOverwritePart = i;
                }
            }
        }

        if (iCurrOverwritePart >= APP_DISK_PART_NUM)
        {
            AV_PRINT("APP_GetRecodeFileStatue, iCurrOverwritePart = %d", iCurrOverwritePart);
            return HI_FAILURE;
        }

        s32CurPart = iCurrOverwritePart;
        AV_PRINT("iCurrOverwritePart(%d)", s32CurPart);
        ulOldestTimeTmp[0] = 0;
        ulNewestTimeTmp[0] = ulOldestTime[s32CurPart];
        for ( i = 1; i < APP_DISK_PART_NUM; i++ )
        {
            ulOldestTimeTmp[i] = ulOldestTime[(s32CurPart + i)%APP_DISK_PART_NUM];
            ulNewestTimeTmp[i] = ulNewestTime[(s32CurPart + i)%APP_DISK_PART_NUM];
        }

        /*当前覆盖分区的前个分区的最新时间到覆盖分区的最新时间构成一个第5时间段 */
        ulOldestTimeTmp[4] = ulNewestTime[(s32CurPart + 3)%APP_DISK_PART_NUM];
        ulNewestTimeTmp[4] = ulNewestTime[(s32CurPart + 4)%APP_DISK_PART_NUM];

        ulNewestTimeTmp[0] = ulOldestTimeTmp[1]; /* 0 的时间从最后和1的最老连接在一起*/
        for ( i = 0; i < APP_DISK_PART_NUM + 1; i++ )
        {
            HI3521_ConvertTime2LocaltimeStr2((HI_CHAR *)"part time", (HI_S32)ulOldestTimeTmp[i], (HI_S32)ulNewestTimeTmp[i]);
            if (tStartTime < ulNewestTimeTmp[i]&& tEndTime > ulOldestTimeTmp[i])
            {
                iMatchPartNums++;
                if(iMatchPartNums == 1)
                {
                    iTmpForm = iTmpForm|0x01;
                    pstPlaybackPart->s32FirstPart = (iCurrOverwritePart+i)%APP_DISK_PART_NUM;
                    m = i;
                }
                else if(iMatchPartNums == 2)
                {
                    iTmpForm = iTmpForm|0x02;
                    pstPlaybackPart->s32SecondPart = (iCurrOverwritePart+i)%APP_DISK_PART_NUM;
                    n = i;
                }

                AV_PRINT("prt = %d, tStartTime = %d, tEndTime = %d, ulNewestTime=%d, ulOldestTime = %d",
                         i, (HI_S32)tStartTime, (HI_S32)tEndTime, (HI_S32)ulNewestTimeTmp[i], (HI_S32)ulOldestTimeTmp[i]);
                /*寻找到跨分区情况退出 */
                if(iMatchPartNums == 2)
                {
                    break;
                }
            }
        }

        /*覆盖情况下第4分区和第1分区 都有的时候要进行一个交换  其它情况下正常不用交换 */
        if(pstPlaybackPart->s32FirstPart == 0 && pstPlaybackPart->s32SecondPart == APP_DISK_PART_NUM-1)
        {
            SWAP(pstPlaybackPart->s32FirstPart, pstPlaybackPart->s32SecondPart);
        }
        iSearchFileForm = APP_ChkFileForm(iTmpForm, m, n);
    }

    if (HI_FAILURE == iSearchFileForm)
    {
        AV_PRINT("APP_ChkFileForm failed");
        return HI_FAILURE;
    }

    *piFileStatue = iSearchFileForm;
    AV_PRINT("iSearchFileForm = 0x%x", iSearchFileForm);

    return HI_SUCCESS;

}

/*****************************************************************************

 函 数 名  : APP_PlayFileByNetThread
 功能描述  : 客户端按照文件进行回放录像线程
 输入参数  : HI_VOID *pArg
 输出参数  : 无
 返 回 值  : HI_VOID

 修改历史      :
  1.日    期   : 2014年2月26日
    作    者   : zhangjinlei
    修改内容   : 新生成函数

*****************************************************************************/
HI_VOID *APP_PlayFileByNetThread(HI_VOID *pArg)
{
    HI_S32 s32Ret = 0;
    HI_S32 i = 0;
    FILE *pFile = NULL;
    HI_U32 iCurrentOffset;
    HI_U32 iEndOffset;
    HI_S32 iTmpLen = 0;
    HI_S32 iStartChn = 0;
    bool bPausePlay = false;  /*播放暂停标志 */
    FRAME_HEAD_T  struFrameHead;
    FRAME_HEAD_T *pstFrameHead = &struFrameHead;
    STOR_FILE_BLK_HEAD_T stFileBlockHead;
    SF_NAME_ST *pstFileName = NULL;
    APP_PLAYCTRL_MSGQ_ST stPlayCtrlMsgQ;
    APP_NET_PLAY_CTRL_ST struNetPlayCtrl;
    HI_CHAR cRecShareBuf[1024*300];

    HI_S32 s32DiskStat = 0;
    HI_S32 s32RecStat = 0;
    HI_S32 s32DiskEvent = 0;
    RW_LOCK_T *pDiskEventLock = NULL;
    RW_LOCK_T *pDiskStatLock = NULL;
    RW_LOCK_T *pRecStatLock = NULL;

    HI3531_GetLock(LOCK_DISK_STAT_E, &pDiskStatLock);
    HI3531_GetLock(LOCK_REC_STAT_E, &pRecStatLock);
    HI3531_GetLock(LOCK_DISK_EVENT_E, &pDiskEventLock);
    if (NULL == pArg)
    {
        AV_PRINT( " Input NULL pointer");

        return NULL;
    }
    AV_PRINT("@%s pid is %d,tid%d", __FUNCTION__,getpid(),gettid());
    /* 用于等待录像数据写入到硬盘 */
    do
    {
        //usleep(500*1000);
        TM_USleep(0,500*1000);
        i++;

        AV_PRINT(" g_iRecodePlaybackStatus counter = %d", i);

        s32Ret = HI3531_GetLockStat(pRecStatLock, &s32RecStat);
        if (HI_SUCCESS != s32Ret)
        {
            AV_PRINT( "HI3531_GetLockStat err:%s", ERR_STR);
            return (HI_VOID *)HI_FAILURE;
        }

        if (!((APP_VIDEOREPLAY_START == s32RecStat) && (i < 20)))
        {
            break;
        }

    }
    while (1);

    if (i >= 20)
    {
        s32Ret = HI3531_SetLockStat(pRecStatLock, APP_VIDEOREPLAY_RUN);
        if (HI_SUCCESS != s32Ret)
        {
            AV_PRINT( "HI3531_GetLockStat err:%s", ERR_STR);
            return (HI_VOID *)HI_FAILURE;
        }
    }

    pstFileName = (SF_NAME_ST *)pArg;
    iStartChn = g_stNetPlayRecodeDump.iChnno;

    pFile = HI3521_OpenSF(pstFileName);

    /*获取当前时间点I帧信息在文件中偏移位置  */
    s32Ret = STOR_GetIFrameSeek(pFile, &iCurrentOffset, g_stNetPlayRecodeDump.tStartTime);
    if (HI_SUCCESS != s32Ret)
    {
        s32Ret = STOR_GetIFrameSeek(pFile, &iCurrentOffset, g_stNetPlayRecodeDump.tStartTime+1);
        if (HI_SUCCESS != s32Ret)
        {
            AV_PRINT( " fread error");

        }
    }

    s32Ret = STOR_GetVideoFileHeadInfo(pFile, &stFileBlockHead, PART_INDEX_SEEK_HEAD);
    if (HI_SUCCESS != s32Ret)
    {
        if(pFile!=HI_NULL)
        {
            fclose(pFile);
        }
        pFile=HI_NULL;

        return NULL;
    }

    iEndOffset = stFileBlockHead.frame_data_end_pos;
    memset((HI_CHAR*)&struNetPlayCtrl, 0, sizeof(APP_NET_PLAY_CTRL_ST));

    while (1)
    {
        HI3531_GetLockStat(pDiskStatLock, &s32DiskStat);
        HI3531_GetLockStat(pDiskEventLock, &s32DiskEvent);
        HI3531_GetLockStat(pRecStatLock, &s32RecStat);

        if (!((APP_DISK_MOUNT == s32DiskStat)
                && (APP_VIDEOREPLAY_RUN == s32RecStat)
                && (APP_RECODE_DUMP == (s32DiskEvent & APP_RECODE_DUMP))))
        {
            break;
        }

        memset(&stPlayCtrlMsgQ, 0, sizeof(stPlayCtrlMsgQ));

        //AV_PRINT("end addr  and cur is %ld--%ld\n",iCurrentOffset, iEndOffset);
        /*如果偏移到了文件数据最后一个 说明文件已经结束到最后一帧
          现在录像都录像都是一个文件一个文件来分配，可以这样考虑文件结尾
          如果后面写录像做成不浪费多个时间段共有一个录像文件的方式这里不能判断，只能根据时间戳来判断*/
        if (iCurrentOffset >= iEndOffset)
        {
            AV_PRINT("the last frame play over!!!");
            break;
        }

        /*非暂停模式下播放，*/
        if(!bPausePlay)
        {
            /*!!!!播放发现其实的时间和文件开始时间一致，但是结束的时间不一致，可以分析到最后一帧数据，
                写文件名的更新时间会存在问题 ?*/
            fseek(pFile, iCurrentOffset, SEEK_SET);
            iTmpLen = fread(pstFrameHead, 1, sizeof(FRAME_HEAD_T), pFile);
            if (sizeof(FRAME_HEAD_T) != iTmpLen)
            {
                AV_PRINT("fread error");
                break;
            }

            iCurrentOffset = iCurrentOffset+pstFrameHead->len+sizeof(FRAME_HEAD_T);

            /*读取该帧头后面的帧的数据长度pstFrameHead->len放到buf中，
            buf中需要放共享内存头结构和帧数据体*/
            memset(cRecShareBuf, 0, sizeof(cRecShareBuf));
            iTmpLen = fread(cRecShareBuf + sizeof(SHM_HEAD_T), 1, pstFrameHead->len, pFile);
            if ((iTmpLen < 0) || (pstFrameHead->len != (HI_U32)iTmpLen))
            {
                AV_PRINT("fread error");
                continue;
            }

            /* 将获取音视频数据进行解码 */
            if((pstFrameHead->type == DATA_VIDEO_IFRAME)||
                    (pstFrameHead->type == DATA_VIDEO_PFRAME))
            {
                s32Ret = APP_SndFrame2Shm(iStartChn , pstFrameHead, cRecShareBuf, 0, 0);
                if (HI_SUCCESS != s32Ret)
                {
                    AV_PRINT("Send to vdec error 0x%x", s32Ret);
                    s32Ret = STOR_GetIFrameSeek(pFile, &iCurrentOffset, g_stNetPlayRecodeDump.tStartTime+1);
                    if (HI_SUCCESS != s32Ret)
                    {
                        AV_PRINT( " fread error");
                    }
                    continue;
                }
                else
                {
                    //usleep(40*1000);
                    TM_USleep(0,40*1000);
                }

            }
            else if(pstFrameHead->type == DATA_AUDIO)
            {
            }

#if 0
            /* 判断该文件是否结束 */
            uiTimeTmp = ((pstFrameHead->nmk_time4 <<24)|(pstFrameHead->nmk_time3 <<16)|(pstFrameHead->nmk_time2 <<8)|pstFrameHead->nmk_time1);

            if (uiTimeTmp > uiTimeTmp1)
            {
                uiTimeTmp1 = uiTimeTmp;
                iSecond = uiTimeTmp;
            }
            else
            {
                if (uiTimeTmp < g_stRecodeDump.tStartTime)
                {
                    uiTimeTmp = g_stRecodeDump.tStartTime;
                }
            }

            if (uiTimeTmp >= g_stRecodeDump.tEndTime)
            {
                AV_PRINT(" uiTimeTmp=%ld, g_stRecLPB.tEndTime=%ld",
                         uiTimeTmp[i], g_stRecodeDump.tEndTime);
                ucPlaybackState = (ucPlaybackState &(0xFF^(0x01<<i)));
                AV_PRINT(" ucPlaybackState=0x%x",
                         ucPlaybackState);
            }
#endif
        }

    }

    HI3531_GetLockStat(pDiskEventLock, &s32DiskEvent);
    s32DiskEvent = (s32DiskEvent & (~APP_RECODE_DUMP));
    HI3531_SetLockStat(pDiskEventLock, s32DiskEvent);

    if(pFile!=HI_NULL)
    {
        fclose(pFile);
    }
    pFile=HI_NULL;


    APP_SendEos(iStartChn);

    return NULL;
}


/*****************************************************************************
 函 数 名  : APP_GetAllFrameDataLen
 功能描述  : 获取文件长度，除文件头之外的所有数据，包括音频和视频数据
 输入参数  : HI_VOID *pArg
 输出参数  : 无
 返 回 值  : HI_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年8月9日
    作    者   : zoulq
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 APP_GetAllFrameDataLen(SF_LIST_ST *pstSFList, SF_RSP_ST *pstSFRsp)
{
    HI_S32 s32Ret = 0;
    HI_S32 iFileSegment = 0;
    HI_S32 iTmp = 0;
//    HI_S32 iChan = 0;
    HI_U32 iCurrentOffset;
    HI_U32 iEndOffset;
    FILE *pFile = NULL;
//    FRAME_HEAD_T  struFrameHead;
//    FRAME_HEAD_T *pstFrameHead = NULL;
    STOR_FILE_BLK_HEAD_T stFileBlockHead;
    SF_TIMESEG_ST stFileTimeSegment;
    SF_TIMESEG_ST *pstFileTimeSeg = &stFileTimeSegment;
    SF_NAME_ST stSearchFileName;
    SF_NAME_ST *pstSFName = &stSearchFileName;
    HI_S32  iAllFrameInFoLen = 0;
//    HI_S32  itestoffset = 0;
//    pstFrameHead = &struFrameHead;

//    iChan = pstSFList->iChnno;
    iFileSegment = pstSFList->s32FileSegNum;

    for (iTmp = 0; iTmp < iFileSegment; iTmp++)
    {
        memcpy(pstSFName, &pstSFList->stFileName[iTmp], sizeof(SF_NAME_ST));
        memcpy(pstFileTimeSeg, &pstSFList->stFileTimeSeg[iTmp], sizeof(SF_TIMESEG_ST));

        pFile = HI3521_OpenSF(pstSFName);

        /*获取当前时间点I帧信息在文件中偏移位置  */
        s32Ret = STOR_GetIFrameSeek(pFile, &iCurrentOffset, pstFileTimeSeg->tStartTime);
        if (HI_SUCCESS != s32Ret)
        {
            s32Ret = STOR_GetIFrameSeek(pFile, &iCurrentOffset, pstFileTimeSeg->tStartTime+1);
            if (HI_SUCCESS != s32Ret)
            {
                AV_PRINT( " fread error");

            }
        }

        s32Ret = STOR_GetVideoFileHeadInfo(pFile, &stFileBlockHead, PART_INDEX_SEEK_HEAD);
        if (HI_SUCCESS != s32Ret)
        {
            if(pFile!=HI_NULL)
            {
                fclose(pFile);
            }
            pFile=HI_NULL;

            return HI_FAILURE;
        }

        iEndOffset = stFileBlockHead.frame_data_end_pos;
        //      itestoffset = stFileBlockHead.frame_data_start_pos;
        iAllFrameInFoLen = iEndOffset - iCurrentOffset;
        pstSFRsp->struSearchFile[iTmp].iFileLen = iAllFrameInFoLen;

        if(pFile!=HI_NULL)
        {
            fclose(pFile);
        }
        pFile=HI_NULL;

        AV_PRINT("iCurrentOffset(%d) iEndOffset(%d) iAllFrameInFoLen(%d)\n",
                 iCurrentOffset, iEndOffset, iAllFrameInFoLen);
    }

    return HI_SUCCESS;
}


/*****************************************************************************
 函 数 名  : HI3531_GetFileFromSFList
 功能描述  : HI3531_GetFileFromSFList
 输入参数  : APP_NET_PLAYFILE_REQ_ST *pstruNetPlayCmd 平台发送的请求播放命令
             SF_NAME_ST *pstFileName 查找到的文件
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年8月5日
    作    者   : zoulq
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 HI3531_GetFileFromSFList(APP_NET_PLAYFILE_REQ_ST *pstruNetPlayCmd,
                                SF_NAME_ST *pstFileName)
{
    HI_S32  i = 0;
    HI_S32  iPlayChn = 0;
    HI_S32  s32FileSegNum = 0;
    SF_TIMESEG_ST stSegTime;
    SF_LIST_ST *pstSFList;

    pstSFList = &g_struNetSearchFileList;
    stSegTime.tStartTime = pstruNetPlayCmd->tStartTime;
    stSegTime.tEndTime = pstruNetPlayCmd->tEndTime;

    iPlayChn = pstruNetPlayCmd->iChnno;
    s32FileSegNum = pstSFList->s32FileSegNum;

    if (NULL == pstruNetPlayCmd || NULL == pstFileName)
    {
        AV_PRINT("input NULL pointer");
        return HI_FAILURE;
    }

    if (iPlayChn != pstSFList->iChnno)
    {
        AV_PRINT("input play channo error");
        return HI_FAILURE;
    }

    if (s32FileSegNum > SF_MAX_NUM)
    {
        AV_PRINT("input NULL pointer");
        return HI_FAILURE;
    }

    for (i = 0; i < s32FileSegNum; i++)
    {
        if((stSegTime.tStartTime == pstSFList->stFileTimeSeg[i].tStartTime)
                && (stSegTime.tEndTime == pstSFList->stFileTimeSeg[i].tEndTime))
        {
            pstFileName->s32FileType = pstSFList->stFileName[i].s32FileType;
            memcpy(pstFileName->szFileName, pstSFList->stFileName[i].szFileName,
                   FILE_NAME_LEN);

            AV_PRINT("type is %d.\n", pstFileName->s32FileType);
            AV_PRINT("filename[0]: %s", (HI_CHAR *)&pstSFList->stFileName[i].szFileName);

        }
    }

    return HI_SUCCESS;
}


/*****************************************************************************
 函 数 名  : HI3531_GetSFListByChn
 功能描述  : 获取搜索通道的搜索到的文件列表，存放在列表中，文件现在对搜索限制，
             如果文件多了的话采用链表方式进行控制
 输入参数  : SF_REQ_CMD_ST *pstSFRsq  搜索命令
 输出参数  :
             SF_LIST_ST *pstSFList 搜索的文件列表

 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年8月5日
    作    者   : zoulq
    修改内容   : 新生成函数

*****************************************************************************/


/*****************************************************************************
 函 数 名  : APP_StartPlayRecFileByNet
 功能描述  : 通过网络远程进行回放录像,直接将数据丢入到共享缓存
 输入参数  : REC_PLAYBACK_T *pstPlayFileCmd 平台发送过来的回放录像命令
 输出参数  : 无
 返 回 值  :
            HI_FAILURE 命令执行失败
            HI_SUCCESS 执行成功
 修改历史      :
  1.日    期   : 2014年08月07日
    作    者   : zoulq
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 APP_StartPlayRecFileByNet(APP_NET_PLAYFILE_REQ_ST *pstPlayFileReq)
{
    HI_S32     s32Ret = HI_FAILURE;
    time_t  tStartTime;
    time_t  tEndTime;
    pthread_attr_t stPthreadAddr;
    pthread_t iNetPlayRecfileId = 0;
    SF_NAME_ST stFileName;
    HI_S32 s32RecStat = 0;
    HI_S32 s32DiskEvent = 0;
    RW_LOCK_T *pRecStatLock = NULL;
    RW_LOCK_T *pDiskEventLock = NULL;
    HI3531_GetLock(LOCK_DISK_EVENT_E, &pDiskEventLock);
    HI3531_GetLock(LOCK_REC_STAT_E, &pRecStatLock);


    if (NULL == pstPlayFileReq)
    {
        AV_PRINT( " input NULL pointer");

        return HI_FAILURE;
    }

    tStartTime = pstPlayFileReq->tStartTime;
    tEndTime = pstPlayFileReq->tEndTime;

    //判断时间的合理性，如果合理继续执行
    s32Ret = HI3531_ChkSearchTime(&tStartTime, &tEndTime, HI_FALSE);
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT( " searchfile timecheck failed");
        return HI_FAILURE;
    }

    HI3531_GetLockStat(pDiskEventLock, &s32DiskEvent);
    if (APP_RECODE_DUMP == (s32DiskEvent & APP_RECODE_DUMP))
    {
        AV_PRINT(" playing file now!!!");
        return HI_SUCCESS;
    }

    memset(&stFileName, 0, sizeof(stFileName));

    s32Ret = HI3531_GetFileFromSFList(pstPlayFileReq, &stFileName);
    if (HI_SUCCESS == s32Ret)
    {
        HDD_SetHddEvent(DISK_EVENT_REC_RPB, HI_TRUE);
        AV_PRINT( " g_iRecodePlaybackStatus s32DiskEvent = %d", HDD_GetHddEvent(DISK_EVENT_REC_RPB));

        HI3531_GetLockStat(pRecStatLock, &s32RecStat);
        if (APP_VIDEOSTATUS_RUN == s32RecStat)
        {
            HI3531_SetLockStat(pRecStatLock, APP_VIDEOREPLAY_START);
        }
        else
        {
            HI3531_SetLockStat(pRecStatLock, APP_VIDEOREPLAY_RUN);
        }

        memcpy(&g_stNetPlayRecodeDump, pstPlayFileReq, sizeof(g_stNetPlayRecodeDump));
        /* 创建解码线程 */
        pthread_attr_init(&stPthreadAddr);
        pthread_attr_setdetachstate(&stPthreadAddr, PTHREAD_CREATE_DETACHED);

        s32Ret=pthread_create(&iNetPlayRecfileId,&stPthreadAddr, APP_PlayFileByNetThread, &stFileName);
        if(s32Ret != 0)
        {
            AV_PRINT( " err create APP_PlayFileByNetThread!");

            return HI_FAILURE;
        }

        AV_PRINT(" APP_StartPlayRecFileByNet success!");
    }
    else
    {
        AV_PRINT( " APP_StartPlayRecFileByNet error!");
    }

    return HI_SUCCESS;
}


/*****************************************************************************
 函 数 名  : APP_StopPlayRecFileByNet
 功能描述  : 停止远程客户端播放回放录像文件
 输入参数  : HI_VOID
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年8月7日
    作    者   : zoulq
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 APP_StopPlayRecFileByNet(HI_VOID)
{
    HI_S32 s32DiskEvent = 0;
    RW_LOCK_T *pDiskEventLock = NULL;
    HI3531_GetLock(LOCK_DISK_EVENT_E, &pDiskEventLock);

    HI3531_GetLockStat(pDiskEventLock, &s32DiskEvent);
    s32DiskEvent = (s32DiskEvent & (~APP_RECODE_DUMP));
    HI3531_SetLockStat(pDiskEventLock, s32DiskEvent);
    return HI_SUCCESS;
}


/*****************************************************************************
 函 数 名  : APP_VideoDecThread
 功能描述  : 录像本地回放解码线程
 输入参数  : HI_VOID *pArg
 输出参数  : 无
 返 回 值  : HI_VOID

 修改历史      :
  1.日    期   : 2013年10月25日
    作    者   : zhangjinlei
    修改内容   : 新生成函数

*****************************************************************************/
HI_VOID *APP_netVideoPlayThread(HI_VOID *pArg)
{
    HI_S32 i = 0;
    HI_S32 s32Ret = 0;
    HI_S32 iTmpAddr = 0;
    HI_S32 iStartChn = 0;
    HI_S32 iTmpLen = 0;
    HI_U8 ucPlaybackState = 0;
    HI_U32 uiFileAddr[APP_VDEC_CHNCNT];
    HI_U32 uiFarmeEndAddr[APP_VDEC_CHNCNT];
    HI_S32 iSecond[APP_VDEC_CHNCNT];
//    HI_S32 iFileLen[APP_VDEC_CHNCNT];
    HI_S32 iFileCnt[APP_VDEC_CHNCNT]= {0};
    FILE *file[APP_VDEC_CHNCNT] = {0};
    HI_CHAR cFileName[APP_VDEC_CHNCNT][96];
    long uiTimeTmp[APP_VDEC_CHNCNT] = {0};
    long uiTimeTmp1[APP_VDEC_CHNCNT] = {0};

    HI_CHAR cRecShareBuf[1024*300];
    FRAME_HEAD_T  stFrameHead;
    FRAME_HEAD_T *pstFrameHead = &stFrameHead;
    STOR_FILE_BLK_HEAD_T stFileBlockHead;
    static HI_S32  ipFrameTimes[4] = {0};  //P帧2个发一次
    static HI_S32  pFrameCnt[4] = {0};
    /*单通道回放测试的时候只需要添加这么宏即可 */
    HI_S32  iAllPlayChanno = 4;
    HI_S32 s32DiskStat = 0;
    HI_S32 s32RecStat = 0;
    HI_S32 s32DiskEvent = 0;
    RW_LOCK_T *pDiskEventLock = NULL;
    RW_LOCK_T *pDiskStatLock = NULL;
    RW_LOCK_T *pRecStatLock = NULL;
    HI3531_GetLock(LOCK_DISK_STAT_E, &pDiskStatLock);
    HI3531_GetLock(LOCK_REC_STAT_E, &pRecStatLock);
    HI3531_GetLock(LOCK_DISK_EVENT_E, &pDiskEventLock);
    AV_PRINT("@%s pid is %d,tid%d", __FUNCTION__,getpid(),gettid());
    /* 用于等待录像数据写入到硬盘 */
    while (1)
    {
        //usleep(500*1000);
        TM_USleep(0,500*1000);
        i++;
        AV_PRINT( " APP_netVideoPlayThread counter = %d", i);

        HI3531_GetLockStat(pDiskEventLock, &s32DiskEvent);
        if (!((APP_RECODE_RUN == (s32DiskEvent&APP_RECODE_RUN)) && (i < 20)))
        {
            break;
        }
    }

    if (i >= 20)
    {
        HI3531_SetLockStat(pRecStatLock, APP_VIDEOREPLAY_RUN);
    }

    /*为了兼容以前的本地回放版本，
      0x01 前4路同步回放  0x02后4路同步回放 */
    if (PLAY_BACK_1TO4CHN == g_stRecLPB.s32Cmd)
    {
        ucPlaybackState = (g_s32PBChnStat&0x0F);
        for(i=0; i<iAllPlayChanno; i++)
        {
            if (g_s32PBChnStat&(0x01<<i))
            {
                iStartChn = i;

                break;
            }
        }
    }
    else if (PLAY_BACK_4TO8CHN == g_stRecLPB.s32Cmd)
    {
        ucPlaybackState = (g_s32PBChnStat&0xF0);
        for(i=APP_VDEC_CHNCNT/2; i<APP_VDEC_CHNCNT; i++)
        {
            if (g_s32PBChnStat&(0x01<<i))
            {
                iStartChn = i;

                break;
            }
        }

    }
    else
    {
        AV_PRINT( " APP_StartVideoVDEC err,CMD=%d", g_stRecLPB.s32Cmd);
    }

    /*默认前4个通道回放 */
    for(i=iStartChn; i<iAllPlayChanno + iStartChn; i++)
    {
        if (Cam_En_Flag&(0x01<<i))
        {
            iSecond[i] = (HI_S32)g_stRecLPB.tStartTime;

            memset(cFileName[i], 0, 96);
            AV_PRINT( " cFileName=%s, addr =%d, fileCnt=%d",
                      g_pcRecodeFile+APP_PLAYBACK_FILE_CNT*APP_PLAYBACK_FILE_LEN*i,
                      APP_PLAYBACK_FILE_CNT*APP_PLAYBACK_FILE_LEN*i,
                      g_iPlaybackFileCnt[i]);

            if (0 == g_iPlaybackFileCnt[i])
            {
                ucPlaybackState = (ucPlaybackState &(0xFF^(0x01<<i)));
                AV_PRINT( " ucPlaybackState=%d",ucPlaybackState);

                continue;
            }
            memcpy(cFileName[i], g_pcRecodeFile+APP_PLAYBACK_FILE_CNT*APP_PLAYBACK_FILE_LEN*i,
                   APP_PLAYBACK_FILE_LEN);
            AV_PRINT( " ucPlaybackState=%d, cFileName[i]=%s",
                      ucPlaybackState, cFileName[i]);

            if (!access(cFileName[i],F_OK))
            {
                AV_PRINT(" cFileName=%s",
                         cFileName[i]);
            }

            file[i] = fopen(cFileName[i], "rb");
            if (NULL == file[i])
            {
                ucPlaybackState = (ucPlaybackState &(0xFF^(0x01<<i)));
                AV_PRINT( " ucPlaybackState=%d, cFileName[i]=%s",
                          ucPlaybackState, cFileName[i]);
            }
            else
            {
                fseek(file[i], 0L, SEEK_END);
                //    iFileLen[i] = ftell(file[i]);
            }

        }
    }

    AV_PRINT( " ucPlaybackState=%d,iStartChn=%d",ucPlaybackState, iStartChn);

    /* 获取开始解码文件首地址 */
    for (i = iStartChn; i < iAllPlayChanno + iStartChn; i++)
    {
        if (((ucPlaybackState >> i)&0x01) != 0x01)
        {
            continue;
        }

        /* 当回放设置为两个文件时，并且第一个文件没有搜索到指定时间段数据，
           会直接返回失败。 */
        s32Ret = STOR_GetIFrameSeek(file[i], uiFileAddr+i, g_stRecLPB.tStartTime);
        if (HI_SUCCESS != s32Ret)
        {
            ucPlaybackState = (ucPlaybackState &(0xFF^(0x01<<i)));
            AV_PRINT( " ucPlaybackState=%d",ucPlaybackState);
            continue;
        }

        s32Ret = STOR_GetVideoFileHeadInfo(file[i], &stFileBlockHead, STOR_IDX_BLK_HEAD_SEEK);
        if (HI_SUCCESS != s32Ret)
        {
            ucPlaybackState = (ucPlaybackState &(0xFF^(0x01<<i)));
            AV_PRINT( " ucPlaybackState=%d",ucPlaybackState);
        }

        uiFarmeEndAddr[i] = stFileBlockHead.frame_data_end_pos;
    }

    HI3531_GetLockStat(pRecStatLock, &s32RecStat);

    memcpy(iFileCnt, g_iPlaybackFileCnt, sizeof(g_iPlaybackFileCnt));
    HI3531_GetLockStat(pDiskEventLock, &s32DiskEvent);

    /* 需要检查本地回放标志是否取消 */
    while (1)
    {
        HI3531_GetLockStat(pDiskStatLock, &s32DiskStat);
        HI3531_GetLockStat(pRecStatLock, &s32RecStat);
        HI3531_GetLockStat(pDiskEventLock, &s32DiskEvent);

        if (!(ucPlaybackState
                && (APP_VIDEOREPLAY_RUN == s32RecStat)
                && (APP_DISK_MOUNT == s32DiskStat)
                && (APP_RECODE_DUMP == (s32DiskEvent & APP_RECODE_DUMP))))
        {
            break;
        }

        i = iStartChn;
        for (i = iStartChn; i < iAllPlayChanno + iStartChn; i++)
        {
            if (((ucPlaybackState >> i)&0x01) != 0x01)
            {
                continue;
            }

            /* 该文件是否结束 */
            if (uiFileAddr[i] >= uiFarmeEndAddr[i])
            {
                /* 判断该通道是否结束 */
                iFileCnt[i]--;
                if (0 == iFileCnt[i])
                {
                    ucPlaybackState = (ucPlaybackState & (0xFF^(0x01<<i)));
                }
                else
                {
                    fclose(file[i]);

                    iTmpAddr = APP_PLAYBACK_FILE_CNT*APP_PLAYBACK_FILE_LEN*i +
                               (g_iPlaybackFileCnt[i] - iFileCnt[i])*APP_PLAYBACK_FILE_LEN;
                    memset(cFileName[i], 0, sizeof(cFileName[i]));
                    memcpy(cFileName[i], g_pcRecodeFile+iTmpAddr,
                           APP_PLAYBACK_FILE_LEN);
                    AV_PRINT("cFileName=%s, iFileCnt[i]=%d",cFileName[i], iFileCnt[i]);
                    if (!access(cFileName[i],F_OK))
                    {
                        AV_PRINT("cFileName=%s", cFileName[i]);
                    }

                    file[i] = fopen(cFileName[i], "rb");
                    if (NULL == file[i])
                    {
                        ucPlaybackState = (ucPlaybackState &(0xFF^(0x01<<i)));
                    }
                    else
                    {
                        fseek(file[i], 0L, SEEK_END);
                        //       iFileLen[i] = ftell(file[i]);
                    }

                    s32Ret = STOR_GetIFrameSeek(file[i], uiFileAddr+i, g_stRecLPB.tStartTime);
                    if (HI_SUCCESS != s32Ret)
                    {
                        ucPlaybackState = (ucPlaybackState &(0xFF^(0x01<<i)));
                        continue;
                    }
                    AV_PRINT("uiFileAddr+i=%d", uiFileAddr[i]);

                    s32Ret = STOR_GetVideoFileHeadInfo(file[i], &stFileBlockHead, STOR_IDX_BLK_HEAD_SEEK);
                    if (HI_SUCCESS != s32Ret)
                    {
                        ucPlaybackState = (ucPlaybackState &(0xFF^(0x01<<i)));
                    }

                    uiFarmeEndAddr[i] = stFileBlockHead.frame_data_end_pos;
                    AV_PRINT("uiFileAddr+i=%d", uiFarmeEndAddr[i]);
                }
            }

            fseek(file[i],uiFileAddr[i],SEEK_SET);

#if 0
            if ((iSecond[i] >= iSecond[iStartChn]) && (i != iStartChn))
            {
                /* 如果第一通道已经播放完成，则退出远程预览 */
                if (((ucPlaybackState >> iStartChn)&0x01) != 0x01)
                {
                    AV_PRINT(" ucPlaybackState=0x%x", ucPlaybackState);
                    ucPlaybackState = 0;
                }
                continue;
            }
#endif

            iTmpLen = fread(pstFrameHead, 1, sizeof(FRAME_HEAD_T), file[i]);
            if (sizeof(FRAME_HEAD_T) != iTmpLen)
            {
                AV_PRINT("fread error");
                break;
            }

            /*读取该帧头后面的帧的数据长度pstFrameHead->len放到buf中，
            buf中需要放共享内存头结构和帧数据体,这里平台不缓存模式，只能给帧数据平台接受到就播放了 */
            memset(cRecShareBuf, 0, sizeof(cRecShareBuf));

            /*下面代码保留后面可能需要发送帧头数据 */
#if 0
            pstFrameHead->u.gps_id = i;
            memcpy(cRecShareBuf + sizeof(SHM_HEAD_T), pstFrameHead, sizeof(FRAME_HEAD_T));
            iTmpLen = fread(cRecShareBuf + sizeof(SHM_HEAD_T)+sizeof(FRAME_HEAD_T), 1, pstFrameHead->len, file[i]);
#else
            iTmpLen = fread(cRecShareBuf + sizeof(SHM_HEAD_T), 1, pstFrameHead->len, file[i]);
#endif
            if ((iTmpLen < 0) || (pstFrameHead->len != (HI_U32)iTmpLen))
            {
                AV_PRINT("fread error");
                s32Ret = STOR_GetIFrameSeek(file[i], uiFileAddr + i, iSecond[i]+1);
                if (HI_SUCCESS != s32Ret)
                {
                    AV_PRINT("fread error iTmpLen=0x%x, ucPlaybackState=0x%x, i=%d",
                             iTmpLen, ucPlaybackState, i);
                    ucPlaybackState = (ucPlaybackState &(0xFF^(0x01<<i)));
                    AV_PRINT("fread error iTmpLen=0x%x, ucPlaybackState=0x%x, i=%d",
                             iTmpLen, ucPlaybackState, i);
                }

                iSecond[i] = iSecond[i] + 1;

                continue;
            }

            uiFileAddr[i] = uiFileAddr[i] + pstFrameHead->len + sizeof(FRAME_HEAD_T);;

            /* 将获取音视频数据进行解码 */
            if(pstFrameHead->type == DATA_VIDEO_IFRAME)
            {
                ipFrameTimes[i] = 0;
                /*send video-stream to unix-domain*/
#if 0
                s32Ret = APP_SndFrame2Shm(i , pstFrameHead, cRecShareBuf, sizeof(FRAME_HEAD_T), 0);
#else
                s32Ret = APP_SndFrame2Shm(i , pstFrameHead, cRecShareBuf, 0, 0);
#endif
                if (HI_SUCCESS != s32Ret)
                {
                    AV_PRINT( " Send to vdec error 0x%x", s32Ret);

                    s32Ret = STOR_GetIFrameSeek(file[i], uiFileAddr + i, iSecond[i] + 1);
                    if (HI_SUCCESS != s32Ret)
                    {
                        AV_PRINT( " fread error");

                    }
                    iSecond[i] = iSecond[i] + 1;

                    continue;
                }
                else
                {

                }
            }
            else if(pstFrameHead->type == DATA_VIDEO_PFRAME)
            {
                ipFrameTimes[i]++;
                pFrameCnt[i]++;
                if (1)
                    //if(ipFrameTimes[i] != 5)
                    //if(ipFrameTimes[i] == (1 + 1 * (pFrameCnt[i])))
                {
                    pFrameCnt[i]++;
                    /*send video-stream to unix-domain*/
#if 0
                    s32Ret = APP_SndFrame2Shm(i , pstFrameHead, cRecShareBuf, sizeof(FRAME_HEAD_T), 0);
#else
                    s32Ret = APP_SndFrame2Shm(i , pstFrameHead, cRecShareBuf, 0, 0);
#endif
                    if (HI_SUCCESS != s32Ret)
                    {
                        AV_PRINT( " Send to vdec error 0x%x", s32Ret);

                        s32Ret = STOR_GetIFrameSeek(file[i], uiFileAddr + i, iSecond[i] + 1);
                        if (HI_SUCCESS != s32Ret)
                        {
                            AV_PRINT( " fread error");

                        }
                        iSecond[i] = iSecond[i] + 1;

                        continue;
                    }
                    else
                    {
                        /*测试数据丢包 */
                        ipFrameTimes[i] = 0;
                    }
                }
                else
                {
                    ipFrameTimes[i] = 0;
                }
            }
            else if(pstFrameHead->type == DATA_AUDIO)
            {
                /*don't send audio-stream to unix-domain, now*/
                break;
            }

            /* 判断该文件是否结束 */
            uiTimeTmp[i] = ((pstFrameHead->nmk_time4 <<24)
                            | (pstFrameHead->nmk_time3 <<16)
                            | (pstFrameHead->nmk_time2 <<8)
                            | pstFrameHead->nmk_time1);

            if (uiTimeTmp[i] > uiTimeTmp1[i])
            {
                uiTimeTmp1[i] = uiTimeTmp[i];
                iSecond[i] = uiTimeTmp[i];
            }
            else
            {
                if (uiTimeTmp[i] < g_stRecLPB.tStartTime)
                {
                    uiTimeTmp[i] = g_stRecLPB.tStartTime;
                }
            }

            if (uiTimeTmp[i] >= g_stRecLPB.tEndTime)
            {
                AV_PRINT(" uiTimeTmp=%ld, g_stRecLPB.tEndTime=%ld",
                         uiTimeTmp[i], g_stRecLPB.tEndTime);
                ucPlaybackState = (ucPlaybackState &(0xFF^(0x01<<i)));
                AV_PRINT(" ucPlaybackState=0x%x",
                         ucPlaybackState);
            }

        }

        //usleep(20000);
        TM_USleep(0,20000);

    }

    AV_PRINT( " uiTimeTmp=%ld, g_stRecLPB.tEndTime=%ld",
              uiTimeTmp[i], g_stRecLPB.tEndTime);

    for(i=0; i<APP_VDEC_CHNCNT; i++)
    {
        if(file[i]!=HI_NULL)
        {
            fclose(file[i]);
        }
        file[i]=HI_NULL;

    }
    return (HI_VOID *)NULL;
}


/*****************************************************************************
 函 数 名  : APP_RecodePlayback
 功能描述  : 历史录像回放
 输入参数  : REC_PLAYBACK_T *pstPlaybackCMD
 输出参数  : 无
 返 回 值  :

 修改历史      :
  1.日    期   : 2013年10月23日
    作    者   : zhangjinlei
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 APP_RecodeNetPlayback(REC_PLAYBACK_T *pstPlaybackCMD)
{
    REC_PLAYBACK_T *pstPlayback = NULL;
    time_t ilPlayTime = 0;
    HI_S32 s32Ret = 0;
    pthread_attr_t stBzqSendAttr;
    pthread_t RecodeVenpthID = 0;
    HI_S32 i = 0;
    struct tm *tmp;
    HI_CHAR cTmp[2][REC_FILENAME_LEN];
    HI_S32 s32DiskEvent = 0;
    RW_LOCK_T *pDiskEventLock = NULL;
    RW_LOCK_T *pRecStatLock = NULL;
    HI3531_GetLock(LOCK_REC_STAT_E, &pRecStatLock);
    HI3531_GetLock(LOCK_DISK_EVENT_E, &pDiskEventLock);

    if (NULL == pstPlaybackCMD)
    {
        AV_PRINT( " input NULL pointer");

        return HI_FAILURE;
    }

    pstPlayback = pstPlaybackCMD;
    AV_PRINT( " startplay tStartTime=%ld,tEndTime=%ld, cmd is %d",
              pstPlayback->tStartTime, pstPlayback->tEndTime,pstPlayback->s32Cmd);

    /* 回放的模式  1 标示 0-4 ，2 标示 5-8 */
    if (APP_VIDEOSTATUS_IDLE == pstPlayback->s32Cmd)
    {
        HI3531_GetLockStat(pDiskEventLock, &s32DiskEvent);
        s32DiskEvent = (s32DiskEvent & (~APP_RECODE_DUMP));
        HI3531_SetLockStat(pDiskEventLock, s32DiskEvent);
        return HI_SUCCESS;
    }

    ilPlayTime = pstPlayback->tEndTime - pstPlayback->tStartTime;
    /* 如果回放时长大于一小时，对时间进行截取 */
    if (3600 < ilPlayTime)
    {
        pstPlayback->tEndTime = pstPlayback->tStartTime + 3600;
    }
    else if (ilPlayTime < 10)
    {
        AV_PRINT("too short,tStartTime=%ld,tEndTime=%ld",
                 pstPlayback->tStartTime, pstPlayback->tEndTime);

        return HI_SUCCESS;
    }

    HI3531_GetLockStat(pDiskEventLock, &s32DiskEvent);
    if (APP_RECODE_DUMP == (s32DiskEvent & APP_RECODE_DUMP))
    {
        return HI_SUCCESS;
    }

    g_pcRecodeFile = (HI_CHAR *)malloc(640*APP_VDEC_CHNCNT);
    if (NULL == g_pcRecodeFile)
    {
        AV_PRINT( " malloc memory failed!");

        return HI_SUCCESS;
    }

    HI3531_GetLockStat(pDiskEventLock, &s32DiskEvent);
    s32DiskEvent = (s32DiskEvent | APP_RECODE_DUMP);
    HI3531_SetLockStat(pDiskEventLock, s32DiskEvent);

    HI3531_SetLockStat(pRecStatLock, APP_VIDEOREPLAY_RUN);

    while (1)
    {
        //usleep(500*1000);
        TM_USleep(0,500*1000);
        i++;
        AV_PRINT( " g_iRecodePlaybackStatus counter = %d", i);

        HI3531_GetLockStat(pDiskEventLock, &s32DiskEvent);
        if (!((APP_RECODE_RUN == (s32DiskEvent&APP_RECODE_RUN)) && (i < 20)))
        {
            break;
        }
    }
    tmp = localtime(&pstPlaybackCMD->tStartTime);
    sprintf(cTmp[0],"%04d%02d%02d%02d%02d%02d",
            tmp->tm_year+1900, tmp->tm_mon+1, tmp->tm_mday, tmp->tm_hour, tmp->tm_min, tmp->tm_sec);
    tmp = localtime(&pstPlaybackCMD->tEndTime);
    sprintf(cTmp[1],"%04d%02d%02d%02d%02d%02d",
            tmp->tm_year+1900, tmp->tm_mon+1, tmp->tm_mday, tmp->tm_hour, tmp->tm_min, tmp->tm_sec);

    AV_PRINT( " input file name, file1=%s, file1=%s",
              cTmp[0], cTmp[1]);

    s32Ret = HI3531_SetLockStat(pRecStatLock, APP_VIDEOREPLAY_RUN);
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT( "HI3531_GetLockStat err:%s", ERR_STR);
        return HI_FAILURE;
    }

    s32Ret = APP_GetRecodeFileName(pstPlayback, g_pcRecodeFile, g_iPlaybackFileCnt, pstPlayback->s32Cmd);
    if (HI_SUCCESS == s32Ret)
    {
        HI3531_GetLockStat(pDiskEventLock, &s32DiskEvent);
        s32DiskEvent = (s32DiskEvent | APP_RECODE_DUMP);
        HI3531_SetLockStat(pDiskEventLock, s32DiskEvent);

        s32Ret = HI3531_SetLockStat(pRecStatLock, APP_VIDEOREPLAY_RUN);
        if (HI_SUCCESS != s32Ret)
        {
            AV_PRINT( "HI3531_GetLockStat err:%s", ERR_STR);
            return HI_FAILURE;
        }

        AV_PRINT( " create APP_VideoDecThread!");

        memcpy(&g_stRecLPB, pstPlayback, sizeof(g_stRecLPB));
        /* 创建解码线程 */
        pthread_attr_init(&stBzqSendAttr);
        pthread_attr_setdetachstate(&stBzqSendAttr, PTHREAD_CREATE_DETACHED);
        s32Ret=pthread_create(&RecodeVenpthID,&stBzqSendAttr, APP_netVideoPlayThread, NULL);
        /* END:   Modified by zhangjinlei, 2012/7/23   PN:防止内存泄漏.71047004 */
        if(s32Ret != 0)
        {
            AV_PRINT( " err create APP_VideoDecThread!");
            if (NULL != g_pcRecodeFile)
            {
                free(g_pcRecodeFile);
                g_pcRecodeFile = NULL;
            }

            return HI_FAILURE;
        }

        AV_PRINT(" APP_GetOldRecodeFile success!");
    }
    else
    {
        HDD_SetHddEvent(DISK_EVENT_REC_RPB, HI_FALSE);

        if (NULL != g_pcRecodeFile)
        {
            free(g_pcRecodeFile);
            g_pcRecodeFile = NULL;
        }
    }
    AV_PRINT(" APP_GetOldRecodeFile ret=%d!", s32Ret);

    return s32Ret;
}


HI_S32 HI3531_GetSFListByChn(SF_REQ_CMD_ST *pstSFRsq, SF_LIST_ST *pstSFList)
{
    HI_S32 s32Ret = 0;
    HI_S32 iFileForm = 0;
    HI_S32 iChn = 0;
    time_t tStartTime = 0;
    time_t tEndTime = 0;
    REC_PLAYBACK_PART_T stPlaybackPart;

    if ((NULL == pstSFRsq)||(NULL == pstSFList))
    {
        return HI_FAILURE;
    }

    iChn = pstSFRsq->iChnno;

    //调用原有接口，用来获取获取当前文件的形态
    tStartTime = pstSFRsq->tStartTime;
    tEndTime = pstSFRsq->tEndTime;


    s32Ret = APP_GetRecFileStat2(tStartTime, tEndTime, &iFileForm, iChn, &stPlaybackPart);
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT("GetRecodeFileStatue ERROR");

        return HI_FAILURE;
    }

    //没有获取到文件的形态，说明查找的文件类型不对，直接退出
    if (0 == iFileForm)
    {
        AV_PRINT("get iFileStatue ERROR");

        return HI_FAILURE;
    }
    AV_PRINT("get iFileForm=0x%x", iFileForm);

    s32Ret = APP_GetSearchFileTimeSeg(pstSFRsq, pstSFList,
                                      &stPlaybackPart,iChn,iFileForm);
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT("APP_GetSearchFileTimeSeg");

        return HI_FAILURE;
    }

    return HI_SUCCESS;

}

HI_VOID HI3521_InitSFList( HI_S32 chn)
{
    //每次搜索清空之前搜索的文件数据
    memset((HI_CHAR*)&g_stLocalSFList[chn], 0, sizeof(g_stLocalSFList));
    return;
}
HI_S32 HI3521_SFByTime2(void *pReq)
{
    REC_MONTH_T * pUiReq = (REC_MONTH_T *)pReq;
    HI_S32 iTmptime = 0;
    HI_U32 iStartTime = pUiReq->iStartTime;
    HI_U32 iEndTime = pUiReq->iEndTime;
    if ( iEndTime <= iStartTime)
    {
        AV_PRINT("search time error,tStartTime=%d,tEndTime=%d",iStartTime,iEndTime);
        return HI_FAILURE;
    }
    iTmptime = iEndTime - iStartTime;
    if ( LOCAL_SEARCH_MAX_TIME < iTmptime )
    {
        iEndTime = iStartTime + LOCAL_SEARCH_MAX_TIME;
        AV_PRINT("Over MaxTime(%d) StartTime:%d,New EndTime:%d",LOCAL_SEARCH_MAX_TIME,iStartTime,iEndTime);
    }
    char szPartFileIndex[64] = {0};
    int i = 0;
    int j = 0;
    int k = 0;
    STOR_CHN_IDX_INFO_T *pstChnIdxInfo = (STOR_CHN_IDX_INFO_T *)malloc(sizeof(STOR_CHN_IDX_INFO_T));
    for ( i = 0; i < APP_DISK_PART_NUM; i++ )
    {
        memset(szPartFileIndex, 0, sizeof(szPartFileIndex));
        sprintf(szPartFileIndex, "%s%d%s%s","/mnt/a",i+5,"/","index00.tmm");
        FILE* file = NULL;
        file = fopen(szPartFileIndex, "r+");
        if(file==NULL)
            continue;
        for(j=0; j<APP_VENC_CHNCNT; j++)
        {
            STOR_GetBlkIdxByChn(file, pstChnIdxInfo, j);
            for ( k = 0; k < pstChnIdxInfo->s32Counter && k<STOR_RECFILE_MAX_NUMS; k++ )
            {
                HI_S32 s32Index = (pstChnIdxInfo->s32OldestIndex+ k)%STOR_RECFILE_MAX_NUMS;
                HI_U32 start_time = pstChnIdxInfo->stBlkIdxList[s32Index].start_time;
                HI_U32 end_time = pstChnIdxInfo->stBlkIdxList[s32Index].end_time;
                if(end_time<iStartTime || start_time>iEndTime)
                {
                    continue;
                }
                if(start_time<=iStartTime&&end_time<iEndTime)
                {
                    struct tm *pTm = NULL;
                    pTm = localtime((time_t*)(&iStartTime));
                    pUiReq->recinfo[pTm->tm_mday - 1] = 1;
                    continue;
                }
                if(start_time>iStartTime&&end_time>=iEndTime)
                {
                    struct tm *pTm = NULL;
                    pTm = localtime((time_t*)(&iEndTime));
                    pUiReq->recinfo[pTm->tm_mday - 1] = 1;
                    continue;

                }
                if(iStartTime<start_time&&end_time<iEndTime)
                {
                    struct tm *pTm = NULL;
                    pTm = localtime((time_t*)(&end_time));
                    pUiReq->recinfo[pTm->tm_mday - 1] = 1;
                    pTm = localtime((time_t*)(&start_time));
                    pUiReq->recinfo[pTm->tm_mday - 1] = 1;
                    continue;
                }
            }
        }
        fclose(file);
    }
    free(pstChnIdxInfo);
    pUiReq->iEndTime = iEndTime;
    return HI_SUCCESS;
}
/*****************************************************************************
 函 数 名  : HI3531_SFByTime
 功能描述  : 根据文件类型和时间搜索
 输入参数  : SF_REQ_CMD_ST *pstSFReq
             SF_RSP_ST *pstSFRsp
             HI_BOOL bLocal
             HI_S32 s32RecType  0-- 1--alarmrec
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年6月4日
    作    者   : zoulq
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 HI3521_SFByTime(SF_REQ_CMD_ST *pstSFReq, SF_RSP_ST *pstSFRsp, HI_BOOL bLocal, HI_S32 s32RecType)
{
    HI_S32 s32Ret = HI_FAILURE;
    HI_S32 s32Chn = 0;
    time_t tStartTime = 0;
    time_t tEndTime = 0;

    SF_LIST_ST *pstSFList  = NULL;

    if ((NULL == pstSFReq) || (NULL == pstSFRsp))
    {
        AV_PRINT("input NULL pointer,p1=0x%x,p2=0x%x",(HI_U32)pstSFReq, (HI_U32)pstSFRsp);
        return HI_FAILURE;
    }

    s32Chn = pstSFReq->iChnno;
    pstSFRsp->iChnno = s32Chn;

    if (s32Chn > APP_VENC_CHNCNT || s32Chn < 0)
    {
        AV_PRINT("input invaild iChnno = %d", s32Chn);
        return HI_FAILURE;
    }

    /*报警录像 */
    if (s32RecType == 1)
    {
        pstSFList = &g_stAlarmRecBkSFList[s32Chn];
    }
    else
    {
        if (bLocal)
        {
            pstSFList = &g_stLocalSFList[s32Chn];
        }
        else
        {
            //每次搜索清空之前搜索的文件数据
            memset((HI_CHAR*)&g_struNetSearchFileList, 0, sizeof(g_struNetSearchFileList));
            pstSFList = &g_struNetSearchFileList;
        }
    }
    //检测搜索时间合法性
    tStartTime = pstSFReq->tStartTime;
    tEndTime = pstSFReq->tEndTime;
    s32Ret = HI3531_ChkSearchTime(&tStartTime, &tEndTime, 1);
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT("searchfile timecheck failed");
        return HI_FAILURE;
    }

    pstSFReq->tStartTime = tStartTime;
    pstSFReq->tEndTime = tEndTime;

    s32Ret = HI3531_GetSFListByChn(pstSFReq, pstSFList);
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT("get search filelist error");
        return HI_FAILURE;
    }

    if (0 == pstSFList->s32FileSegNum )
    {
        AV_PRINT("searchfile segnum is 0!!!!");
        return HI_FAILURE;
    }

    pstSFList->iChnno = s32Chn;

    AV_PRINT("HI3521_SFByTime s32Chn(%d),fileSegNum(%d)", pstSFList->iChnno, pstSFList->s32FileSegNum);

    pstSFRsp->s32FileSegNum = pstSFList->s32FileSegNum;
    memcpy((HI_CHAR*)pstSFRsp->struSearchFile, pstSFList->stFileTimeSeg, SF_MAX_NUM*sizeof(SF_TIMESEG_ST));
    s32Ret = APP_GetAllFrameDataLen(pstSFList, pstSFRsp);
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT("APP_GetAllFrameDataLen error.");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}


/* BEGIN: Added by mabuyun, 2015/6/10  DES:添加28181对接代码  PN:20150608 */
HI_S32 HI3531_SFByTime_V28181(SF_REQ_CMD_ST *pstSFReq, SF_LIST_ST *pstSFList, HI_BOOL bLocal, HI_S32 s32RecType)
{
    HI_S32 s32Ret = HI_FAILURE;
    HI_S32 s32Chn = 0;

    if ((NULL == pstSFReq) || (NULL == pstSFList))
    {
        AV_PRINT("input NULL pointer,p1=0x%x,p2=0x%x",(HI_U32)pstSFReq, (HI_U32)pstSFList);
        return HI_FAILURE;
    }

    s32Chn = pstSFReq->iChnno;
    pstSFList->iChnno = s32Chn;

    if (s32Chn > APP_VENC_CHNCNT || s32Chn < 0)
    {
        AV_PRINT("input invaild iChnno = %d", s32Chn);
        return HI_FAILURE;
    }

    s32Ret = HI3531_GetSFListByChn(pstSFReq, pstSFList);
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT("get search filelist error");
        return HI_FAILURE;
    }

    if (0 == pstSFList->s32FileSegNum )
    {
        AV_PRINT("searchfile segnum is 0!!!!");
        return HI_FAILURE;
    }

    pstSFList->iChnno = s32Chn;

    AV_PRINT("HI3531_SFByTime s32Chn(%d),fileSegNum(%d)", pstSFList->iChnno, pstSFList->s32FileSegNum);

    return HI_SUCCESS;
}


/*****************************************************************************
 函 数 名  : HI3531_LoaclPlayback
 功能描述  : 本地回放
 输入参数  : REC_PLAYBACK_T *pstLPB
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年6月4日
    作    者   : zoulq
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 HI3521_LoaclPlayback(REC_PLAYBACK_T *pstLPB)
{
    HI_S32 s32Ret = HI_SUCCESS;
    REC_PLAYBACK_T stPB;
    SF_RSP_ST stSFRsp;

    if (NULL == pstLPB)
    {
        AV_PRINT("input NULL pointer");
        return HI_FAILURE;
    }

    memcpy(&stPB, pstLPB, sizeof(REC_PLAYBACK_T));
    /*获取回放状态 */
    if (HDD_GetHddEvent(DISK_EVENT_REC_LPB))
    {
        AV_PRINT("play already start!!!");
        return HI_SUCCESS;
    }
#if 0
    s32Ret = HI3531_ChkSearchTime(&stPB.tStartTime, &stPB.tEndTime, HI_TRUE);
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT("searchfile timecheck failed");
        return HI_FAILURE;
    }

    HI3521_ConvertTime2LocaltimeStr2((HI_CHAR *)"ui LPB send time", stPB.tStartTime, stPB.tEndTime);
#endif
    /*本地搜索列表初始化 */
    HI3521_InitSFList(pstLPB->s32Cmd);

    s32Ret = HI3521_SFByTime((SF_REQ_CMD_ST*)&stPB, &stSFRsp, HI_TRUE, 0);
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT("HI3531_SFByTime error");
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

HI_VOID *HI3521_VdecbyChnThd(HI_VOID *pArg)
{
    FILE *pFile = NULL;
    HI_S32 s32Ret = HI_FAILURE;
    HI_U32 u32FrameSeek = 0;
    HI_U32 u32FrameEnd = 0;
    HI_S32 s32Chn = 0,logcalchn = 0;;
    HI_S32 s32FileCnt = 0;
    SF_NAME_ST stSFName;
    SF_LIST_ST *pstSFList = NULL;
    REC_PLAYBACK_T *pstPB = NULL;
    REC_PLAYBACK_T stTmpPB;
    STOR_FILE_BLK_HEAD_T stFileBlkHead;
    SF_PB_ST stSFPBarg;
    SF_PB_ST *pstSFPB = (SF_PB_ST *)pArg;
    memcpy(&stSFPBarg.stPB, &pstSFPB->stPB, sizeof(REC_PLAYBACK_T));
    pstPB = &stSFPBarg.stPB;
    s32Chn = pstSFPB->Chn;/*通道*/;
    pstPB->s32Cmd = s32Chn;
    logcalchn = s32Chn%4;
    memcpy(&stTmpPB, pstPB, sizeof(REC_PLAYBACK_T));
    HI3521_SignalPBCond(pstSFPB);
    AV_PRINT("@%s(%d) pid is %d,tid%d", __FUNCTION__,s32Chn,getpid(),gettid());
    AV_PRINT("###############chn : %d ###########",s32Chn);
    pstSFList = HI3521_GetLocalSFList(s32Chn);
    AV_PRINT("tid(%d) HI3531_VdecbyChnThd, chn(%d),s32FileSegNum(%d)", gettid(), s32Chn,pstSFList->s32FileSegNum);
    while (HDD_IsLPBRunning())
    {
        if (HI3521_GetdecPause(logcalchn))
        {
            //usleep(400000);
            TM_USleep(0,400000);
            AV_PRINT("STOP PLAY NOW");
            if (HI3521_GetPlayCtrlChn(logcalchn) == LPB_CTRL_PAUSE)
            {
                HI3521_SetdecSpeedUp(logcalchn,HI_FALSE);
                HI3521_SetdecSpeedDown(logcalchn,HI_FALSE);
                HI3521_SetdecPause(logcalchn,HI_FALSE);
                AV_PRINT("repaly again");
                memcpy(pstPB, &stTmpPB, sizeof(REC_PLAYBACK_T));
                HI3521_SetPlayCtrl(-1,logcalchn);
            }
            else
            {
                continue;
            }
        }
        else
        {
            /*一段录像可能有N个时间段组成*/
            for (s32FileCnt = 0; s32FileCnt < pstSFList->s32FileSegNum; s32FileCnt++)
            {
                memset(&stSFName, 0, sizeof(stSFName));
                memcpy(&stSFName.szFileName, pstSFList->stFileName[s32FileCnt].szFileName, FILE_NAME_LEN);
                AV_PRINT("s32Chn(%d),start HI3521_OpenSF(%s) ",s32Chn,stSFName.szFileName);
                pFile = HI3521_OpenSF(&stSFName);
                if (NULL == pFile)
                {
                    continue;
                }
                AV_PRINT("s32Chn(%d), HI3521_OpenSF(%s) success",s32Chn,stSFName.szFileName);
                s32Ret = STOR_GetIFrameSeek(pFile, &u32FrameSeek, pstPB->tStartTime);
                if (HI_SUCCESS != s32Ret)
                {
                    s32Ret = STOR_GetIFrameSeek(pFile, &u32FrameSeek, pstPB->tStartTime + 1);
                    if (HI_SUCCESS != s32Ret)
                    {
                        if(NULL != pFile)
                        {
                            fclose(pFile);
                            pFile = NULL;
                        }
                        AV_PRINT("STOR_GetIFrameSeek err");
                        continue;
                    }
                }
                s32Ret = STOR_GetVideoFileHeadInfo(pFile, &stFileBlkHead, PART_INDEX_SEEK_HEAD);
                if (HI_SUCCESS != s32Ret)
                {
                    if(NULL != pFile)
                    {
                        fclose(pFile);
                        pFile = NULL;
                    }
                    continue;
                }
                u32FrameEnd = stFileBlkHead.frame_data_end_pos;
                s32Ret = HI3521_SndFrameData2Vdec(pFile, s32Chn, s32FileCnt,
                                                  u32FrameSeek, u32FrameEnd, pstPB, &stTmpPB);
#if 0
                if (1 == s32Ret)
                {
                    if (NULL != pFile)
                    {
                        fclose(pFile);
                        pFile = NULL;
                    }
                    HDD_SetHddEvent(DISK_EVENT_REC_LPB, HI_FALSE);
                    if (!HDD_GetHddEvent(DISK_EVENT_REC_LPB))
                    {
                        DOMAIN_PackageData(ID_UI, AV_CLOSE_L_PLAY_BACK, NULL, 0);
                        AV_PRINT("all chan play ok DOMAIN_PackageData");
                    }
                    return NULL;
                }
#endif
                if (NULL != pFile)
                {
                    fclose(pFile);
                    pFile = NULL;
                }
                if (s32Ret == 2 || 1 == s32Ret)
                {
                    AV_PRINT("stop play ok");
                    break;
                }
            }
            if (s32Ret == 0)
            {
                AV_PRINT("s32Chn :%d play over exit play",s32Chn);
                break;
            }
            else if(s32Ret == 1)
            {
                AV_PRINT("s32Chn :%d exit playback.Hdd umount",s32Chn);
                break;
            }
        }
    }
    HI3521_Cleardecexit(logcalchn);
    if(!HI3521_Getalldecexit())
    {
        HDD_SetHddEvent(DISK_EVENT_REC_LPB, HI_FALSE);
        if (!HDD_GetHddEvent(DISK_EVENT_REC_LPB))
        {
            DOMAIN_PackageData(ID_UI, AV_CLOSE_L_PLAY_BACK, NULL, 0);
            AV_PRINT("all chan play ok DOMAIN_PackageData");
        }
    }
    return NULL;
}
/*****************************************************************************
 函 数 名  : HI3521_RecodePlayback
 功能描述  : 历史录像回放
 输入参数  : TM_APP_RECODE_PLAYBACK_t *pstPlaybackCMD
 输出参数  : 无
 返 回 值  :

 修改历史      :
  1.日    期   : 2016年6月17日
    作    者   : duzhanhui
    修改内容   : 新生成函数

*****************************************************************************/
void * HI3521_StartVDecThd(void *arg)
{
    AV_PRINT("@%s pid is %d,tid%d", __FUNCTION__,getpid(),gettid());
    int iRet = 0;
    SF_PB_ST *pstPlayback = (SF_PB_ST *)arg;
    SF_PB_ST stSFPB,stSFPBtmp;
    int i = 0;
    int j = 0;
    pthread_t VdecThread[VDEC_MAX_CHN_NUM];
    memcpy(&stSFPB.stPB, &pstPlayback->stPB, sizeof(REC_PLAYBACK_T));
    memcpy(&stSFPBtmp,pstPlayback,sizeof(SF_PB_ST));
    /*解码音频初始化*/
    HI3521A_AdecExit();
    HI3521A_AoExit();

#if (( AENC_TYPE == AENC_PCM) || (AENC_TYPE ==AENC_ADPCM))

    /*LPCM 320 的采样点  */
    HI3521A_AoInit(VOIP_DATA_PER_FRAM);

#elif ( AENC_TYPE == AENC_G711)

    HI3521A_AoInit(REC_AUDIO_SAMP_PER_FRAME);

#endif

    HI3521A_AdecInit();

    HDD_StartLPB();
    iRet = HI3521_StartVideoVDEC(stSFPB.stPB.s32Cmd);
    if (HI_SUCCESS != iRet)
    {
        AV_PRINT("#[app]TM_APP_StartVideoVDEC err");
        return NULL;
    }
    HI3521_SignalPBCond(pstPlayback);
    SF_PB_ST stSFPBbyChn[MAX_PLAY_BACK_CHN];
    for(i = 0;i<MAX_PLAY_BACK_CHN;i++)
    {
        memcpy(&stSFPBbyChn[i].stPB, &pstPlayback->stPB, sizeof(REC_PLAYBACK_T));

        HI3521_InitPBMutex(&stSFPBbyChn[i]);
    }
    //HI3521_InitPBMutex(&stSFPB);
    HI3521_InitPBCtrl();
    BOOL bExistDecode = HI_FALSE;
    for(i=0; i < APP_VENC_CHNCNT; i++)
    {
        if((((stSFPBtmp.stPB.s32Cmd)>>i)&0x01)==0)
            continue;
        AV_PRINT("########### select chn :%d ##########",i);
        //stSFPB.stPB.s32Cmd = i;
        stSFPBbyChn[i%MAX_PLAY_BACK_CHN].stPB.s32Cmd = i;
        //iRet = HI3521_LoaclPlayback(&stSFPB.stPB);
        iRet = HI3521_LoaclPlayback(&(stSFPBbyChn[i%MAX_PLAY_BACK_CHN].stPB));
        if (HI_SUCCESS != iRet)
        {
            AV_PRINT("#[app]HI3521_LoaclPlayback err or no record,CHN(%d)",i);
            continue;
        }
        //stSFPB.Chn = i;
        stSFPBbyChn[i%MAX_PLAY_BACK_CHN].Chn = i;
        stSFPBbyChn[i%MAX_PLAY_BACK_CHN].bActive = HI_TRUE;
        HI3521_Setdecexit(i%MAX_PLAY_BACK_CHN);
        //iRet = pthread_create(&VdecThread[j], 0, HI3521_VdecbyChnThd, (HI_VOID *)&stSFPB);
        iRet = pthread_create(&VdecThread[j], 0, HI3521_VdecbyChnThd  \
                                , (HI_VOID *)&(stSFPBbyChn[i%MAX_PLAY_BACK_CHN]));
        if (HI_SUCCESS != iRet)
        {
            AV_PRINT("create HI3521_VdecbyChnThd failed, err-%s!",  ERR_STR);
            return NULL;
        }
        iRet = pthread_detach(VdecThread[j]);
        if (HI_SUCCESS != iRet)
        {
            AV_PRINT("detach HI3521_VdecbyChnThd failed, err-%s!",  ERR_STR);
            return NULL;
        }
        AV_PRINT("########### create decode thread sucess chn(%d) ##########",i);
        bExistDecode = HI_TRUE;
        usleep(10*1000);
        j++;
    }
    for(i = 0;i<MAX_PLAY_BACK_CHN;i++)
    {
        if(stSFPBbyChn[i].bActive == HI_TRUE)
        {
            HI3521_WaitPBCond(&(stSFPBbyChn[i]));
        }
        HI3521_ExitPBMutex(&(stSFPBbyChn[i]));
    }
    if(bExistDecode == HI_TRUE)
    {
        DOMAIN_PackageData(ID_UI, AV_PLAYBACK_OK, NULL, 0);
        HDD_SetHddEvent(DISK_EVENT_REC_LPB, HI_TRUE);
        while(1)
        {
            if(!HDD_GetHddEvent(DISK_EVENT_REC_LPB))
            {
                LPB_SWITCH_AO_T stLPBAoSwitch;
                stLPBAoSwitch.cSwitchAo = 0;
                AV_PRINT("DISK_EVENT_REC_LPB OVER!!!");
                DOMAIN_PackageData(ID_BZQ, AV_LPB_AO_SWITCH, (HI_U8 *)&stLPBAoSwitch, sizeof(stLPBAoSwitch));
                break;
            }
            //usleep(10*1000);
            TM_USleep(0,10*1000);
            continue;
        }
    }
    AV_PRINT("HI3531_StopVDecThd SUCCESS.");
    HI3521_deInitPBCtrl();
    HI3521_VdecChnDisable();
    HI3521A_AdecExit();
    HI3521A_AoExit();
    /*回放结束，重新初始化*/
    HI3521A_AoInit(HI_MP3_PLAY_PTNUMPERFRM);
    HDD_ExitLPB();
    HI3521_StartDefaultPreview();
    return NULL;
}
HI_S32 HI3521_StartPlayback(REC_PLAYBACK_T *pstLPB)
{
    pthread_t tid;
    HI_S32 s32Err = HI_SUCCESS;
    HI_S32 s32Ret = HI_SUCCESS;
    SF_PB_ST stSFPB;
    SF_PB_ST *pstSFPB = &stSFPB;
    memcpy(&stSFPB.stPB, pstLPB, sizeof(REC_PLAYBACK_T));
    HI3521_InitPBMutex(pstSFPB);
    s32Ret = pthread_create(&tid, NULL, HI3521_StartVDecThd, (void *)pstSFPB);
    if (HI_SUCCESS != s32Ret)
    {
        s32Err = HI_FAILURE;
        AV_PRINT("err create HI3521_StartVDecThd!");
    }
    s32Ret = pthread_detach(tid);
    if (HI_SUCCESS != s32Ret)
    {
        s32Err = HI_FAILURE;
        AV_PRINT("err detach HI3521_StartVDecThd!");
    }
    HI3521_WaitPBCond(pstSFPB);
    HI3521_ExitPBMutex(pstSFPB);
    return s32Err;
}

/**
 * 查询历史视频
 * @param req         [in] 查询参数
 * @param resp        [in] 响应信息
 */
HI_S32 HI3521_QueryRecord(time_t tStartTime,time_t  tEndTime,REC_QUREY_RSP_T *resp,HI_U32 chn)
{
    REC_PLAYBACK_T  pstPB = {0};
    pstPB.s32Cmd = chn;
    memset(resp,0,sizeof(REC_QUREY_RSP_T));
    pstPB.tStartTime =tStartTime;
    pstPB.tEndTime=tEndTime;
    SF_LIST_ST  pstSFList = {0};
    HI_BOOL bLocal = HI_FALSE;
    HI_S32 s32RecType = 0;
    resp->chn = chn;
    AV_PRINT("HI3521_QueryRecord Chn:%d s=%ld,e=%ld",chn,pstPB.tStartTime,pstPB.tEndTime);
    if(HI_SUCCESS != HI3531_SFByTime_V28181((SF_REQ_CMD_ST*)&pstPB,&pstSFList,bLocal,s32RecType))
    {
        return HI_FAILURE;
    }
    REC_ITEM_T  Items;
    Items.type = 1;
    int iRecItemCount = 0;
    for(int i = 0; i<pstSFList.s32FileSegNum; i++)
    {
        if(i==0)
        {
            Items.rsp_time.uiStartTime= pstSFList.stFileTimeSeg[i].tStartTime;
            Items.rsp_time.uiEndTime= pstSFList.stFileTimeSeg[i].tEndTime;
            if(Items.rsp_time.uiStartTime<(HI_U32)(pstPB.tStartTime))
                Items.rsp_time.uiStartTime=pstPB.tStartTime;
            if(Items.rsp_time.uiEndTime>(HI_U32)(pstPB.tEndTime))
                Items.rsp_time.uiEndTime=pstPB.tEndTime;
            memcpy(&(resp->rec_items[iRecItemCount]),&Items,sizeof(REC_ITEM_T));
            iRecItemCount++;
        }
        else
        {
            if((Items.rsp_time.uiEndTime+1)==(HI_U32)(pstSFList.stFileTimeSeg[i].tStartTime))
            {
                Items.rsp_time.uiEndTime= pstSFList.stFileTimeSeg[i].tEndTime;
                if(Items.rsp_time.uiEndTime>(HI_U32)(pstPB.tEndTime))
                    Items.rsp_time.uiEndTime=pstPB.tEndTime;
                AV_PRINT("HI3521_QueryRecord Update %d endTime=%d\n",iRecItemCount-1,(int)Items.rsp_time.uiEndTime);
                memcpy(&(resp->rec_items[iRecItemCount-1]),&Items,sizeof(REC_ITEM_T));
            }
            else
            {
                Items.rsp_time.uiStartTime= pstSFList.stFileTimeSeg[i].tStartTime;
                Items.rsp_time.uiEndTime= pstSFList.stFileTimeSeg[i].tEndTime;
                if(Items.rsp_time.uiStartTime<(HI_U32)(pstPB.tStartTime))
                    Items.rsp_time.uiStartTime=pstPB.tStartTime;
                if(Items.rsp_time.uiEndTime>(HI_U32)(pstPB.tEndTime))
                    Items.rsp_time.uiEndTime=pstPB.tEndTime;
                memcpy(&(resp->rec_items[iRecItemCount]),&Items,sizeof(REC_ITEM_T));
                AV_PRINT("HI3521_QueryRecord iRecItemCount %d beginTime =%d,endTime=%d\n",iRecItemCount,(int)Items.rsp_time.uiStartTime,(int)Items.rsp_time.uiEndTime);
                iRecItemCount++;
            }
        }
    }
    resp->sum = iRecItemCount;
    return HI_SUCCESS;
}

void *APP_LocalRecQeryThread(HI_VOID *pArg)
{
    AV_PRINT("@%s pid is %d,tid%d", __FUNCTION__,getpid(),gettid());
    SF_PB_ST stSFPBarg;
    SF_PB_ST *pstSFPB = (SF_PB_ST *)pArg;
    memcpy(&stSFPBarg.stPB, &pstSFPB->stPB, sizeof(REC_PLAYBACK_T));
    HI3521_SignalPBCond(pstSFPB);
    REC_QUREY_RSP_T query_resp;
    int chn =0;
    for(chn =  0; chn< APP_VENC_CHNCNT; chn++)
    {
        HI3521_QueryRecord(stSFPBarg.stPB.tStartTime,stSFPBarg.stPB.tEndTime,&query_resp,chn);
        DOMAIN_PackageData(ID_UI, UI_REC_RESP, (HI_U8 *)&query_resp, sizeof(REC_QUREY_RSP_T));
        //usleep(100*1000);
        TM_USleep(0,100*1000);
    }
    return NULL;
}

void HI3521_LoaclRecQuery(RetrieveResTimeQuantum req_time)
{
    HI_U32 s32Ret;
    pthread_t recqerytid;
    SF_PB_ST stSFPB;
    SF_PB_ST *pstSFPB = &stSFPB;
    HI3521_InitPBMutex(pstSFPB);
    pstSFPB->stPB.tStartTime = req_time.uiStartTime;
    pstSFPB->stPB.tEndTime= req_time.uiEndTime;
    s32Ret = pthread_create(&recqerytid, NULL, APP_LocalRecQeryThread, pstSFPB);
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT("err create HI3521_LoaclRecQuery!");
        return;
    }

    s32Ret = pthread_detach(recqerytid);
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT("err detach HI3521_LoaclRecQuery!");
        return ;
    }
    HI3521_WaitPBCond(pstSFPB);
    HI3521_ExitPBMutex(pstSFPB);
}


/* BEGIN: Added by zhangjinlei, 2014/10/11  DES:添加调整视频输入参数  PN:14100801008 */
/*****************************************************************************
 Prototype    : APP_ChkVideoCfgVer
 Description  : 检测视频配置参数版本号
 Input        : const HI_CHAR *cfg_file
 Output       : None
 Return Value :

  History        :
  1.Date         : 2014/10/11
    Author       : zhangjinlei
    Modification : Created function

*****************************************************************************/
HI_S32 APP_ChkVideoCfgVer(const HI_CHAR *cfg_file)
{
    FILE *cfg_fp = NULL;
    HI_S32 s32Ret = 0;

    VIDEO_CFG_T stVideCfg;

    cfg_fp = fopen(cfg_file, "r+");
    if(cfg_fp == NULL)
    {
        AV_PRINT( "  in write config fopen filename=%s", cfg_file);

        return HI_FAILURE;
    }

    s32Ret = fseek(cfg_fp,0,SEEK_SET);

    s32Ret = fread(&stVideCfg, 1, sizeof(VIDEO_CFG_T), cfg_fp);
    if(s32Ret<0)
    {
        AV_PRINT( " read recode log config err!");

        fclose(cfg_fp);

        return HI_FAILURE;
    }

    if (APP_VIDEO_CFG_VERSION != stVideCfg.s32Ver)
    {
        AV_PRINT( " video input output cfg version err!version=%d, vale=%d",
                  APP_VIDEO_CFG_VERSION, stVideCfg.s32Ver);

        fclose(cfg_fp);

        return HI_FAILURE;
    }

    fclose(cfg_fp);

    return HI_SUCCESS;
}


/*****************************************
 *函数名    	：write_config
 *功能描述	    ：写入配置文件
 *入口参数  	：cfg_file--文件名， cfg_s--要写入的数据
 *出口参数  	：暂无
 *返回值 		：成功返回0，失败返回相应的值
 *****************************************/
HI_S32 APP_WrVideoParaCfg(const HI_CHAR *cfg_file, HI_VOID *add_t)
{
    FILE *cfg_fp = NULL;
    HI_S32 s32Ret = 0;
    VIDEO_CFG_T *pstVideoOutPara = NULL;

    pstVideoOutPara = (VIDEO_CFG_T *)add_t;

    if (cfg_file == NULL)
    {
        AV_PRINT( " Input NULL pointer!");

        return HI_FAILURE;
    }

    cfg_fp = fopen(cfg_file, "w");
    if(cfg_fp == NULL)
    {
        AV_PRINT( "  in write config fopen filename=%s", cfg_file);

        return HI_FAILURE;
    }

    s32Ret = fseek(cfg_fp,0,SEEK_SET);

    s32Ret=fwrite(pstVideoOutPara, sizeof(VIDEO_CFG_T), 1,cfg_fp);
    if(s32Ret < 0)
    {
        AV_PRINT( " write recode log config err!");

        fclose(cfg_fp);
        return HI_FAILURE;
    }

    fflush(cfg_fp);

    fclose(cfg_fp);

    return 0;
}


/* END:   Added by zhangjinlei, 2014/10/11   PN:14100801008 */

/*****************************************************************************
 函 数 名  : APP_IsFileExist
 功能描述  : 检测配置文件是否存在
 输入参数  : const HI_CHAR *cfg_file
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年6月4日
    作    者   : zhangjinlei
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 APP_IsFileExist(const HI_CHAR *cfg_file)
{
    HI_S32 s32Ret = 0;
    VIDEO_CFG_T stVideoCfg;
    HI_S32 i = 0;

    if(!access(cfg_file, F_OK))  //文件存在
    {
        /* 检测版本号 */
        s32Ret = APP_ChkVideoCfgVer(cfg_file);
        if (HI_SUCCESS != s32Ret)
        {
            AV_PRINT( " APP_ChkVideoCfgVer filename=%s", cfg_file);

            //return HI_FAILURE;
        }
        else
        {
            return HI_SUCCESS;
        }
    }

    for ( i = 0 ; i < APP_VI_CHNCNT; i++ )
    {
        stVideoCfg.stInputCfg[i].s32CscLuma = APP_VIDEO_INPUT_BRIGHT;
        stVideoCfg.stInputCfg[i].s32CscContr = APP_VIDEO_INPUT_CONTRAST;
        stVideoCfg.stInputCfg[i].s32CscHue = APP_VIDEO_INPUT_HUE;
        stVideoCfg.stInputCfg[i].s32CscSatu = APP_VIDEO_INPUT_SATURATION;
        stVideoCfg.stInputCfg[i].s32Sharpness = APP_VIDEO_INPUT_SHARPNESS;
    }

    /* 设置默认参数 */
    stVideoCfg.stOutputCfg.s32CscLuma = APP_VIDEO_OUTPUT_BRIGHT;
    stVideoCfg.stOutputCfg.s32CscContr = APP_VIDEO_OUTPUT_CONTRAST;
    stVideoCfg.stOutputCfg.s32CscHue = APP_VIDEO_OUTPUT_HUE;
    stVideoCfg.stOutputCfg.s32CscSatu = APP_VIDEO_OUTPUT_SATURATION;

    stVideoCfg.s32Ver = APP_VIDEO_CFG_VERSION;

    s32Ret = APP_WrVideoParaCfg(cfg_file, &stVideoCfg);
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT( " write config fopen filename=%s", cfg_file);

        return HI_FAILURE;
    }

    return HI_SUCCESS;
}



/*****************************************
 *函数名    	：read_config
 *功能描述	    ：读取配置文件
 *入口参数  	：cfg_file--文件名， cfg_s--保存读取到的数据
 *出口参数  	：暂无
 *返回值 		：成功返回0，失败返回相应的值
 *修改记录 		: 2012-05-05 增加voip_time-ip对讲限制时间，原来的ip地址的关键字"ip"就会出现匹配错误问题，
 				  主要是ip地址匹配到voip_time上了，因此将ip地址关键字改为"tcp_ip",避免关键字冲突
 *****************************************/
HI_S32 APP_RdVideoParaCfg(const HI_CHAR *cfg_file, HI_VOID *add_t)
{
    FILE *cfg_fp = NULL;
    HI_S32 s32Ret = 0;
    VIDEO_CFG_T *cfg_s = (VIDEO_CFG_T *)add_t;

    /* 检测文件是否存在，如果不存在，就新生成一个 */
    s32Ret = APP_IsFileExist(cfg_file);
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT( " Creat config failed!");

        return HI_FAILURE;
    }

    cfg_fp = fopen(cfg_file, "r+");
    if(cfg_fp == NULL)
    {
        AV_PRINT( "  in write config fopen filename=%s", cfg_file);

        return HI_FAILURE;
    }

    s32Ret = fseek(cfg_fp,0,SEEK_SET);

    s32Ret = fread(cfg_s, 1, sizeof(VIDEO_CFG_T), cfg_fp);
    if(s32Ret<0)
    {
        AV_PRINT( " read recode log config err!");

        fclose(cfg_fp);

        return HI_FAILURE;
    }

    fclose(cfg_fp);

    return HI_SUCCESS;
}

HI_S32 HI3521_Open_HdViDev()
{
    g_Videcfd = open(NVP6114DEV,O_RDWR);
    if(g_Videcfd == -1)
    {
        printf("open %s error\n",NVP6114DEV);
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

HI_S32 HI3521_Open_SdViDev()
{
    g_Videcsd = open(TW2968DEV,O_RDWR);
    if(g_Videcsd == -1)
    {
        printf("open %s error\n",TW2968DEV);
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

HI_S32 HI3521_SdSetVideoInPara(tw2968_image_adjust videoinpara)
{
    videoinpara.contrast = videoinpara.contrast*255/100;
    videoinpara.saturation = videoinpara.saturation*255/100;
    videoinpara.hue = videoinpara.hue-50;
    videoinpara.sharpness = videoinpara.sharpness*15/100;
    videoinpara.brightness = videoinpara.brightness -50;
    videoinpara.item_sel = TW2968_SET_BRIGHT|TW2968_SET_CONTRAST|TW2968_SET_SATURATION|TW2968_SET_HUE|TW2968_SET_SHARPNESS;
    ioctl(g_Videcsd,TW2968_SET_IMAGE_ADJUST,&videoinpara);
    return HI_SUCCESS;
}

HI_S32 HI3521_SdGetVideoLostStat(tw2968_video_loss *video_loss)
{
    ioctl(g_Videcsd,TW2968_GET_VIDEO_LOSS,video_loss);
    return HI_SUCCESS;
}

HI_S32 HI3521_SdGetVideoInPara(tw2968_image_adjust *videoinpara)
{
    ioctl(g_Videcsd,TW2968_GET_IMAGE_ADJUST,videoinpara);
    return HI_SUCCESS;
}

HI_S32 HI3521_HdGetVideoLostStat(HI_VOID)//低4位有效
{
    HI_S32 video_stat = 0;
    ioctl(g_Videcfd,IOC_VDEC_GET_VIDEO_LOSS,&video_stat);
    return video_stat;
}
HI_S32 HI3521_HdSetVideoInBrightness(HI_U32 chl,HI_U32 val)//0-3通道有效
{
    vdec_video_adjust vadjust;
    vadjust.ch = chl >7? (chl-8):chl;
    vadjust.value = val+50;
    ioctl(g_Videcfd,IOC_VDEC_SET_BRIGHTNESS,&vadjust);
    return HI_SUCCESS;
}


HI_S32 HI3521_HdSetVideoInContrast(HI_U32 chl,HI_U32 val)
{
    vdec_video_adjust vadjust;
    vadjust.ch = chl >7? (chl-8):chl;
    vadjust.value = val+50;
    ioctl(g_Videcfd,IOC_VDEC_SET_CONTRAST,&vadjust);
    return HI_SUCCESS;
}

HI_S32 HI3521_HdSetVideoInHue(HI_U32 chl,HI_U32 val)
{
    vdec_video_adjust vadjust;
    vadjust.ch = chl >7? (chl-8):chl;
    vadjust.value = val+50;
    ioctl(g_Videcfd,IOC_VDEC_SET_HUE,&vadjust);
    return HI_SUCCESS;
}

HI_S32 HI3521_HdSetVideoInSaturation(HI_U32 chl,HI_U32 val)
{
    vdec_video_adjust vadjust;
    vadjust.ch =chl >7? (chl-8):chl;
    vadjust.value = val+50;
    ioctl(g_Videcfd,IOC_VDEC_SET_SATURATION,&vadjust);
    return HI_SUCCESS;
}
HI_S32 HI3521_HdSetVideoInSharpness(HI_U32 chl,HI_U32 val)
{
    vdec_video_adjust vadjust;
    vadjust.ch = chl >7? (chl-8):chl;
    vadjust.value = val+50;
    ioctl(g_Videcfd,IOC_VDEC_SET_SHARPNESS,&vadjust);
    return HI_SUCCESS;
}



HI_S32 HI3521_VideoInParaInit(HI_VOID)
{
    HI_S32 i =0;
    HI_S32 s32Result;
    VIDEO_CFG_T stVideoCfg;
    tw2968_image_adjust videoinpara;
    memset(&videoinpara,0,sizeof(tw2968_image_adjust));
    s32Result = APP_RdVideoParaCfg(APP_DEV_CFG, &stVideoCfg);
    if (HI_SUCCESS != s32Result)
    {
        AV_PRINT( " Read app cfg failed!");
        return HI_FAILURE;
    }
    for(i=0; i< HI_VI_MAX_CHN; i++)
    {
        if(i > 7)//HD
        {
            HI3521_HdSetVideoInBrightness(i,stVideoCfg.stInputCfg[i].s32CscLuma);
            HI3521_HdSetVideoInContrast(i,stVideoCfg.stInputCfg[i].s32CscContr);
            HI3521_HdSetVideoInHue(i,stVideoCfg.stInputCfg[i].s32CscHue);
            HI3521_HdSetVideoInSaturation(i,stVideoCfg.stInputCfg[i].s32CscSatu);
            HI3521_HdSetVideoInSharpness(i,stVideoCfg.stInputCfg[i].s32Sharpness);
        }
        else
        {
            videoinpara.brightness = stVideoCfg.stInputCfg[i].s32CscLuma;
            videoinpara.contrast = stVideoCfg.stInputCfg[i].s32CscContr;
            videoinpara.saturation = stVideoCfg.stInputCfg[i].s32CscSatu;
            videoinpara.sharpness = stVideoCfg.stInputCfg[i].s32Sharpness;
            videoinpara.hue = stVideoCfg.stInputCfg[i].s32CscHue;
            videoinpara.chn = i ;
            HI3521_SdSetVideoInPara(videoinpara);
        }
    }
    s32Result = APP_WrVideoParaCfg(APP_DEV_CFG, &stVideoCfg);
    if (HI_SUCCESS != s32Result)
    {
        AV_PRINT( " APP_WrVideoParaCfg error! result=0x%x",
                  s32Result);
        return HI_FAILURE;
    }
    return 0;
}
/* BEGIN: Added by zhangjinlei, 2014/10/11  DES:添加调整视频输入参数  PN:14100801008 */
/*****************************************************************************
 Prototype    : APP_SetVIParae
 Description  : 设置视频输入参数
 Input        : VI_PARA_T *pstVideoInPara
 Output       : None
 Return Value :

  History        :
  1.Date         : 2014/10/11
    Author       : zhangjinlei
    Modification : Created function

*****************************************************************************/
HI_S32 APP_SetVideoInPara(VI_PARA_T *pstVideoInPara)
{
    HI_S32 i =0;
    HI_S32 s32Result;
    VIDEO_CFG_T stVideoCfg;
    tw2968_image_adjust videoinpara;
    memset(&videoinpara,0,sizeof(tw2968_image_adjust));
    s32Result = APP_RdVideoParaCfg(APP_DEV_CFG, &stVideoCfg);
    if (HI_SUCCESS != s32Result)
    {
        AV_PRINT( " Read app cfg failed!");

        return HI_FAILURE;
    }


    for(i=0; i < HI_VI_MAX_CHN; i++)
    {
        if(i > 7)//HD
        {
            HI3521_HdSetVideoInBrightness(i,pstVideoInPara->s32CscLuma[i]);
            HI3521_HdSetVideoInContrast(i,pstVideoInPara->s32CscContr[i]);
            HI3521_HdSetVideoInHue(i,pstVideoInPara->s32CscHue[i]);
            HI3521_HdSetVideoInSaturation(i,pstVideoInPara->s32CscSatu[i]);
            HI3521_HdSetVideoInSharpness(i,pstVideoInPara->s32Sharpness[i]);
        }
        else
        {
            videoinpara.brightness = pstVideoInPara->s32CscLuma[i];
            videoinpara.contrast = pstVideoInPara->s32CscContr[i];
            videoinpara.saturation = pstVideoInPara->s32CscSatu[i];
            videoinpara.sharpness = pstVideoInPara->s32Sharpness[i];
            videoinpara.hue = pstVideoInPara->s32CscHue[i];
            videoinpara.chn = i ;
            HI3521_SdSetVideoInPara(videoinpara);
        }
        stVideoCfg.stInputCfg[i].s32CscLuma=pstVideoInPara->s32CscLuma[i];
        stVideoCfg.stInputCfg[i].s32CscContr=pstVideoInPara->s32CscContr[i];
        stVideoCfg.stInputCfg[i].s32CscHue=pstVideoInPara->s32CscHue[i];
        stVideoCfg.stInputCfg[i].s32CscSatu=pstVideoInPara->s32CscSatu[i];
        stVideoCfg.stInputCfg[i].s32Sharpness=pstVideoInPara->s32Sharpness[i];
    }


    /* 保存到文件 */
    s32Result = APP_WrVideoParaCfg(APP_DEV_CFG, &stVideoCfg);
    if (HI_SUCCESS != s32Result)
    {
        AV_PRINT( " APP_WrVideoParaCfg error! result=0x%x",
                  s32Result);

        return HI_FAILURE;
    }
    return 0;
}

HI_S32 APP_SetVideoInPara_Chn(VI_PARA_CHN_T *pstVideoInPara)
{
    tw2968_image_adjust videoinpara;
    memset(&videoinpara,0,sizeof(tw2968_image_adjust));
    if(pstVideoInPara->s32Chn > 7)//HD
    {
        HI3521_HdSetVideoInBrightness(pstVideoInPara->s32Chn,pstVideoInPara->s32CscLuma);
        HI3521_HdSetVideoInContrast(pstVideoInPara->s32Chn,pstVideoInPara->s32CscContr);
        HI3521_HdSetVideoInHue(pstVideoInPara->s32Chn,pstVideoInPara->s32CscHue);
        HI3521_HdSetVideoInSaturation(pstVideoInPara->s32Chn,pstVideoInPara->s32CscSatu);
        HI3521_HdSetVideoInSharpness(pstVideoInPara->s32Chn,pstVideoInPara->s32Sharpness);
    }
    else
    {
        videoinpara.brightness = pstVideoInPara->s32CscLuma;
        videoinpara.contrast = pstVideoInPara->s32CscContr;
        videoinpara.saturation = pstVideoInPara->s32CscSatu;
        videoinpara.sharpness = pstVideoInPara->s32Sharpness;
        videoinpara.hue = pstVideoInPara->s32CscHue;
        videoinpara.chn = pstVideoInPara->s32Chn ;
        HI3521_SdSetVideoInPara(videoinpara);
    }
    return 0;
}
/*****************************************************************************
 Prototype    : APP_GetVideoInputPare
 Description  : 获取视频输入参数
 Input        : VI_PARA_T *pstVideoInPara
 Output       : None
 Return Value :

  History        :
  1.Date         : 2014/10/15
    Author       : zhangjinlei
    Modification : Created function

*****************************************************************************/
HI_S32 APP_GetVideoInputPara(VI_PARA_T *pstVideoInPara)
{
    HI_S32 s32Ret = 0;
    HI_S32 i = 0;
    VIDEO_CFG_T stVideoCfg;

    if (NULL == pstVideoInPara)
    {
        AV_PRINT( " input error pointer,pstVideoOutPara=0x%x",
                  (HI_S32)pstVideoInPara);

        return HI_FAILURE;
    }

    s32Ret = APP_RdVideoParaCfg(APP_DEV_CFG, &stVideoCfg);
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT( " Read app cfg failed!");

        return HI_FAILURE;
    }
    for ( i = 0; i < HI_VI_MAX_CHN; i++ )
    {
        pstVideoInPara->s32CscLuma[i] = stVideoCfg.stInputCfg[i].s32CscLuma;
        pstVideoInPara->s32CscContr[i] = stVideoCfg.stInputCfg[i].s32CscContr;
        pstVideoInPara->s32CscHue[i] = stVideoCfg.stInputCfg[i].s32CscHue;
        pstVideoInPara->s32CscSatu[i] = stVideoCfg.stInputCfg[i].s32CscSatu;
        pstVideoInPara->s32Sharpness[i] = stVideoCfg.stInputCfg[i].s32Sharpness;
    }
    return HI_SUCCESS;
}



/*****************************************************************************
 函 数 名  : APP_GetVideoOutputPara
 功能描述  : 获取视频输出参数
 输入参数  : VO_PARA_T *pstVideoOutPara
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年6月3日
    作    者   : zhangjinlei
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 APP_GetVideoOutputPara(VO_PARA_T *pstVideoOutPara)
{
    HI_S32 s32Result = 0;
    VO_CSC_S stVideoOutPara;

    if (NULL == pstVideoOutPara)
    {
        AV_PRINT( " input error pointer,pstVideoOutPara=0x%x",
                  (HI_S32)pstVideoOutPara);

        return HI_FAILURE;
    }
    /* 获取参数 */
    s32Result = HI_MPI_VO_GetVideoLayerCSC(2, &stVideoOutPara);
    if (HI_SUCCESS != s32Result)
    {
        AV_PRINT( " Get dev video output error! result=0x%x",
                  s32Result);

        return HI_FAILURE;
    }
    pstVideoOutPara->s32CscContr = stVideoOutPara.u32Contrast;
    pstVideoOutPara->s32CscHue = stVideoOutPara.u32Hue;
    pstVideoOutPara->s32CscLuma = stVideoOutPara.u32Luma;
    pstVideoOutPara->s32CscSatu = stVideoOutPara.u32Saturation;
    return HI_SUCCESS;
}


/*****************************************************************************
 函 数 名  : APP_SetVideoOutputPare
 功能描述  : 设置视频输出参数并保存到文件
 输入参数  : VO_PARA_T *pstVideoOutPara
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年6月3日
    作    者   : zhangjinlei
    修改内容   : 新生成函数

*****************************************************************************/
HI_S32 APP_SetVideoOutputPara(VO_PARA_T *pstVideoOutPara)
{
    HI_S32 s32Result = 0;
    VO_CSC_S stVideoOutPara;
    VIDEO_CFG_T stVideoCfg;

    if (NULL == pstVideoOutPara)
    {
        AV_PRINT( " input error pointer,pstVideoOutPara=0x%x",
                  (HI_S32)pstVideoOutPara);
        return HI_FAILURE;
    }
    memcpy(&stVideoOutPara.u32Luma,&pstVideoOutPara->s32CscLuma, sizeof(VO_PARA_T));
    stVideoOutPara.enCscMatrix = VO_CSC_MATRIX_IDENTITY;
    s32Result = HI_MPI_VO_SetVideoLayerCSC(2, &stVideoOutPara);
    if (HI_SUCCESS != s32Result)
    {
        AV_PRINT( " Set dev video output error! result=0x%x",
                  s32Result);
        return HI_FAILURE;
    }
    s32Result = APP_RdVideoParaCfg(APP_DEV_CFG, &stVideoCfg);
    if (HI_SUCCESS != s32Result)
    {
        AV_PRINT( " Read app cfg failed!");

        return HI_FAILURE;
    }
    stVideoCfg.stOutputCfg.s32CscLuma = pstVideoOutPara->s32CscLuma;
    stVideoCfg.stOutputCfg.s32CscContr = pstVideoOutPara->s32CscContr;
    stVideoCfg.stOutputCfg.s32CscHue = pstVideoOutPara->s32CscHue;
    stVideoCfg.stOutputCfg.s32CscSatu = pstVideoOutPara->s32CscSatu;

    /* 保存到文件 */
    s32Result = APP_WrVideoParaCfg(APP_DEV_CFG, &stVideoCfg);
    if (HI_SUCCESS != s32Result)
    {
        AV_PRINT( " APP_WrVideoParaCfg error! result=0x%x",
                  s32Result);

        return HI_FAILURE;
    }
    return HI_SUCCESS;
}
HI_S32 APP_SetDefaultChnPara(HI_U16 u16Para)
{
    HI_S32 s32Result;
    VIDEO_CFG_T stVideoCfg;
    s32Result = APP_RdVideoParaCfg(APP_DEV_CFG, &stVideoCfg);
    if (HI_SUCCESS != s32Result)
    {
        AV_PRINT( " Read app cfg failed!");
        return HI_FAILURE;
    }
    stVideoCfg.s32PreviewType = u16Para;
    s32Result = APP_WrVideoParaCfg(APP_DEV_CFG, &stVideoCfg);
    if (HI_SUCCESS != s32Result)
    {
        AV_PRINT( " APP_WrVideoParaCfg error! result=0x%x",
                  s32Result);
        return HI_FAILURE;
    }
    return 0;
}
HI_S32 APP_GetDefaultChnPara(HI_S32 *pdefaultchn)
{
    HI_S32 s32Ret = 0;
    VIDEO_CFG_T stVideoCfg;
    s32Ret = APP_RdVideoParaCfg(APP_DEV_CFG, &stVideoCfg);
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT( " Read app cfg failed!");
        return HI_FAILURE;
    }
    if((stVideoCfg.s32PreviewType <1 )||(stVideoCfg.s32PreviewType > 16))
        *pdefaultchn = 13;
    else
        *pdefaultchn = stVideoCfg.s32PreviewType;
    return 0;
}
HI_S32 HI3521_GetChnStoreStat(HI_U16 *chnStore)
{
    HI_S32 s32Chn = 0;
    CHN_STOR_MGMT_ST *pstChnStorMgmt = NULL;
    for (s32Chn = 0; s32Chn < APP_VI_CHNCNT; s32Chn++)
    {
        pstChnStorMgmt = STOR_GetChnStoreMgmt(s32Chn);
        if (pstChnStorMgmt->bRecording)
        {
            *chnStore |=(1<<s32Chn);
        }
        else
        {
            *chnStore &=~(1<<s32Chn);
        }
    }
    return HI_SUCCESS;
}
HI_S32 HI3521_GetMp3FileStat(HI_VOID)
{
    return g_Mp3FileStat;
}
HI_S32 HI3521_GetDevErrStat(HI_VOID)
{
    HI_S32 s32Chn = 0;
    HI_S32 s32DiskStat = 0;
    HI_S32 s32mp3Stat = 0;
    HI_S32 s32SdStat = 0;
    HI_S32 s32VideoLost = 0;
    HI_S32 bit = 0;
    CHN_STOR_MGMT_ST *pstChnStorMgmt = NULL;
    HI_S32 s32DevErrStat = 0;
    time_t nowTime = 0;
    tw2968_video_loss video_loss;
    memset(&video_loss,0,sizeof(tw2968_video_loss));
    HI_S32 s32HdVideoStat = HI3521_HdGetVideoLostStat();
    HI3521_SdGetVideoLostStat(&video_loss);
    static HI_S32 s32Lost = 0;
    s32VideoLost = video_loss.is_lost;
    s32VideoLost|= (~s32HdVideoStat)<<8;
    if(s32Lost!=s32VideoLost)
        AV_PRINT("HI3521_GetDevErrStat %#x",s32Lost);
    s32Lost = s32VideoLost;
    for (s32Chn = 0; s32Chn < APP_VI_CHNCNT; s32Chn++)
    {
        if (s32VideoLost & (1<<s32Chn))
        {
            if (s32Chn < 4)
            {
                bit = s32Chn;
                SET_BIT_MASK(s32DevErrStat, bit);
            }
            else if(s32Chn < 8)
            {
                bit = s32Chn+12;
                SET_BIT_MASK(s32DevErrStat, bit);
            }
            else
            {
                bit = s32Chn+16;
                SET_BIT_MASK(s32DevErrStat, bit);
            }
        }
        else
        {
            if (s32Chn < 4)
            {
                bit = s32Chn;
                CLEAR_BIT_MASK(s32DevErrStat, bit);
            }
            else if(s32Chn < 8)
            {
                bit = s32Chn+12;
                CLEAR_BIT_MASK(s32DevErrStat, bit);
            }
            else
            {
                bit = s32Chn+16;
                CLEAR_BIT_MASK(s32DevErrStat, bit);
            }
        }
    }
    for (s32Chn = 0; s32Chn < APP_VI_CHNCNT; s32Chn++)
    {
        pstChnStorMgmt = STOR_GetChnStoreMgmt(s32Chn);
        if (pstChnStorMgmt->bRecording)
        {
            if (s32Chn < 4)
            {
                bit = s32Chn+4;
                SET_BIT_MASK(s32DevErrStat, bit);
            }
            else if(s32Chn < 8)
            {
                bit = s32Chn+16;
                SET_BIT_MASK(s32DevErrStat, bit);
            }
            else
            {
                bit = s32Chn+20;
                SET_BIT_MASK(s32DevErrStat, bit);
            }
        }
        else
        {
            if (s32Chn < 4)
            {
                bit = s32Chn+4;
                CLEAR_BIT_MASK(s32DevErrStat, bit);
            }
            else if(s32Chn < 8)
            {
                bit = s32Chn+16;
                CLEAR_BIT_MASK(s32DevErrStat, bit);
            }
            else
            {
                bit = s32Chn+20;
                CLEAR_BIT_MASK(s32DevErrStat, bit);
            }
        }
    }
    s32DiskStat = HDD_GetDiskStat();
    bit = 10;
    if (1 == s32DiskStat)
    {
        SET_BIT_MASK(s32DevErrStat, bit);
    }
    else
    {
        CLEAR_BIT_MASK(s32DevErrStat, bit);
    }
    bit = 9;
    s32mp3Stat = HI3521_GetMp3FileStat();
    if (1 == s32mp3Stat)
    {
        SET_BIT_MASK(s32DevErrStat, bit);
    }
    else
    {
        CLEAR_BIT_MASK(s32DevErrStat, bit);
    }
    s32SdStat = HDD_GetSdStat();
    bit = 8;
    if (1 == s32SdStat)
    {
        SET_BIT_MASK(s32DevErrStat, bit);
    }
    else
    {
        CLEAR_BIT_MASK(s32DevErrStat, bit);
    }
    nowTime = time((time_t *)NULL);
    bit = 11;
    if (nowTime > 31104000)
    {
        SET_BIT_MASK(s32DevErrStat, bit);
    }
    else
    {
        CLEAR_BIT_MASK(s32DevErrStat, bit);
    }
    return s32DevErrStat;
}
HI_S32  IFrameCnt[APP_VENC_CHNCNT] = {0};
int STOR_CheckUdiskFreespace(char *pcPartName)
{
    long long freespace = 0;
    struct statfs disk_statfs;
    long long totalspace = 0;
    float freeSpacePercent = 0 ;
    if ( statfs(pcPartName, &disk_statfs) >= 0 )
    {
        freespace = (((long long)disk_statfs.f_bsize * (long long)disk_statfs.f_bfree)/(long long)1024);
        totalspace = (((long long)disk_statfs.f_bsize * (long long)disk_statfs.f_blocks) /(long long)1024);
    }
    else
    {
        AV_PRINT("STOR_CheckUdiskFreespace get usb info error");
        return 0;
    }

    freeSpacePercent = ((float)freespace/(float)totalspace)*100 ;
    AV_PRINT("freeSpacePercent (%d)", (HI_S32)freeSpacePercent);
    if ((HI_S32)freeSpacePercent <= 1)
    {
        return 0;
    }
    return 1;
}

HI_S32 HI3521_ChkUsbBkTime(time_t *tStartTime, time_t *tEndTime)
{
    HI_S32 iTmptime = 0;
    if ( *tEndTime <= *tStartTime )
    {
        AV_PRINT("backup time error,tStartTime=%ld,tEndTime=%ld", *tStartTime, *tEndTime);
        return HI_FAILURE;
    }
    iTmptime = *tEndTime - *tStartTime;
    if ( LOCAL_BK_MAX_TIME < iTmptime )
    {
        AV_PRINT("backup time too long,tStartTime=%ld,tEndTime=%ld", *tStartTime, *tEndTime);
        *tEndTime = *tStartTime + LOCAL_BK_MAX_TIME;
    }
    else if ( iTmptime < LOCAL_BK_MIN_TIME)
    {
        AV_PRINT("backup time too short,tStartTime=%ld,tEndTime=%ld", *tStartTime, *tEndTime);
        return HI_FAILURE;
    }
    else
        AV_PRINT("backup time tStartTime=%ld,tEndTime=%ld", *tStartTime, *tEndTime);
    return HI_SUCCESS;
}
HI_S32 HI3521_CreateBkFileDir()
{
    HI_CHAR szBkFileDir[64];
    HI_CHAR cBusId[BUS_ID_LEN] = {0};
    HI3521_GetBusId(cBusId,BUS_ID_LEN);
    MK_USB_BK_FILE_DIR(szBkFileDir, cBusId);
    if (access(szBkFileDir, F_OK) != 0)
    {
        if (0 != mkdir(szBkFileDir, 0777))
        {
            return HI_FAILURE;
        }
    }
    return HI_SUCCESS;
}
HI_S32 HI3521_CheckUsbExit(void)
{
    FILE *fp = NULL;
    char ReadBuf[1024 * 5] = {0};
    int iRet = -1;
    char *p = NULL;
    int UsbFlag = 0; /*四个硬盘挂载标志*/
    fp = fopen("/proc/mounts", "r");
    if(fp == NULL)
    {
        perror("UI: in SYS_CheckUsbExits, fopen error!");
        return -1;
    }
    iRet = fread(ReadBuf, 1, 1024 * 5, fp);
    if(iRet < 0)
    {
        perror("UI: in detect_hdd_mount, fread error!");
        fclose(fp);

        return -1;
    }
    p = strstr(ReadBuf, "/mnt/usb");
    if(p != NULL)
    {
        UsbFlag = HI_SUCCESS;
    }
    else
    {
        UsbFlag = HI_FAILURE;
    }
    fclose(fp);
    return UsbFlag;
}
HI_S32 HI3521_ChkBkChnCnt(REC_COPY_T *pstBK)
{
    if ( pstBK->s32ChnCnt <0 || pstBK->s32ChnCnt >APP_VDEC_CHNCNT)
    {
        AV_PRINT("HI3531_ChkUsbBkChn chn error");
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}
HI_S32 HI3521_UpdateBkFileHead(STOR_FILE_BLK_HEAD_T *pstFileBlkHead, time_t tStartTime, time_t tEndTime,
                               HI_U32 s32EndPosSeek)
{
    HI_U32 timeInterval = 0;
    HI3521_ConvertTime2LocaltimeStr2((HI_CHAR *)"updateBkFileTime", tStartTime, tEndTime);
    AV_PRINT("HI3521_UpdateBkFileHead tStartTime(%d) tEndTime(%d) endPos(%d)",
             (HI_S32)tStartTime,(HI_S32)tEndTime,s32EndPosSeek);
    pstFileBlkHead->frame_data_end_pos = s32EndPosSeek;
    pstFileBlkHead->reserve = tStartTime;
    timeInterval = tEndTime - tStartTime;
    pstFileBlkHead->last_frame_ts_offset = timeInterval * 1000;
    return HI_SUCCESS;
}
HI_S32 HI3521_SetBkFileIFrameList(FILE* pFile, FILE_IFRAME_LIST_T *pstIFrameList)
{
    HI_S32 s32Len = 0;
    HI_S32 s32Ret = 0;
    if ((NULL == pFile) || (NULL == pstIFrameList))
    {
        AV_PRINT("Input NULL pointer");
        return HI_FAILURE;
    }
    s32Ret = fseek(pFile, BKFILE_IFRAME_START_POS, SEEK_SET);
    if (s32Ret != 0)
    {
        AV_PRINT("fseek err(%s)", ERR_STR);
        return HI_FAILURE;
    }
    s32Len = fwrite(pstIFrameList, 1, sizeof(FILE_IFRAME_LIST_T), pFile);
    if (s32Len != sizeof(FILE_IFRAME_LIST_T))
    {
        AV_PRINT("fwrite err(%s)", ERR_STR);
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}
HI_S32 HI3521_UpdateBkRecFileInfo(FILE *pRecFileFp, FILE_IFRAME_LIST_T *pstIFrameList,
                                  STOR_FILE_BLK_HEAD_T *pstFileHead)
{
    HI_S32 s32Ret = 0;
    if (NULL == pRecFileFp)
    {
        AV_PRINT("pRecFileFp para invalid.");
        return HI_FAILURE;
    }
    if (NULL == pstIFrameList)
    {
        AV_PRINT("pstIFrameList para invalid.");
        return HI_FAILURE;
    }
    if (NULL == pstFileHead)
    {
        AV_PRINT("pstFileHead para invalid.");
        return HI_FAILURE;
    }
#ifndef STORE_DEBUG
    STOR_DebugIdxEntry(pstIFrameList);
#endif
    s32Ret = HI3521_SetBkFileIFrameList(pRecFileFp, pstIFrameList);
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT("STOR_SetFileIFrameList err!");
        return HI_FAILURE;
    }
#ifndef STORE_DEBUG
    STOR_DebugFileBlkHead(pstFileHead);
#endif
    s32Ret = STOR_SetVideoFileHeadInfo(pRecFileFp, pstFileHead, STOR_IDX_BLK_HEAD_SEEK);
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT("STOR_SetVideoFileHeadInfo err!");
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}
HI_S32 HI3521_InitUsbBkFile(FILE *pRecFile, STOR_FILE_BLK_HEAD_T *pstFileBlkHead,
                            FILE_IFRAME_LIST_T **ppstIFrameList, HI_S32 s32StartTime)
{
    HI_S32 s32Ret = 0;
    FILE_IFRAME_LIST_T *pstIFrameList = NULL;
    STOR_FILE_INFO_T  stVideoFileInfo;
    STOR_FILE_INFO_T *pstVideoFileInfo = &stVideoFileInfo;
    if ((NULL == pRecFile) || (NULL == ppstIFrameList))
    {
        AV_PRINT("Input para invalid.");
        return HI_FAILURE;
    }
    memset(pstFileBlkHead, 0, sizeof(STOR_FILE_BLK_HEAD_T));
    pstFileBlkHead->frame_index_start_pos = BKFILE_IFRAME_START_POS;  /*I帧位置放在1M位置 */
    pstFileBlkHead->frame_data_start_pos = BKFILE_FRAME_DATA_START_POS;
    pstFileBlkHead->frame_data_end_pos = BKFILE_FRAME_DATA_START_POS;
    pstFileBlkHead->file_info_start_pos = STOR_FILE_INFO_START_ADDR;
    pstFileBlkHead->reserve = s32StartTime;
    s32Ret = STOR_SetVideoFileHeadInfo(pRecFile, pstFileBlkHead, STOR_IDX_BLK_HEAD_SEEK);
    if (HI_SUCCESS != s32Ret)
    {
        return HI_FAILURE;
    }
    memset(&stVideoFileInfo, 0, sizeof(stVideoFileInfo));
    stVideoFileInfo.frame_rate = g_iVideoFrameRate;
    stVideoFileInfo.pixelwidth = 1280;
    stVideoFileInfo.pixelheight = 720;
    stVideoFileInfo.tv_sec = time((time_t *)NULL);
    strncpy(stVideoFileInfo.company, "tiamaes", 7);
    strncpy(stVideoFileInfo.device_name, "TM8721", 6);
    s32Ret = STOR_SetVideoFileInfo(pRecFile, pstVideoFileInfo);
    if (HI_SUCCESS != s32Ret)
    {
        return HI_FAILURE;
    }
    pstIFrameList = (FILE_IFRAME_LIST_T *)malloc(sizeof(FILE_IFRAME_LIST_T));
    if (NULL == pstIFrameList)
    {
        AV_PRINT("malloc err(%s)", ERR_STR);
        return HI_FAILURE;
    }
    memset(pstIFrameList, 0, sizeof(FILE_IFRAME_LIST_T));
    s32Ret = HI3521_SetBkFileIFrameList(pRecFile, pstIFrameList);
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT("HI3531_SetBkFileIFrameList error!");
        return HI_FAILURE;
    }
    *ppstIFrameList = pstIFrameList;
    return HI_SUCCESS;
}
HI_S32 HI3521_InitBKMutex(SF_BK_ST *pstSFBK)
{
    pthread_cond_init(&pstSFBK->cond, NULL);
    pthread_mutex_init(&pstSFBK->mutex, NULL);
    pstSFBK->bWaitSignal = HI_FALSE;
    pstSFBK->bSendSignal = HI_FALSE;
    return HI_SUCCESS;
}
HI_S32 HI3521_ExitBKMutex(SF_BK_ST *pstSFBK)
{
    pthread_cond_destroy(&pstSFBK->cond);
    pthread_mutex_destroy(&pstSFBK->mutex);
    return HI_SUCCESS;
}
HI_S32 HI3521_WaitBKCond(SF_BK_ST *pstSFBK)
{
    pthread_mutex_lock(&pstSFBK->mutex);
    pstSFBK->bWaitSignal = HI_FALSE;
    pthread_cond_wait(&pstSFBK->cond, &pstSFBK->mutex);
    pstSFBK->bWaitSignal = HI_TRUE;
    pthread_mutex_unlock(&pstSFBK->mutex);
    while (!pstSFBK->bSendSignal)
    {
        //usleep(100000);/*100MS*/
        TM_USleep(0,100000);
    }
    return HI_SUCCESS;
}
HI_S32 HI3521_SignalBKCond(SF_BK_ST *pstSFBK)
{
    pstSFBK->bSendSignal = HI_FALSE;
    while (!pstSFBK->bWaitSignal)
    {
        pthread_mutex_lock(&pstSFBK->mutex);
        pthread_cond_signal(&pstSFBK->cond);
        pthread_mutex_unlock(&pstSFBK->mutex);
        //usleep(100000);/*100MS*/
        TM_USleep(0,100000);
    }
    pstSFBK->bSendSignal = HI_TRUE;
    return HI_SUCCESS;
}
HI_VOID HI3521_WaitTimePBCond(SF_BK_ST *pstSFBK)
{
    struct timeval now;
    struct timespec timeout;
    HI_S32 s32Ret;
    pthread_mutex_lock(&pstSFBK->mutex);
    gettimeofday(&now, NULL);
    timeout.tv_sec = now.tv_sec + 5;
    timeout.tv_nsec = now.tv_usec * 1000;
    s32Ret = 0;
    while (s32Ret != ETIMEDOUT)
    {
        s32Ret = pthread_cond_timedwait(&pstSFBK->cond, &pstSFBK->mutex, &timeout);
    }
    if (s32Ret == ETIMEDOUT)
    {
    }
    pthread_mutex_unlock(&pstSFBK->mutex);
}
HI_S32 HI3521_CreateUsbBkFile(FILE **pFile, REC_COPY_T *pstBK, HI_S32 s32BkChn)
{
    FILE *pDstFile = NULL;
    HI_CHAR szFileName[128];
    HI_CHAR szDirName[64];
    time_t startTime = pstBK->tStartTime;
    HI_S32 year,mon,day;
    HI_S32 hour,min,sec;
    struct tm tmm;
    memset(&tmm, 0, sizeof(struct tm));
    localtime_r(&startTime, &tmm);
    year = tmm.tm_year+1900;
    mon = tmm.tm_mon+1;
    day = tmm.tm_mday;
    hour = tmm.tm_hour;
    min = tmm.tm_min;
    sec = tmm.tm_sec;
    HI_CHAR cBusId[BUS_ID_LEN] = {0};
    HI3521_GetBusId(cBusId,BUS_ID_LEN);
    MK_USB_BK_FILE_DIR(szDirName, cBusId);
    MK_USB_BK_FILE_NAME(szFileName, szDirName,year, mon, day, hour, min, sec, s32BkChn);
    AV_PRINT("MK_USB_BK_FILE_NAME s32Chn(%d)(%s)",s32BkChn,szFileName);
    pDstFile = fopen(szFileName, "w+");
    if (NULL == pDstFile)
    {
        AV_PRINT("HI3521_CreateUsbBkFile failed");
        return HI_FAILURE;
    }
    *pFile = pDstFile;
    return HI_SUCCESS;
}
HI_VOID HI3521_SndUiBkStat(HI_S32 s32BkChn, HI_S32 s32BkStat)
{
    LBK_STAT_ST stLBKStat;
    memset(&stLBKStat, 0, sizeof(LBK_STAT_ST));
    AV_PRINT("HI3521_SndUiBkStat s32BkChn(%d)  s32BkStat(%d)", s32BkChn, s32BkStat);
    stLBKStat.s32BkChn = s32BkChn;
    stLBKStat.s32BkStat = s32BkStat;
    DOMAIN_PackageData(ID_UI, AV_BK_STAT, (HI_U8 *)&stLBKStat, sizeof(LBK_STAT_ST));
    return;
}
HI_S32 HI3521_WriteRecFile2Usb(FILE *pDstFile, HI_U32 s32DstFileSeek, HI_U8 *psrcFrameData, HI_S32 s32SrcFilelen)
{
    fseek(pDstFile, s32DstFileSeek, SEEK_SET);
    fwrite(psrcFrameData, s32SrcFilelen, 1, pDstFile);
    fflush(pDstFile);
    return HI_SUCCESS;
}
unsigned long long mygetLocalTime()
{
    struct timeval tp;
    gettimeofday(&tp, NULL);
    int  m_second = tp.tv_sec;
    int  m_millisecond = tp.tv_usec / 1000;
    return (unsigned long long  )m_second*1000+m_millisecond;
}
HI_S32 HI3521_CopyRecFile2Usb(FILE *pSrcFile, HI_S32 s32BkChn,
                              HI_U32 u32FrameSeek, HI_U32 u32FrameEnd,
                              HI_S32 s32AllIFrameNums, REC_COPY_T *pstBK)
{
    HI_S32 s32Ret = HI_FAILURE;
    HI_S32 s32RdLen = 0;
    HI_S32 s32ExitType = 0;	/*0正常备份退出，1外部事件导致中途退出*/
    FRAME_HEAD_T *pstFrameHead = NULL;
    FILE *pDstFile = NULL;
    HI_U32 u32CurFrameTime = 0;
    HI_U32 u32LastFrameTime = 0;
    HI_U8 u8TmpBuf[307200] = {0};	/*300KB*/

    HI_CHAR tmpTime[64] = {0};
    FILE_IFRAME_LIST_T *pstBkIFrameList = NULL;
    STOR_FILE_BLK_HEAD_T stBkFileBlkHead;
    STOR_FILE_BLK_HEAD_T *pstBkFileBlkHead = &stBkFileBlkHead;
    time_t tBkStartTime = 0;
    HI_S32  s32BkPercent = 0;
    HI_S32 s32ChnStat[APP_VENC_CHNCNT] = {0};
    if ((NULL == pSrcFile) || (NULL == pstBK))
    {
        AV_PRINT("HI3521_CopyRecFile2Usb check para err");
        s32ExitType = 1;
        return s32ExitType;
    }
    memset(&stBkFileBlkHead, 0, sizeof(STOR_FILE_BLK_HEAD_T));
    s32Ret = HI3521_CreateUsbBkFile(&pDstFile, pstBK, s32BkChn);
    if (HI_SUCCESS != s32Ret)
    {
        s32ExitType = 1;
        if(NULL != pDstFile)
        {
            fclose(pDstFile);
            pDstFile = NULL;
        }
        return s32ExitType;
    }
    s32Ret = STOR_CheckUdiskFreespace((HI_CHAR*)"/mnt/usb");
    if (s32Ret == 0)
    {
        AV_PRINT("start backup udisk no space");
        HI3521_SndUiBkStat(s32BkChn, BK_HDD_UDISK_NOSPASE); /*U盘没空间 */
        if(NULL != pDstFile)
        {
            fclose(pDstFile);
            pDstFile = NULL;
        }
        s32ExitType = 1;
        return s32ExitType;
    }
    tBkStartTime = HI3521_GetIFrameTime(pSrcFile, pstBK->tStartTime);
    AV_PRINT("STOR_GetIFrameTime (%d)(%d)--(%d)"
        ,(HI_S32)tBkStartTime,(HI_S32)pstBK->tStartTime
        ,(HI_S32)pstBK->tEndTime);
    HI3521_ConvertTime2LocaltimeStr2((HI_CHAR *)"seek and bk time ", pstBK->tStartTime, tBkStartTime);
    s32Ret = HI3521_InitUsbBkFile(pDstFile, &stBkFileBlkHead, &pstBkIFrameList, tBkStartTime);
    if (s32Ret != HI_SUCCESS)
    {
        AV_PRINT("HI3531_InitUsbBkFile");
        s32ExitType = 1;
        if(NULL != pDstFile)
        {
            fclose(pDstFile);
            pDstFile = NULL;
        }
        if(NULL != pstBkIFrameList)
        {
            free(pstBkIFrameList);
            pstBkIFrameList = NULL;
        }
        return s32ExitType;
    }
    unsigned long long ulTime4 = mygetLocalTime();

    while (HDD_GetHddEvent(DISK_EVENT_USB_BK))
    {

        s32Ret = HI3521_CheckUsbExit();
        if (s32Ret != HI_SUCCESS )
        {
            AV_PRINT("HI3521_CheckUsbExit");
            HI3521_SndUiBkStat(s32BkChn, BK_UDISK_EXIT);
            s32ExitType = 1;
            break;
        }
        if (u32FrameSeek >= u32FrameEnd)
        {
            s32ExitType = 0;
            break;
        }
        if (DISK_STAT_WORKING != HDD_GetDiskFSM())
        {
            s32ExitType = 1;
            HI3521_SndUiBkStat(s32BkChn, BK_HDD_ABSENCE);
            break;
        }
        memset(u8TmpBuf, 0, sizeof(u8TmpBuf));
        pstFrameHead = (FRAME_HEAD_T *)u8TmpBuf;
        fseek(pSrcFile, u32FrameSeek, SEEK_SET);
        s32RdLen = fread(pstFrameHead, 1, sizeof(FRAME_HEAD_T), pSrcFile);
        if (sizeof(FRAME_HEAD_T) != s32RdLen)
        {
            AV_PRINT("fread err(%s)", ERR_STR);
            s32ExitType = 0;
            break;
        }
        if (pstFrameHead->len > 1024*1024)
        {
            HI_U32 u32CurFrameTimeTmp = u32CurFrameTime;
            u32CurFrameTime = STOR_CalTime2(pstFrameHead);
            s32Ret = STOR_GetIFrameSeek(pSrcFile, &u32FrameSeek, u32CurFrameTime+1);
            if (HI_SUCCESS != s32Ret)
            {
                s32ExitType = 0;
                AV_PRINT("Frame length error,rebuild fail!!!!!!!");
                debug_print("Error Frame Head",(char *)pstFrameHead,sizeof(FRAME_HEAD_T));
                u32CurFrameTime = u32CurFrameTimeTmp;
                break;
            }
        }
        u32FrameSeek = u32FrameSeek + pstFrameHead->len + sizeof(FRAME_HEAD_T);
        s32RdLen = fread(u8TmpBuf + sizeof(FRAME_HEAD_T), 1, pstFrameHead->len, pSrcFile);
        if ((s32RdLen < 0) || (pstFrameHead->len != (HI_U32)s32RdLen))
        {
            AV_PRINT("fread err(%s)", ERR_STR);
            continue;
        }
        if (u32CurFrameTime > (HI_U32)pstBK->tEndTime)
        {
            s32ExitType = 0;
            AV_PRINT("chn(%d) copy timeover.", s32BkChn);
            break;
        }
        HI3521_WriteRecFile2Usb(pDstFile, pstBkFileBlkHead->frame_data_end_pos, u8TmpBuf, s32RdLen+sizeof(FRAME_HEAD_T));
        pstBkFileBlkHead->frame_data_end_pos = pstBkFileBlkHead->frame_data_end_pos + pstFrameHead->len+sizeof(FRAME_HEAD_T);
        u32CurFrameTime = STOR_CalTime2(pstFrameHead);
        if (pstFrameHead->type == DATA_VIDEO_IFRAME)
        {
            STOR_UpdateIFrameIdx(pstBkIFrameList, pstFrameHead, pstBkFileBlkHead->frame_data_end_pos, 0, s32BkChn, &s32ChnStat[s32BkChn]);
            HI3521_ConvertTime2LocaltimeStr(u32CurFrameTime, tmpTime);
            if ((IFrameCnt[s32BkChn]++%BK_STAT_SND) == 0)
            {
                s32BkPercent = (IFrameCnt[s32BkChn]*100)/s32AllIFrameNums;
                HI3521_SndUiBkStat(s32BkChn, s32BkPercent);
                s32Ret = STOR_CheckUdiskFreespace((HI_CHAR*)"/mnt/usb");
                if (s32Ret == 0)
                {
                    AV_PRINT("backuping udisk no space");
                    HI3521_SndUiBkStat(s32BkChn, BK_HDD_UDISK_NOSPASE); /*U盘没空间 */
                    s32ExitType = 1;
                    if(NULL != pDstFile)
                    {
                        fclose(pDstFile);
                        pDstFile = NULL;
                    }
                    return s32ExitType;
                }
                unsigned long long ulTime1 = mygetLocalTime();
                AV_PRINT("(%d)(%lld)current Iframetime is(%s)...percent(%d)...IFrameCnt(%d)...AllIFrameNums(%d)"
                         ,s32BkChn,ulTime1 - ulTime4,tmpTime,s32BkPercent,IFrameCnt[s32BkChn],s32AllIFrameNums);
                ulTime4 = ulTime1;
            }
            TM_USleep(0,200*1000);
        }
        if (u32CurFrameTime > u32LastFrameTime)
        {
            u32LastFrameTime = u32CurFrameTime;
            pstBK->tStartTime = u32CurFrameTime; /*该时间会带出去*/
        }
        else
        {
            u32CurFrameTime = pstBK->tStartTime;
        }
    }
    HI3521_UpdateBkFileHead(pstBkFileBlkHead, tBkStartTime, u32CurFrameTime, pstBkFileBlkHead->frame_data_end_pos);
    HI3521_UpdateBkRecFileInfo(pDstFile, pstBkIFrameList, pstBkFileBlkHead);
    if(NULL != pstBkIFrameList)
    {
        free(pstBkIFrameList);
        pstBkIFrameList = NULL;
    }
    if(NULL != pDstFile)
    {
        fclose(pDstFile);
        pDstFile = NULL;
    }
    if (!HDD_GetHddEvent(DISK_EVENT_USB_BK))
    {
        s32ExitType = 1;
    }
    return s32ExitType;
}
HI_S32 HI3521_GetIFrameCount(SF_LIST_ST *pstSFList,time_t tFrameStartTime,time_t tFrameEndTime)
{
    HI_S32 s32FileCnt = 0;
    SF_NAME_ST stSFName;
    HI_S32 s32RecIFrameCnt = 0;
    FILE_IFRAME_LIST_T *pstIFrameList = (FILE_IFRAME_LIST_T *)malloc(sizeof(FILE_IFRAME_LIST_T));
    if (NULL == pstIFrameList)
    {
        return s32RecIFrameCnt;
    }
    FILE *pSrcFile = NULL;
    for (s32FileCnt = 0; s32FileCnt < pstSFList->s32FileSegNum; s32FileCnt++)
    {
        memset(&stSFName, 0, sizeof(stSFName));
        memcpy(&stSFName.szFileName, pstSFList->stFileName[s32FileCnt].szFileName, FILE_NAME_LEN);
        pSrcFile = HI3521_OpenSF(&stSFName);
        if (NULL == pSrcFile)
        {
            continue;
        }

        HI_U32 u32IFrameTime = 0;
        HI_S32 s32IFrameCnt = 0;
        HI_S32 s32IFrameTotalCnt = 0;
        HI_S32 s32Ret = HI_FAILURE;

        STOR_REC_IFRAME_INFO *pstIFRAMEInfo = NULL;

        memset(pstIFrameList, 0, sizeof(FILE_IFRAME_LIST_T));

        s32Ret = STOR_GetFileIFrameList(pSrcFile, pstIFrameList);

        if (HI_SUCCESS != s32Ret)
        {
            fclose(pSrcFile);
            pSrcFile = NULL;
            continue;
        }

        s32IFrameTotalCnt = pstIFrameList->s32Counter;

        if (s32IFrameTotalCnt > STOR_FILE_IFRAME_MAX_NUMS)
        {
            fclose(pSrcFile);
            pSrcFile = NULL;
            continue;
        }

        int iFrameCnt = 0;
        for (s32IFrameCnt = pstIFrameList->s32OldestIndex; s32IFrameCnt < s32IFrameTotalCnt; s32IFrameCnt++)
        {
            pstIFRAMEInfo = &(pstIFrameList->stRecIFrameInfo[s32IFrameCnt]);
            u32IFrameTime = STOR_CalTime1(pstIFRAMEInfo);
            if ((u32IFrameTime >= (HI_U32)tFrameStartTime) && (u32IFrameTime < (HI_U32)tFrameEndTime))
            {
                iFrameCnt++;
            }
            else if(u32IFrameTime>(HI_U32)tFrameEndTime)
            {
                break;
            }
        }
        s32RecIFrameCnt+=iFrameCnt;
        AV_PRINT("Export file(%s) has IFrame (%d)",stSFName.szFileName,iFrameCnt);
        fclose(pSrcFile);
        pSrcFile = NULL;
    }
    return s32RecIFrameCnt;
}
HI_VOID *HI3521_StartBkThd(HI_VOID *pArg)
{
    FILE *pSrcFile = NULL;
    HI_S32 s32Ret = HI_FAILURE;
    HI_U32 u32FrameSeek = 0;
    HI_U32 u32FrameEnd = 0;
    HI_S32 s32ChnCnt = 0;
    HI_S32 s32BkChn = 0;
    HI_S32 s32FileCnt = 0;
    HI_S32 s32TmpFileNums = 0;
    SF_NAME_ST stSFName;
    SF_LIST_ST *pstSFList = NULL;
    STOR_FILE_BLK_HEAD_T stFileBlkHead;
    SF_BK_ST stSFBKarg;
    SF_BK_ST *pstSFBK = (SF_BK_ST *)pArg;
    REC_COPY_T *pstBK = NULL;
    REC_COPY_T tmpStBk[APP_VENC_CHNCNT];
    HI_S32  s32AllIFrameNums = 0;
    memset(tmpStBk, 0, sizeof(REC_COPY_T)*APP_VENC_CHNCNT);
    memcpy(&stSFBKarg.stBK, &pstSFBK->stBK, sizeof(REC_COPY_T));
    pstBK = &stSFBKarg.stBK;
    s32AllIFrameNums = pstBK->tEndTime-pstBK->tStartTime;
    AV_PRINT("@%s pid is %d,tid%d", __FUNCTION__,getpid(),gettid());
    HDD_SetHddEvent(DISK_EVENT_USB_BK, HI_TRUE);
    HI3521_SignalBKCond(pstSFBK);
    for (s32ChnCnt = 0; s32ChnCnt < pstBK->s32ChnCnt; s32ChnCnt++)
    {
        s32BkChn = pstBK->s32Chn[s32ChnCnt];
        tmpStBk[s32BkChn].tStartTime = pstBK->tStartTime;
        tmpStBk[s32BkChn].tEndTime = pstBK->tEndTime;
        pstSFList = HI3521_GetLocalSFList(s32BkChn);
        s32AllIFrameNums = HI3521_GetIFrameCount(pstSFList,tmpStBk[s32BkChn].tStartTime,tmpStBk[s32BkChn].tEndTime);
        AV_PRINT("tid(%d) HI3521_StartBkThd, s32ChnCnt(%d),s32BkChn(%d),s32FileSegNum(%d),s32AllIFrameNums(%d)"
                 , gettid(), pstBK->s32ChnCnt,s32BkChn,pstSFList->s32FileSegNum,s32AllIFrameNums);
        for (s32FileCnt = 0; s32FileCnt < pstSFList->s32FileSegNum; s32FileCnt++)
        {
            memset(&stSFName, 0, sizeof(stSFName));
            memcpy(&stSFName.szFileName, pstSFList->stFileName[s32FileCnt].szFileName, FILE_NAME_LEN);
            s32TmpFileNums += pstSFList->s32FileSegNum;
            AV_PRINT("s32Chn(%d),start HI3521_OpenSF(%s) ",s32BkChn,stSFName.szFileName);
            pSrcFile = HI3521_OpenSF(&stSFName);
            if (NULL == pSrcFile)
            {
                HI3521_SndUiBkStat(s32BkChn, BK_FILEOPEN_FAIL);
                AV_PRINT("chn(%d) file(%d) not find.", s32BkChn, s32FileCnt);
                continue;
            }
            AV_PRINT("s32Chn(%d), HI3521_OpenSF(%s) success",s32BkChn,stSFName.szFileName);
            s32Ret = STOR_GetIFrameSeek(pSrcFile, &u32FrameSeek, tmpStBk[s32BkChn].tStartTime);
            if (HI_SUCCESS != s32Ret)
            {
                s32Ret = STOR_GetIFrameSeek(pSrcFile, &u32FrameSeek, tmpStBk[s32BkChn].tStartTime + 1);
                if (HI_SUCCESS != s32Ret)
                {
                    AV_PRINT("STOR_GetIFrameSeek err");
                }
            }
            s32Ret = STOR_GetVideoFileHeadInfo(pSrcFile, &stFileBlkHead, PART_INDEX_SEEK_HEAD);
            if (HI_SUCCESS != s32Ret)
            {
                if(NULL != pSrcFile)
                {
                    fclose(pSrcFile);
                    pSrcFile = NULL;
                }
                continue;
            }
            u32FrameEnd = stFileBlkHead.frame_data_end_pos;
            s32Ret = HI3521_CopyRecFile2Usb(pSrcFile, s32BkChn, u32FrameSeek, u32FrameEnd,
                                            s32AllIFrameNums,&tmpStBk[s32BkChn]);
            if (1 == s32Ret)
            {
                if (NULL != pSrcFile)
                {
                    fclose(pSrcFile);
                    pSrcFile = NULL;
                }
                HI3521_SndUiBkStat(0xFF, BK_FILEEXPORT_QUIT);
                memset(IFrameCnt, 0, sizeof(int)*APP_VENC_CHNCNT);
                HDD_SetHddEvent(DISK_EVENT_USB_BK, HI_FALSE);
                return (HI_VOID *)HI_SUCCESS;
            }
            if (NULL != pSrcFile)
            {
                fclose(pSrcFile);
                pSrcFile = NULL;
            }
            AV_PRINT("chn(%d) file(%d) copy end.", s32BkChn, s32FileCnt);
        }
        if (s32FileCnt != 0)
        {
            HI3521_SndUiBkStat(s32BkChn, BK_FINISH);
        }
    }
    if (s32TmpFileNums == 0)
    {
        HI3521_SndUiBkStat(0xFF, BK_FILESEARCH_FAIL);
    }
    else
    {
        HI3521_SndUiBkStat(0xFF, BK_FILEEXPORT_QUIT);
    }
    memset(IFrameCnt, 0, sizeof(int)*APP_VENC_CHNCNT);
    HDD_SetHddEvent(DISK_EVENT_USB_BK, HI_FALSE);
    return (HI_VOID *)HI_SUCCESS;
}
HI_S32 HI3521_StartBK(REC_COPY_T *stBK)
{
    pthread_t tid;
    HI_S32 s32Err = HI_SUCCESS;
    HI_S32 s32Ret = HI_SUCCESS;
    SF_BK_ST stSFBK;
    SF_BK_ST *pstSFBK = &stSFBK;
    memcpy(&stSFBK.stBK, stBK, sizeof(REC_COPY_T));
    HI3521_InitBKMutex(pstSFBK);
    s32Ret = pthread_create(&tid, NULL, HI3521_StartBkThd, pstSFBK);
    if (HI_SUCCESS != s32Ret)
    {
        s32Err = HI_FAILURE;
        AV_PRINT("err create HI3521_StartVDecThd!");
    }
    s32Ret = pthread_detach(tid);
    if (HI_SUCCESS != s32Ret)
    {
        s32Err = HI_FAILURE;
        AV_PRINT("err detach HI3521_StartVDecThd!");
    }
    HI3521_WaitBKCond(pstSFBK);
    HI3521_ExitBKMutex(pstSFBK);
    return s32Err;
}
HI_S32 HI3521_LocalBK(REC_COPY_T *pstLBK)
{
    HI_S32 s32StartChn;
    HI_S32 s32MaxChn;
    HI_S32 s32Ret = HI_SUCCESS;
    REC_COPY_T stBK;
    SF_RSP_ST stSFRsp;
    SF_REQ_CMD_ST stSFReq;
    if (NULL == pstLBK)
    {
        AV_PRINT("input NULL pointer");
        return HI_FAILURE;
    }
    memcpy(&stBK, pstLBK, sizeof(REC_COPY_T));
    memset(&stSFReq, 0, sizeof(SF_REQ_CMD_ST));
    if (HDD_GetHddEvent(DISK_EVENT_USB_BK))
    {
        AV_PRINT("USB bk already start!!!");
        return HI_SUCCESS;
    }
    s32Ret = HI3521_ChkUsbBkTime(&stBK.tStartTime, &stBK.tEndTime);
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT("HI3521_ChkUsbBkTime failed");
        HI3521_SndUiBkStat(0xff, BK_TIME_CHECK_ERROR);
        return HI_FAILURE;
    }
    s32Ret = HI3521_ChkBkChnCnt(&stBK);
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT("HI3521_ChkUsbBkChn failed");
        HI3521_SndUiBkStat(0xff, BK_CHN_CHECK_ERROR);
        return HI_FAILURE;
    }
    s32Ret = HI3521_CreateBkFileDir();
    if (HI_SUCCESS != s32Ret)
    {
        AV_PRINT("HI3521_CreateBkFileDir failed");
        HI3521_SndUiBkStat(0xff, BK_DIRCAREATE_FAIL);
        return HI_FAILURE;
    }
    HI3521_ConvertTime2LocaltimeStr2((HI_CHAR *)"ui bk time", stBK.tStartTime, stBK.tEndTime);
    s32MaxChn = stBK.s32ChnCnt;
    for (s32StartChn = 0; s32StartChn < s32MaxChn; s32StartChn++)
    {
        stSFReq.iChnno = stBK.s32Chn[s32StartChn];
        stSFReq.tStartTime = stBK.tStartTime;
        stSFReq.tEndTime = stBK.tEndTime;
        HI3521_InitSFList(stSFReq.iChnno);
        s32Ret = HI3521_SFByTime(&stSFReq, &stSFRsp, HI_TRUE, 0);
        if (HI_SUCCESS != s32Ret)
        {
            AV_PRINT("HI3521_SFByTime chn(%d) failed", stSFReq.iChnno);
        }
    }
    HI3521_StartBK(pstLBK);
    return HI_SUCCESS;
}
