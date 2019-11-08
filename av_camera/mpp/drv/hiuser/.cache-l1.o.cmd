cmd_/opt/TM8721/package/drv/hiuser/cache-l1.o := /opt/hisi-linux/x86-arm/arm-hisiv300-linux/bin/arm-hisiv300-linux-uclibcgnueabi-gcc -Wp,-MD,/opt/TM8721/package/drv/hiuser/.cache-l1.o.d  -nostdinc -isystem /opt/hisi-linux/x86-arm/arm-hisiv300-linux/bin/../lib/gcc/arm-hisiv300-linux-uclibcgnueabi/4.8.3/include -I/opt/TM8721/linux-3.10.y/arch/arm/include -Iarch/arm/include/generated  -Iinclude -I/opt/TM8721/linux-3.10.y/arch/arm/include/uapi -Iarch/arm/include/generated/uapi -I/opt/TM8721/linux-3.10.y/include/uapi -Iinclude/generated/uapi -include /opt/TM8721/linux-3.10.y/include/linux/kconfig.h -D__KERNEL__ -mlittle-endian -Iarch/arm/mach-hi3521a/include  -D__ASSEMBLY__ -mabi=aapcs-linux -mno-thumb-interwork -funwind-tables -marm -D__LINUX_ARM_ARCH__=7 -march=armv7-a  -include asm/unified.h -msoft-float       -DMODULE  -c -o /opt/TM8721/package/drv/hiuser/cache-l1.o /opt/TM8721/package/drv/hiuser/cache-l1.S

source_/opt/TM8721/package/drv/hiuser/cache-l1.o := /opt/TM8721/package/drv/hiuser/cache-l1.S

deps_/opt/TM8721/package/drv/hiuser/cache-l1.o := \
    $(wildcard include/config/cpu/v7.h) \
    $(wildcard include/config/cpu/arm926t.h) \
    $(wildcard include/config/cpu/dcache/writethrough.h) \
  /opt/TM8721/linux-3.10.y/arch/arm/include/asm/unified.h \
    $(wildcard include/config/arm/asm/unified.h) \
    $(wildcard include/config/thumb2/kernel.h) \
  include/linux/linkage.h \
  include/linux/compiler.h \
    $(wildcard include/config/sparse/rcu/pointer.h) \
    $(wildcard include/config/trace/branch/profiling.h) \
    $(wildcard include/config/profile/all/branches.h) \
    $(wildcard include/config/enable/must/check.h) \
    $(wildcard include/config/enable/warn/deprecated.h) \
    $(wildcard include/config/kprobes.h) \
  include/linux/stringify.h \
  include/linux/export.h \
    $(wildcard include/config/have/underscore/symbol/prefix.h) \
    $(wildcard include/config/modules.h) \
    $(wildcard include/config/modversions.h) \
    $(wildcard include/config/unused/symbols.h) \
  /opt/TM8721/linux-3.10.y/arch/arm/include/asm/linkage.h \
  include/linux/init.h \
    $(wildcard include/config/broken/rodata.h) \
  include/linux/types.h \
    $(wildcard include/config/uid16.h) \
    $(wildcard include/config/lbdaf.h) \
    $(wildcard include/config/arch/dma/addr/t/64bit.h) \
    $(wildcard include/config/phys/addr/t/64bit.h) \
    $(wildcard include/config/64bit.h) \
  include/uapi/linux/types.h \
  arch/arm/include/generated/asm/types.h \
  /opt/TM8721/linux-3.10.y/include/uapi/asm-generic/types.h \
  include/asm-generic/int-ll64.h \
  include/uapi/asm-generic/int-ll64.h \
  arch/arm/include/generated/asm/bitsperlong.h \
  include/asm-generic/bitsperlong.h \
  include/uapi/asm-generic/bitsperlong.h \
  /opt/TM8721/linux-3.10.y/arch/arm/include/asm/assembler.h \
    $(wildcard include/config/cpu/feroceon.h) \
    $(wildcard include/config/trace/irqflags.h) \
    $(wildcard include/config/smp.h) \
    $(wildcard include/config/cpu/use/domains.h) \
  /opt/TM8721/linux-3.10.y/arch/arm/include/asm/ptrace.h \
    $(wildcard include/config/arm/thumb.h) \
  /opt/TM8721/linux-3.10.y/arch/arm/include/uapi/asm/ptrace.h \
    $(wildcard include/config/cpu/endian/be8.h) \
  /opt/TM8721/linux-3.10.y/arch/arm/include/asm/hwcap.h \
  /opt/TM8721/linux-3.10.y/arch/arm/include/uapi/asm/hwcap.h \
  /opt/TM8721/linux-3.10.y/arch/arm/include/asm/domain.h \
    $(wildcard include/config/io/36.h) \
  /opt/TM8721/linux-3.10.y/arch/arm/include/asm/opcodes-virt.h \
  /opt/TM8721/linux-3.10.y/arch/arm/include/asm/opcodes.h \
    $(wildcard include/config/cpu/endian/be32.h) \

/opt/TM8721/package/drv/hiuser/cache-l1.o: $(deps_/opt/TM8721/package/drv/hiuser/cache-l1.o)

$(deps_/opt/TM8721/package/drv/hiuser/cache-l1.o):
