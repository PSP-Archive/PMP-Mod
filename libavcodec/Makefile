PSPSDK=$(shell psp-config --pspsdk-path)
PSPLIBSDIR=$(PSPSDK)/..
TARGET_LIB = ../libavcodec.a

include ../config.mak

OBJS= bitstream.o mod/utils.o mod/mem.o allcodecs.o \
      mod/mpegvideo.o jrevdct.o jfdctfst.o jfdctint.o\
      mpegaudio.o ac3enc.o mjpeg.o resample.o resample2.o mod/dsputil.o \
      motion_est.o imgconvert.o imgresample.o \
      mpeg12.o mpegaudiodec.o pcm.o mod/simple_idct.o \
      ratecontrol.o adpcm.o eval.o error_resilience.o \
      fft.o mdct.o raw.o golomb.o cabac.o\
      dpcm.o adx.o faandct.o parser.o g726.o opt.o \
	  mod/dsputil_mips.o \
      vp3dsp.o h264idct.o rangecoder.o pnm.o h263.o msmpeg4.o mod/h263dec.o dvdsub.o dvbsub.o dvbsubdec.o \
      mod/me_idct.o

ifeq ($(CONFIG_AASC_DECODER),yes)
    OBJS+= aasc.o
endif
ifeq ($(CONFIG_ALAC_DECODER),yes)
    OBJS+= alac.o
endif
ifneq ($(CONFIG_ASV1_DECODER)$(CONFIG_ASV1_ENCODER)$(CONFIG_ASV2_DECODER)$(CONFIG_ASV2_ENCODER),)
    OBJS+= asv1.o
endif
ifeq ($(CONFIG_CINEPAK_DECODER),yes)
    OBJS+= cinepak.o
endif
ifneq ($(CONFIG_CLJR_DECODER)$(CONFIG_CLJR_ENCODER),)
    OBJS+= cljr.o
endif
ifeq ($(CONFIG_CYUV_DECODER),yes)
    OBJS+= cyuv.o
endif
ifneq ($(CONFIG_DVVIDEO_DECODER)$(CONFIG_DVVIDEO_ENCODER),)
    OBJS+= dv.o
endif
ifeq ($(CONFIG_EIGHTBPS_DECODER),yes)
    OBJS+= 8bps.o
endif
ifneq ($(CONFIG_FFV1_DECODER)$(CONFIG_FFV1_ENCODER),)
    OBJS+= ffv1.o
endif
ifeq ($(CONFIG_FLAC_DECODER),yes)
    OBJS+= flac.o
endif
ifeq ($(CONFIG_FLIC_DECODER),yes)
    OBJS+= flicvideo.o
endif
ifeq ($(CONFIG_FOURXM_DECODER),yes)
    OBJS+= 4xm.o
endif
ifeq ($(CONFIG_FRAPS_DECODER),yes)
    OBJS+= fraps.o
endif
ifneq ($(CONFIG_H261_DECODER)$(CONFIG_H261_ENCODER),)
    OBJS+= h261.o
endif
ifneq ($(CONFIG_H264_DECODER)$(CONFIG_SVQ3_DECODER),)
    OBJS+= h264.o
endif
ifneq ($(CONFIG_HUFFYUV_DECODER)$(CONFIG_HUFFYUV_ENCODER)$(CONFIG_FFVHUFF_DECODER)$(CONFIG_FFVHUFF_ENCODER),)
    OBJS+= huffyuv.o
endif
ifeq ($(CONFIG_IDCIN_DECODER),yes)
    OBJS+= idcinvideo.o
endif
ifeq ($(CONFIG_INDEO2_DECODER),yes)
    OBJS+= indeo2.o
endif
ifeq ($(CONFIG_INDEO3_DECODER),yes)
    OBJS+= indeo3.o
endif
ifeq ($(CONFIG_INTERPLAY_VIDEO_DECODER),yes)
    OBJS+= interplayvideo.o
endif
ifneq ($(CONFIG_MSZH_DECODER)$(CONFIG_ZLIB_DECODER)$(CONFIG_ZLIB_ENCODER),)
    OBJS+= lcl.o
endif
ifeq ($(CONFIG_LOCO_DECODER),yes)
    OBJS+= loco.o
endif
ifneq ($(CONFIG_MACE3_DECODER)$(CONFIG_MACE6_DECODER),)
    OBJS+= mace.o
