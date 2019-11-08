
#ifndef _I2C_H
#define _I2C_H


#define I2C_READ   0x01
#define I2C_WRITE  0x03
typedef unsigned char		byte;

unsigned char i2c_read(unsigned char devaddress, unsigned char address);
void i2c_write(unsigned char devaddress, unsigned char address, unsigned char value);

unsigned char SetMicDigitalPotentiometer(unsigned int volume);
unsigned char ReadMicDigitalPotentiometer(unsigned int *volume);
unsigned char SetAudioOutDigitalPotentiometer(unsigned int volume);
unsigned char ReadAudioOutDigitalPotentiometer(unsigned int *volume);

#endif

