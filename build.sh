#!/bin/sh
set -e
set -x

pfx="$PWD/fltk-1.3.4/build/usr"
config="$pfx/bin/fltk-config"

if [ ! -d fltk-1.3.4 ]; then
  git clone "https://github.com/darealshinji/fltk-1.3.4"
  mkdir fltk-1.3.4/build
  cd fltk-1.3.4/build
  cmake .. -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX="$PWD/usr" \
    -DOPTION_ABI_VERSION=10304 \
    -DOPTION_BUILD_EXAMPLES=OFF \
    -DOPTION_OPTIM="-O3" \
    -DOPTION_USE_GL=OFF
  make -j4
  make install
  cd -
fi

xxd -i icon.png > icon.h

CFLAGS="-Wall -Wshadow -O3 $(pkg-config --cflags libmediainfo)"

g++ $CFLAGS -I"$pfx/include/FL" $($config --use-images --cflags) -c mediainfo.cpp
g++ $CFLAGS -c compact.cpp
g++ -s -o mediainfo-gui *.o $($config --use-images --ldflags) $(pkg-config --libs libmediainfo)

