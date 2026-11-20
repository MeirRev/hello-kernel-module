ARCH = arm64
CROSS_COMPILE = /home/student/rpi-buildroot-2026.08/output/host/bin/aarch64-linux-
ROOTDIR = /home/student/rpi-buildroot-2026.08/output/build/linux-custom

obj-m := hello_module.o

default:
	$(MAKE) -C "$(ROOTDIR)" ARCH=$(ARCH) CROSS_COMPILE="$(CROSS_COMPILE)" M="$(CURDIR)" modules

clean:
	$(MAKE) -C "$(ROOTDIR)" ARCH=$(ARCH) CROSS_COMPILE="$(CROSS_COMPILE)" M="$(CURDIR)" clean
