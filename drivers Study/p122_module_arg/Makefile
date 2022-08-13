MOD := hello
obj-m := $(MOD).o
PWD := $(shell pwd)
#ARCH = 
#KDIR := /lib/modules/$(shell uname -r)/build
ARCH = ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf-
KDIR := /home/ubuntu/udoo_linux_bsp/kernel/3.14.1
default:
	$(MAKE) -C $(KDIR) M=$(PWD) modules $(ARCH)
	cp $(MOD).ko /srv/nfs
clean:
	rm -rf $(MOD).ko
	rm -rf $(MOD)*.o
	rm -rf $(MOD)*mod*
	rm -rf Module.*
	rm -rf modules.*
	rm -rf .$(MOD)*
	rm -rf .Module.*
	rm -rf .modules.*
	rm -rf .tmp_versions
	rm -rf *built-in*

