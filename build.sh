#!/bin/sh

dynamic=0
jobs=4
pfx="fltk/build/usr"
config="$pfx/bin/fltk-config"

if [ "x$1" = "xdynamic" ]; then
  dynamic=1
fi

set -e
set -x

if [ "x$dynamic" != "x1" ]; then
  pkg-config --exists libzen
  pkg-config --exists libmediainfo
fi
pkg-config --exists xft  # FLTK looks ugly without it

if [ "x$dynamic" = "x1" ] && [ ! -d MediaInfoLib ]; then
  git clone --depth 1 https://github.com/MediaArea/MediaInfoLib
fi

if [ ! -d fltk ]; then
  git clone "https://github.com/fltk/fltk"
  cd fltk
  git checkout branch-1.3
  cd ..
fi

if [ ! -d fltk/build ]; then
  mkdir -p fltk/build
  cd fltk/build
  cmake .. -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX="$PWD/usr" \
    -DOPTION_BUILD_EXAMPLES=OFF \
    -DOPTION_OPTIM="-O3" \
    -DOPTION_USE_GL=OFF \
    -DOPTION_USE_SYSTEM_LIBPNG=ON \
    -DOPTION_USE_SYSTEM_ZLIB=ON \
    -DOPTION_ABI_VERSION=10305
  make -j$jobs
  make install
  cd ../..
fi

CXXFLAGS="-Wall -Wshadow -O3 $(pkg-config --cflags libzen)"
LIBS="$(pkg-config --libs libzen)"

if [ "x$dynamic" = "x1" ]; then
  # load libmediainfo dynamically
  CXXFLAGS="$CXXFLAGS -DMEDIAINFO_DYNAMIC -IMediaInfoLib/Source"
else
  # link against libmediainfo (shared/static library)
  CXXFLAGS="$CXXFLAGS $(pkg-config --cflags libmediainfo)"
  LIBS="$(pkg-config --libs libmediainfo) $LIBS"
fi

xxd -i icon.png > icon.h
g++ $CXXFLAGS -I"$pfx/include/FL" $($config --use-images --cflags) -c mediainfo.cpp
g++ $CXXFLAGS -c compact.cpp
g++ -s -o mediainfo-fltk *.o $($config --use-images --ldflags) $LIBS


