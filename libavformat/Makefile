PSPSDK=$(shell psp-config --pspsdk-path)
PSPLIBSDIR=$(PSPSDK)/..
TARGET_LIB = ../libavformat.a

include ../config.mak

OBJS= utils.o cutils.o os_support.o allformats.o

# mux and demuxes
OBJS+=mpeg.o mpegts.o mpegtsenc.o ffm.o crc.o img.o img2.o raw.o rm.o \
      avienc.o avidec.o wav.o mmf.o swf.o au.o gif.o mov.o mpjpeg.o dv.o \
      yuv4mpeg.o 4xm.o flvenc.o flvdec.o movenc.o psxstr.o idroq.o ipmovie.o \
      nut.o wc3movie.o mp3.o westwood.o segafilm.o idcin.o flic.o \
      sierravmd.o matroska.o sol.o electronicarts.o nsvdec.o asf.o asf-enc.o daud.o \
      ogg2.o oggparsevorbis.o oggparsetheora.o oggparseflac.o

# image formats
OBJS+= pnm.o yuv.o png.o jpeg.o gifdec.o sgi.o
# file I/O
OBJS+= avio.o aviobuf.o mod/file.o
OBJS+= framehook.o

ifeq ($(CONFIG_LIBOGG),yes)
OBJS+= ogg.o
endif

INCDIR = .. ../libavcodec ../libavutil $(PSPLIBSDIR)/include
CFLAGS = -O3 -G0 -g -Wall -DHAVE_AV_CONFIG_H -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -D_GNU_SOURCE
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)
	
include $(PSPSDK)/lib/build.mak

