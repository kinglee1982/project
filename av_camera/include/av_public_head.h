

#ifndef __AV_PUBLIC_HEAD_H__
#define __AV_PUBLIC_HEAD_H__

//#include <string>

extern "C"
{
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <sys/un.h>
#include <sys/syscall.h>
#include <sys/mount.h>
#include <sys/statfs.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdarg.h>
#include <linux/fb.h>
#include <assert.h>
#include <math.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <netdb.h>
#include <netinet/tcp.h>
#include <linux/unistd.h>
#include <dirent.h>

#include "hi_comm_adec.h"
#include "hi_comm_aenc.h"
#include "hi_comm_ai.h"
#include "hi_comm_aio.h"
#include "hi_comm_ao.h"
#include "hi_comm_sys.h"
#include "hi_comm_vb.h"
#include "hi_comm_vdec.h"
#include "hi_comm_venc.h"
#include "hi_comm_vi.h"
#include "hi_comm_video.h"
#include "hi_comm_vo.h"
#include "hi_common.h"
#include "hi_debug.h"
#include "hi_errno.h"
#include "hi_type.h"
#include "hifb.h"
#include "mpi_adec.h"
#include "mpi_aenc.h"
#include "mpi_ai.h"
#include "mpi_ao.h"
#include "mpi_sys.h"
#include "mpi_vb.h"
#include "mpi_vdec.h"
#include "mpi_venc.h"
#include "mpi_vi.h"
#include "mpi_vo.h"
#include "hi_comm_pciv.h"
#include "hi_tde_api.h"
#include "hi_comm_ive.h"
#include "hi_comm_rc.h"
#include "hi_comm_region.h"
#include "hi_comm_vda.h"
#include "hi_comm_vpss.h"
#include "hi_comm_hdmi.h"
#include "mpi_ive.h"
#include "mpi_region.h"
#include "mpi_vda.h"
#include "mpi_vpss.h"
//#include "mpi_hdmi.h"
#include "av_debug.h"
#include "zlog.h"
//#include "opus_encode.h"
//#include "opus_decode.h"
#include "av_common_31.h"
#include "av_common_21.h"
#ifdef JHY_SDK
#include "mpi_ive.h"
#include "interface.h"
#endif
}


#endif


