all:
	./build.sh

clean:
	rm -f icon.h compact.o mediainfo.o mediainfo-fltk

distclean: clean
	rm -rf fltk

