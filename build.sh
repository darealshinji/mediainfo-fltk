#!/bin/sh
set -e
set -x

top="$PWD"
pfx="$PWD/fltk/build/usr"
config="$pfx/bin/fltk-config"

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
  make -j4
  make install
  cd "$top"
fi

xxd -i icon.png > icon.h

CFLAGS="-Wall -Wshadow -O3 $(pkg-config --cflags libmediainfo)"

g++ $CFLAGS -I"$pfx/include/FL" $($config --use-images --cflags) -c mediainfo.cpp
g++ $CFLAGS -c compact.cpp
g++ -s -o mediainfo-gui *.o $($config --use-images --ldflags) $(pkg-config --libs libmediainfo)

