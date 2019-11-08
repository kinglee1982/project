#!/bin/sh

#date -s 2015-09-09
#date -s "2015-09-09 10:10:10"
# utc=1262304000 --> (gmtime)utc=[2010.01.01 00:00:00]
#date
#date_seconds=`date +%s`
#if [ "$date_seconds" -lt "1262304000" ]; then
#	echo "date err, need auto timing"
#	date -s "2010-01-01 10:10:10"
#else
#	echo "date ok"
#i


httpd -p 12300 -h /app/webroot

# serial
himm 0x120f00f8 0x1
himm 0x120f00fc 0x1
himm 0x120f0100 0x1
himm 0x120f0104 0x1
stty -F /dev/ttyAMA2 ospeed 38400 inpck -icrnl -ixon -opost -isig -icanon -iexten -echo
echo -en "#wdt0A2\r\n" >> /dev/ttyAMA2
# usb sd
#himm 0x120f00f8 0x1
/usr/sbin/test_gpio set dir 0 0 1
/usr/sbin/test_gpio set val 0 0 1
/usr/sbin/test_gpio set dir 0 1 1
/usr/sbin/test_gpio set val 0 1 1

echo "
            _ _ _ _ || _ _ _ _ _ _ _
            \  _  _ ||  _  _ _ ___
            / /__/ \|| |_/
           / __   / || -  _ ___
          / /  / /  ||/ /
  _ _ _ _/ /  /  \_/||  \_ ______
___________\___\____||______________
"

dirlist="/mnt/nand/dvrapp /mnt/sd/dvrapp"
suffixlist="elf sh"
for dir in $dirlist
do
	for suffix in $suffixlist
	do
		applist=`ls ${dir}/*.${suffix}`
			for app in ${applist}
			do
				chmod 777 ${app}
				echo "run ${app}" 
				sleep 5
				${app} &
			done
	done
done

##################################################################################
BINFILE="jhyapp-update.bin"
cd /mnt/nand/jhyapp
rm jhyapp-bk.bin

if [ -f "$BINFILE" ]; 
then
 tar -zxvf /mnt/nand/jhyapp/jhyapp-update.bin -C /tmp
 mv jhyapp-update.bin jhyapp-bk.bin
else
 tar -zxvf /mnt/nand/jhyapp/jhyapp.bin -C /tmp
fi
 

#tar -zxvf /mnt/nand/jhyapp/jhyapp.bin -C /tmp
cd /tmp/jhyapp
./jhyboot.sh

#################################################################################

exit 0

