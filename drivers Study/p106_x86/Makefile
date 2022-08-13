MOD := hello
obj-m := $(MOD).o


KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

default:
#	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules
	$(MAKE) -C $(KDIR) M=$(PWD) modules	
clean:
#	rm -rf *.ko
	rm -rf *.mod.*
	rm -rf .*.cmd
	rm -rf *.o
	rm -rf $(MOD).mod
	rm -rf modules.order
	rm -rf Module.symvers

