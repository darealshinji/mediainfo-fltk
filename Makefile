
all:
	./build.sh

clean:
	rm -f icon.h compact.o mediainfo.o mediainfo-gui

distclean: clean
	rm -rf fltk-1.3.4

