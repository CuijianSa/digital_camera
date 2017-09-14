
CROSS_COMPILE = arm-linux-
AS		= $(CROSS_COMPILE)as
LD		= $(CROSS_COMPILE)ld
CC		= $(CROSS_COMPILE)gcc
CPP		= $(CC) -E
AR		= $(CROSS_COMPILE)ar
NM		= $(CROSS_COMPILE)nm

STRIP		= $(CROSS_COMPILE)strip
OBJCOPY		= $(CROSS_COMPILE)objcopy
OBJDUMP		= $(CROSS_COMPILE)objdump

export AS LD CC CPP AR NM
export STRIP OBJCOPY OBJDUMP

CFLAGS := -O2 -g -Wall #-Werror
CFLAGS += -I $(shell pwd)/include -I/opt/lib/jpeg/include -I/opt/lib/freetype/include

LDFLAGS := -L/opt/lib/freetype/lib/ -lm -lpthread -ljpeg -lfreetype -lts

export CFLAGS LDFLAGS

TOPDIR := $(shell pwd)
export TOPDIR

TARGET := main


obj-y += main.o
obj-y += display/
obj-y += encoding/
obj-y += fonts/
obj-y += input/
obj-y += debug/
obj-y += render/
obj-y += page/
obj-y += file/

all : 	
	make -C ./ -f $(TOPDIR)/Makefile.build
	$(CC)  -o $(TARGET) built-in.o $(LDFLAGS)

clean:
	rm -f $(shell find -name "*.o")
	rm -f $(TARGET)

distclean:
	rm -f $(shell find -name "*.o")
	rm -f $(shell find -name "*.d")
	rm -f $(TARGET)
	
