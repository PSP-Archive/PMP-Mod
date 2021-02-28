PSPSDK=$(shell psp-config --pspsdk-path)
PSPLIBSDIR=$(PSPSDK)/..
TARGET_LIB = ../libavutil.a

include ../config.mak

OBJS= mathematics.o \
      integer.o \
      rational.o \
      intfloat_readwrite.o

INCDIR = .. $(PSPLIBSDIR)/include
CFLAGS = -O3 -G0 -g -Wall -DHAVE_AV_CONFIG_H -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -D_GNU_SOURCE
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)
	
include $(PSPSDK)/lib/build.mak
