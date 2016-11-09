TARGET_MODULE := tem

ifneq ($(KERNELRELEASE),)
#$(TARGET_MODULE)-objs := dr_code.o
obj-m := $(TARGET_MODULE).o
else
BUILDSYSTEM_DIR:=/lib/modules/$(shell uname -r)/build
PWD:=$(shell pwd)


#ifeq ($(KERNELDIR),)
#KERNELDIR=/lib/modules/$(shell uname -r)/build
#endif

all:
	$(MAKE) -C $(BUILDSYSTEM_DIR) M=$(PWD) modules
clean:
	$(MAKE) -C $(BUILDSYSTEM_DIR) M=$(PWD) clean

load:
	insmod ./$(TARGET_MODULE).ko gpio_pin=2 format=3
unload:
	rmmod ./$(TARGET_MODULE).ko
endif
