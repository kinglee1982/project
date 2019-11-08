#通用makefile
#文件目录
DEBUG_DIR=.
SRC_DIR=.
INC_DIR=.

SRC=$(wildcard *.cc )  #源文件
OBJS=$(patsubst %.cpp,$(DEBUG_DIR)/%.o,$(SRC))

#目标文件名
TARGET=led_test
INSTALL_PATH ?= .

#修改编译器
ARCH ?=/opt/hisi-linux/x86-arm/arm-hisiv300-linux/bin/arm-hisiv300-linux-uclibcgnueabi-
CC=$(ARCH)gcc
CPP=$(ARCH)g++
AR=$(ARCH)ar
STRIP=$(ARCH)strip


CFLAGS += -Wall 
LDFLAGS += -lpthread -L. -lcommctl

CFLAGS  += $(foreach dir,$(INC_DIR),-I$(dir))

all:$(TARGET)
$(TARGET): $(OBJS)
	$(CPP) $(OBJS) -o $@ $(CFLAGS) $(LDFLAGS)
	$(STRIP) $(TARGET)
#cp $(TARGET) $(INSTALL_PATH)


$(DEBUG_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CPP) $(CFLAGS) -c $< -o $@ 
	
#@echo $(SRC)
#@echo $(OBJS)

	
clean:
	-rm $(TARGET)