endif
ifeq ($(CONFIG_MSRLE_DECODER),yes)
    OBJS+= msrle.o
endif
ifeq ($(CONFIG_MSVIDEO1_DECODER),yes)
    OBJS+= msvideo1.o
endif
ifneq ($(CONFIG_PNG_DECODER)$(CONFIG_PNG_ENCODER),)
    OBJS+= png.o
endif
ifeq ($(CONFIG_QDRAW_DECODER),yes)
    OBJS+= qdrw.o
endif
ifeq ($(CONFIG_QPEG_DECODER),yes)
    OBJS+= qpeg.o
endif
ifeq ($(CONFIG_QTRLE_DECODER),yes)
    OBJS+= qtrle.o
endif
ifeq ($(CONFIG_RA_144_DECODER),yes)
    OBJS+= ra144.o
endif
ifeq ($(CONFIG_RA_288_DECODER),yes)
    OBJS+= ra288.o
endif
ifeq ($(CONFIG_ROQ_DECODER),yes)
    OBJS+= roqvideo.o
endif
ifeq ($(CONFIG_RPZA_DECODER),yes)
    OBJS+= rpza.o
endif
ifneq ($(CONFIG_RV10_DECODER)$(CONFIG_RV20_DECODER)$(CONFIG_RV10_ENCODER)$(CONFIG_RV20_ENCODER),)
    OBJS+= rv10.o
endif
ifeq ($(CONFIG_SHORTEN_DECODER),yes)
    OBJS+= shorten.o
endif
ifeq ($(CONFIG_SMC_DECODER),yes)
    OBJS+= smc.o
endif
ifneq ($(CONFIG_SNOW_DECODER)$(CONFIG_SNOW_ENCODER),)
    OBJS+= snow.o
endif
ifneq ($(CONFIG_SONIC_DECODER)$(CONFIG_SONIC_ENCODER)$(CONFIG_SONIC_LS_ENCODER),)
    OBJS+= sonic.o
endif
ifneq ($(CONFIG_SVQ1_DECODER)$(CONFIG_SVQ1_ENCODER),)
    OBJS+= svq1.o
endif
ifeq ($(CONFIG_TRUEMOTION1_DECODER),yes)
    OBJS+= truemotion1.o
endif
ifeq ($(CONFIG_TSCC_DECODER),yes)
    OBJS+= tscc.o
endif
ifeq ($(CONFIG_ULTI_DECODER),yes)
    OBJS+= ulti.o
endif
ifneq ($(CONFIG_VC9_DECODER)$(CONFIG_WMV3_DECODER),)
    OBJS+= vc9.o
endif
ifneq ($(CONFIG_VCR1_DECODER)$(CONFIG_VCR1_ENCODER),)
    OBJS+= vcr1.o
endif
ifneq ($(CONFIG_VMDVIDEO_DECODER)$(CONFIG_VMDAUDIO_DECODER),)
    OBJS+= vmdav.o
endif
ifeq ($(CONFIG_VORBIS_DECODER),yes)
    OBJS+= vorbis.o
endif
ifneq ($(CONFIG_VP3_DECODER)$(CONFIG_THEORA_DECODER),)
    OBJS+= vp3.o
endif
ifeq ($(CONFIG_VQA_DECODER),yes)
    OBJS+= vqavideo.o
endif
ifneq ($(CONFIG_WMAV1_DECODER)$(CONFIG_WMAV2_DECODER),)
    OBJS+= wmadec.o
endif
ifeq ($(CONFIG_WNV1_DECODER),yes)
    OBJS+= wnv1.o
endif
ifeq ($(CONFIG_WS_SND1_DECODER),yes)
    OBJS+= ws-snd1.o
endif
ifneq ($(CONFIG_XAN_WC3_DECODER)$(CONFIG_XAN_WC4_DECODER),)
    OBJS+= xan.o
endif
ifeq ($(CONFIG_XL_DECODER),yes)
    OBJS+= xl.o
endif

ifeq ($(CONFIG_LIBOGG),yes)
ifeq ($(CONFIG_LIBVORBIS),yes)
OBJS += oggvorbis.o
endif
endif

INCDIR = .. ../libavutil $(PSPLIBSDIR)/include ./mod ../pmpmod/mod
CFLAGS = -O3 -G0 -g -Wall -DHAVE_AV_CONFIG_H -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -D_GNU_SOURCE
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

include $(PSPSDK)/lib/build.mak
