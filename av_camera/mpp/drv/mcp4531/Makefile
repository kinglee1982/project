
ifeq ($(PARAM_FILE), )
                PARAM_FILE:=../../Makefile.param
                include $(PARAM_FILE)
endif
			
obj-m := mcp4531ctl.o
mcp4531ctl-y += mcp4531.o

EXTRA_CFLAGS += -D$(HI_FPGA) 
 
default:	
	@make -C $(LINUX_ROOT) M=$(PWD) modules
clean:
	@rm *.o  -rf
	@make -C $(LINUX_ROOT) M=$(PWD) clean
