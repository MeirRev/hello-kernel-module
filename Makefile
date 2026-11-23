ARCH = arm64
CROSS_COMPILE = /home/student/rpi-buildroot-2026.08/output/host/bin/aarch64-linux-
ROOTDIR = /home/student/rpi-buildroot-2026.08/output/build/linux-custom
TEST_CC = $(CROSS_COMPILE)gcc
TEST_CFLAGS = -Wall -Wextra

obj-m := hello_module.o

default:
	$(MAKE) -C "$(ROOTDIR)" ARCH=$(ARCH) CROSS_COMPILE="$(CROSS_COMPILE)" M="$(CURDIR)" modules

clean:
	$(MAKE) -C "$(ROOTDIR)" ARCH=$(ARCH) CROSS_COMPILE="$(CROSS_COMPILE)" M="$(CURDIR)" clean



.PHONY: default clean tests

default: tests

test_write test_read: %: %.c
	"$(TEST_CC)" $(TEST_CFLAGS) -o $@ $<