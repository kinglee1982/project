#通用makefile
#文件目录
BIN_DIR=./bin
OBJ_DIR=./obj
SRC_DIR=./source
INC_DIR := ./include
INC_DIR += ./include/opus
INC_DIR += ./mpp/include

LIB_DIR := ./lib
LIB_DIR += ./mpp/lib_hi3521

SRC=$(wildcard $(SRC_DIR)/*.c )  #源文件
OBJS=$(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC))

GB28181_DIR=$(SRC_DIR)/28181
INC_DIR += ./include/28181
INC_DIR += ./include/28181/opus/include
GB28181SRC+=$(wildcard $(GB28181_DIR)/*.cpp )  #源文件*/
GB28181OBJS+=$(patsubst $(GB28181_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(GB28181SRC))

#目标文件名
TARGET=$(BIN_DIR)/av
INSTALL_PATH ?= ~/nfs

#修改编译器
ARCH ?= arm-hisiv300-linux-
CC=$(ARCH)g++
CPP=$(ARCH)g++
AR=$(ARCH)ar
STRIP=$(ARCH)strip

#视频时间戳使用车载机系统时间
CFLAGS += -DTIME_TAGS
#视频服务器双IP
CFLAGS += -DPLT_PARA_BK
#录像查询方式（按照时间段返回搜索结果）
CFLAGS += -DPLAT_QUERY_REC_TIME
CFLAGS += -DVO1024X768
CFLAGS += -DHI3531
CFLAGS += -DMODE_720P
CFLAGS += -DPLAYBACK
#CFLAGS += -DRUIMINGPING	
CFLAGS += -DPLAT_28181_SVR
CFLAGS += -DREBOOT_DEVICE
CFLAGS += -DPTZ_CTRL_28181
CFLAGS += -DALARM_RING
CFLAGS += -DAV_VDA

#CFLAGS += -DVOIP
#CFLAGS += -DMODE_16D1
CFLAGS += -DVOIP_SAVE_FILE
CFLAGS += -DSTORE_MULTI_THD
CFLAGS += -DMULTI_PB_THD
CFLAGS += -DHD_VDEC
#CFLAGS += -DUSE_NEW_OSD_FUNC
CFLAGS += -DHDD_FSM
CFLAGS += -DUSB_BK
CFLAGS += -DOSD_SHOW
CFLAGS += -DALARM_REC
CFLAGS += -DREC_SAVE_GPS
#CFLAGS += -DAENC_G711
CFLAGS += -DUSE_SYS_TIMEPTS
CFLAGS += -DHDD_SIZE_CHECK
#CFLAGS += -DCHN_VENC
#CFLAGS += -DSTORE_10M
#CFLAGS += -DAENC_PCM
#JHY客流SDK
ifeq ($(KL_SDK),1)
	CFLAGS += -DJHY_SDK
	CFLAGS += -lstereo -l3521alg -live
endif
CFLAGS += -g -Wall -march=armv7-a -mcpu=cortex-a9 -mfloat-abi=softfp -mfpu=vfpv3-d16
#LDFLAGS += -lcommctl -lpthread -lzlog -lmpi -lhdmi -lVoiceEngine -laec -lanr -ljpeg -lresampler -lpciv -ltde -lm -lrt -lcyclequeue -lmp3dec -lVoiceEngine -laec -lresampler -lanr -lvqev2
LDFLAGS += -lcommctl -lpthread -lzlog -lmpi -lhdmi -lVoiceEngine -ljpeg -ltde -lm -lrt -lcyclequeue -lmp3dec -lVoiceEngine -lupvqe -ldnvqe -lmem

#LDFLAGS += -lcommctl
#LDFLAGS += -L/usr/local/lib
LDFLAGS += -L./lib

LIBS_STATIC:=   ./lib/28181/lib28181.a	\
	./lib/28181/libcrypto.a	\
	./lib/28181/libgnutls-openssl.a	\
	./lib/28181/libjrtp.a	\
	./lib/28181/libssl.a	\
	./lib/28181/libeXosip2.a	\
	./lib/28181/libosip2.a	\
   ./lib/28181/libosipparser2.a \
   ./lib/28181/libopus.a 

CFLAGS  += $(foreach dir,$(INC_DIR),-I$(dir))
CFLAGS  += $(foreach dir,$(LIB_DIR),-L$(dir))


all:$(TARGET)
$(TARGET): $(OBJS) $(GB28181OBJS)
	@echo "###############step 1####################"
	@echo $(CFLAGS)
	@echo "###############step 2####################"
	$(CPP) $(OBJS) $(GB28181OBJS) $(LIBS_STATIC) -o $@ $(CFLAGS) $(LDFLAGS)
	@echo "###############step 3####################"
	$(STRIP) $(TARGET)
	@echo "###############step 4####################"
	cp $(TARGET) $(INSTALL_PATH) -f

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@
$(OBJ_DIR)/%.o: $(GB28181_DIR)/%.cpp
	$(CPP) $(CFLAGS) -c $< -o $@

#@echo $(SRC)
#@echo $(OBJS)


clean:
	-rm $(OBJ_DIR)/*
	-rm $(BIN_DIR)/*

