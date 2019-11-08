#!/bin/sh

echo "run $0 begin!!!";

#I2C(����ΪI2C)
himm 0x120F00E0 0x1; # 0��GPIO12_6  1��I2C_SDA
himm 0x120F00E4 0x1; # 0��GPIO12_7  1��I2C_SCL

#VICAP
himm 0x120F0000 0x2;  # 00:GPIO5_7  01:VI0_CLK   10:VI_ADC_REFCLK0

himm 0x120F0004 0x1;  #  0:GPIO1_0   1:VI0_DAT7
himm 0x120F0008 0x1;  #  0:GPIO1_1   1:VI0_DAT6
himm 0x120F000C 0x1;  #  0:GPIO1_2   1:VI0_DAT5
himm 0x120F0010 0x1;  #  0:GPIO1_3   1:VI0_DAT4
himm 0x120F0014 0x1;  #  0:GPIO1_4   1:VI0_DAT3
himm 0x120F0018 0x1;  #  0:GPIO1_5   1:VI0_DAT2
himm 0x120F001C 0x1;  #  0:GPIO1_6   1:VI0_DAT1
himm 0x120F0020 0x1;  #  0:GPIO1_7   1:VI0_DAT0

himm 0x120F0024 0x2;  # 00:GPIO10_6 01:VI1_CLK   10:VI0_CLK

himm 0x120F0028 0x1   #  0:GPIO2_0   1:VI1_DAT7
himm 0x120F002C 0x1   #  0:GPIO2_1   1:VI1_DAT6
himm 0x120F0030 0x1   #  0:GPIO2_2   1:VI1_DAT5
himm 0x120F0034 0x1   #  0:GPIO2_3   1:VI1_DAT4
himm 0x120F0038 0x1   #  0:GPIO2_4   1:VI1_DAT3
himm 0x120F003C 0x1   #  0:GPIO2_5   1:VI1_DAT2
himm 0x120F0040 0x1   #  0:GPIO2_6   1:VI1_DAT1
himm 0x120F0044 0x1   #  0:GPIO2_7   1:VI1_DAT0

himm 0x120F0048 0x2;  # 00:GPIO6_0   01:VI_ADC_REFCLK0   10:VI1_CLK
himm 0x120F004C 0x2;  # 00:GPIO11_7  01:VI2_CLK          10:VI_ADC_REFCLK1

himm 0x120F0050 0x1   #  0:GPIO3_0   1:VI2_DAT7
himm 0x120F0054 0x1   #  0:GPIO3_1   1:VI2_DAT6
himm 0x120F0058 0x1   #  0:GPIO3_2   1:VI2_DAT5
himm 0x120F005C 0x1   #  0:GPIO3_3   1:VI2_DAT4
himm 0x120F0060 0x1   #  0:GPIO3_4   1:VI2_DAT3
himm 0x120F0064 0x1   #  0:GPIO3_5   1:VI2_DAT2
himm 0x120F0068 0x1   #  0:GPIO3_6   1:VI2_DAT1
himm 0x120F006C 0x1   #  0:GPIO3_7   1:VI2_DAT0

himm 0x120F0070 0x2;  # 00:GPIO10_5   01:VI3_CLK   10:VI2_CLK

himm 0x120F0074 0x1   #  0:GPIO4_0   1:VI3_DAT7
himm 0x120F0078 0x1   #  0:GPIO4_1   1:VI3_DAT6
himm 0x120F007C 0x1   #  0:GPIO4_2   1:VI3_DAT5
himm 0x120F0080 0x1   #  0:GPIO4_3   1:VI3_DAT4
himm 0x120F0084 0x1   #  0:GPIO4_4   1:VI3_DAT3
himm 0x120F0088 0x1   #  0:GPIO4_5   1:VI3_DAT2
himm 0x120F008C 0x1   #  0:GPIO4_6   1:VI3_DAT1
himm 0x120F0090 0x1   #  0:GPIO4_7   1:VI3_DAT0 

himm 0x120F0094 0x2;  # 00:GPIO6_1  01:VI_ADC_REFCLK1     10:VI3_CLK

himm 0x120F00AC 1 #i2S1 00�� GPIO9_3��01�� I2S1_BCLK_RX��10�� I2S2_MCLK��

echo "run $0 end!!!";