#ifeq ($(KERNELRELEASE),)
#export MMZ_SRC_BASE=$(PWD)
#endif

ifeq ($(PARAM_FILE), )
		PARAM_FILE:=../../Makefile.param
		include $(PARAM_FILE)
endif
obj-m := nvp6114_ex.o
nvp6114_ex-objs := nvp6114_drv.o coax_protocol.o video.o motion.o audio.o


EXTRA_CFLAGS += -I$(PWD)/../gpio_i2c_8b

default:
	@make -C $(LINUX_ROOT) M=$(PWD) modules 
#ifeq ($(HIARCH),hi3520D)
#	cp *.ko /nfs/hi3520d/app/ko/extdrv/
#else
#	cp *.ko /nfs/hi3531/b.2/ko/extdrv/
#endif
clean: 
	@make -C $(LINUX_ROOT) M=$(PWD) clean 


