
ifeq ($(PARAM_FILE), )
	PARAM_FILE:=../../Makefile.param
	include $(PARAM_FILE)
endif

obj-m := tw2968.o 
tw_2960-y += tw2968.o 

#EXTRA_CFLAGS += -D$(HI_FPGA) 

default:
	@make -C $(LINUX_ROOT) M=$(PWD) modules
clean: 
	@make -C $(LINUX_ROOT) M=$(PWD) clean



