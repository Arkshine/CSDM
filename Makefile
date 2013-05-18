#(C)2004-2005 AMX Mod X Development Team
# Makefile written by David "BAILOPAN" Anderson

HLSDK = ../../../amxx/hlsdk
MM_ROOT = ../../../amxx/metamod/metamod

### EDIT BELOW FOR OTHER PROJECTS ###

OPT_FLAGS = -O2 -funroll-loops -s -pipe -fomit-frame-pointer
DEBUG_FLAGS = -g -ggdb3
CPP = gcc-4.1
NAME = csdm_amxx

OBJECTS = amxxmodule.cpp csdm_amxx.cpp csdm_config.cpp csdm_message.cpp	\
	csdm_mm.cpp csdm_natives.cpp csdm_player.cpp csdm_spawning.cpp \
	csdm_tasks.cpp csdm_timer.cpp csdm_util.cpp CSigMngr.cpp

LINK = -lgcc -static-libgcc

INCLUDE = -I. -I$(HLSDK) -I$(HLSDK)/dlls -I$(HLSDK)/engine -I$(HLSDK)/game_shared -I$(HLSDK)/game_shared \
	-I$(MM_ROOT) -Lzlib -I$(HLSDK)/common -Iextra/include

GCC_VERSION := $(shell $(CPP) -dumpversion >&1 | cut -b1)

ifeq "$(GCC_VERSION)" "4"
	OPT_FLAGS += -fvisibility=hidden -fvisibility-inlines-hidden
endif

ifeq "$(DEBUG)" "true"
	BIN_DIR = Debug
	CFLAGS = $(DEBUG_FLAGS)
else
	BIN_DIR = Release
	CFLAGS = $(OPT_FLAGS)
endif

CFLAGS += -DNDEBUG -fPIC -Wno-deprecated -fno-exceptions -DHAVE_STDINT_H -Dstricmp=strcasecmp -fno-rtti -static-libgcc -Dstrcmpi=strcasecmp -D_snprintf=snprintf -Wall -Wno-non-virtual-dtor

ifeq "$(AMD64)" "true"
	BINARY = $(NAME)_amd64.so
	CFLAGS += -DPAWN_CELL_SIZE=64 -DHAVE_I64 -m64 -DAMD64
#	LINK += /lib/libgcc_eh.a /lib/libstdc++.a
else
	BINARY = $(NAME)_i386.so
	CFLAGS += -DPAWN_CELL_SIZE=32 -DJIT -DASM32
	OPT_FLAGS += -march=i586
#	LINK += /lib/libgcc_eh.a /lib/libstdc++.a
endif

OBJ_LINUX := $(OBJECTS:%.cpp=$(BIN_DIR)/%.o)

$(BIN_DIR)/%.o: %.cpp
	$(CPP) $(INCLUDE) $(CFLAGS) -o $@ -c $<

all:
	mkdir -p $(BIN_DIR)
	$(MAKE) csdm

amd64:
	$(MAKE) all AMD64=true

amd64_debug:
	$(MAKE) all AMD64=true DEBUG=true

csdm: $(OBJ_LINUX)
	$(CPP) $(INCLUDE) $(CFLAGS) $(OBJ_LINUX) $(LINK) -shared -ldl -lm -o$(BIN_DIR)/$(BINARY)

debug:	
	$(MAKE) all DEBUG=true

default: all

clean:
	rm -rf Release/*.o
	rm -rf Release/$(BINARY)
	rm -rf Debug/*.o
	rm -rf Debug/$(BINARY)

