#!/bin/sh

jobs=4
top="$PWD"
pfx="$PWD/fltk/build/usr"
config="$pfx/bin/fltk-config"

set -e
set -x

pkg-config --exists libzen
pkg-config --exists libmediainfo
pkg-config --exists xft  # FLTK looks ugly without it

if [ ! -d fltk ]; then
  git clone "https://github.com/fltk/fltk"
  cd fltk
  git checkout branch-1.3
  mkdir build
  cd build
  cmake .. -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX="$PWD/usr" \
    -DOPTION_ABI_VERSION=10304 \
    -DOPTION_BUILD_EXAMPLES=OFF \
    -DOPTION_OPTIM="-O3" \
    -DOPTION_USE_GL=OFF \
    -DOPTION_USE_SYSTEM_LIBPNG=ON \
    -DOPTION_USE_SYSTEM_ZLIB=ON
  make -j$jobs
  make install
  cd "$top"
fi

xxd -i icon.png > icon.h

CXXFLAGS="-Wall -Wshadow -O3 $(pkg-config --cflags libmediainfo libzen)"

g++ $CXXFLAGS -I"$pfx/include/FL" $($config --use-images --cflags) -c mediainfo.cpp
g++ $CXXFLAGS -c compact.cpp
g++ -s -o mediainfo-fltk *.o $($config --use-images --ldflags) $(pkg-config --libs libmediainfo libzen)

