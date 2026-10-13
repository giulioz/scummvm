MODULE := engines/policenauts

MODULE_OBJS = \
	archive.o \
	console.o \
	gfx.o \
	location.o \
	lzss.o \
	metaengine.o \
	policenauts.o

# This module can be built as a plugin
ifeq ($(ENABLE_POLICENAUTS), DYNAMIC_PLUGIN)
PLUGIN := 1
endif

# Include common rules
include $(srcdir)/rules.mk

# Detection objects
DETECT_OBJS += $(MODULE)/detection.o
