/*************************************************************************
	> File Name: regCtrl.c
	> Author: 
	> Mail:  
	> Created Time: Tue 07 Jun 2016 03:55:02 PM UTC
 ************************************************************************/

#include<stdio.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "tw2968.h"

unsigned char convertCharToHex(char *str)
{
	char hByte=str[0];
	char lByte=str[1];
	unsigned char val = 0;
	
	if ( (hByte >= '0') && (hByte <= '9'))
	{
		val = (hByte-'0') << 4;
	}
	else if ((hByte >= 'a') && (hByte <= 'z'))
	{
		val = ((hByte-'a') << 4) +0xA0;
	}
	else if ((hByte >= 'A') && (hByte <= 'Z'))
	{
		val = ((hByte-'A') << 4) +0xA0;
	}
	
	if ( (lByte >= '0') && (lByte <= '9'))
	{
		val |= lByte-'0';
	}
	else if ((lByte >= 'a') &&(lByte <= 'z'))
	{
		val += (lByte-'a') + 0x0A;
	}
	else if ((lByte >= 'A') && (lByte <= 'Z'))
	{
		val += (lByte - 'A') + 0x0A;
	}
	
	//printf("convert str: %s to hex: 0x%02x\n", str, val);
	return val;
}

int main(int argc , char **argv)
{
	int paraNum=argc;
	if ( argc < 2 )
	{
		
		printf("Usag: <reg addr> <reg val>\n");
		return -1;
	}
	
	
	int fd = open("/dev/tw2968dev", O_RDWR);
	if (fd < 0 )
	{
		perror("open dev fault");
		return 0;
	}
	
	
	unsigned int reg_addr = 0;
	unsigned int reg_val = 0;
	
    //printf("set reg: %s val: %s\n",argv[1], argv[2]);

	reg_addr = convertCharToHex(argv[1]);
	
	if (paraNum == 3)
		reg_val = convertCharToHex(argv[2]);
	

	
	
	tw2968_reg_val reg_val_st;
	
	reg_val_st.RegVal = ((reg_addr << 8 ) | reg_val);
	
	if (paraNum == 3) //set 
	{
		printf("set reg: 0x%02x val: 0x%02x\n",reg_addr, reg_val);
		ioctl(fd, TW2968_WRITE_REG_VAL, &reg_val_st);
	}
	else  //read
	{	
		ioctl(fd, TW2968_READ_REG_VAL, &reg_val_st);
		reg_val = reg_val_st.RegVal & 0xff;
		printf("read reg: 0x%02x val: 0x%02x\n",reg_addr, reg_val);
	}
	
	close(fd);
	
	return 0;
}
