ifeq ("$(DYNAMIC)", "0")
dynamic_option = no-dynamic
else
dynamic_option =
endif


all:
	./build.sh $(dynamic_option)

clean:
	rm -f icon.h compact.o mediainfo.o mediainfo-fltk

distclean: clean
	rm -rf fltk/build

maintainer-clean: clean
	rm -rf fltk MediaInfoLib

