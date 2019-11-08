rmmod mcp4531ctl
rmmod hii2c
cp ../hi_i2c_8b/hii2c.ko ./
insmod hii2c.ko
insmod mcp4531ctl.ko 
./audiogain_ctl_test 
