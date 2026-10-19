BIN  = simple-mediainfo-gui
OBJS = src/brief.o src/main.o src/mediainfo.o

PKG_CONFIG  ?= pkg-config
CXXFLAGS    ?= -Wall -O3
LDFLAGS     ?= -Wl,--as-needed -Wl,--gc-sections -s
LIBS        ?=

STDCXX      ?= -std=gnu++20
CXXFLAGS    += $(STDCXX)

# find version.h and icon.h
CXXFLAGS    += -I. -I./icon

# fltk
FLTK_CONFIG ?= fltk/build/usr/bin/fltk-config
ifeq ($(shell $(FLTK_CONFIG) --version 2>/dev/null), )
FLTK_CONFIG  = fltk-config
endif
CXXFLAGS    += $(shell $(FLTK_CONFIG) --cxxflags --use-images 2>/dev/null)
LDFLAGS     += $(shell $(FLTK_CONFIG) --ldstaticflags --use-images 2>/dev/null)
LIBS        += $(shell $(FLTK_CONFIG) --libs --use-images 2>/dev/null)

# MediaInfo
CXXFLAGS    += $(shell $(PKG_CONFIG) --cflags-only-I libmediainfo 2>/dev/null)



all: $(BIN)

clean:
	-rm -f $(BIN) $(OBJS)

distclean: clean
	-rm -rf fltk/build/

$(BIN): $(OBJS)
	$(CXX) -o $@ $^ $(LDFLAGS) $(LIBS)

