
ifeq ($(PARAM_FILE), )
                PARAM_FILE:=../../Makefile.param
                include $(PARAM_FILE)
endif
			
obj-m := gpio.o
gpio-y +=hi_gpio.o

EXTRA_CFLAGS += -D$(HI_FPGA) 
 
default:	
#	@$(CC) -g -Wall -o hi_gpio.o hi_gpio.c	
	@make -C $(LINUX_ROOT) M=$(PWD) modules
clean:
	@rm *.o 
	@make -C $(LINUX_ROOT) M=$(PWD) clean
