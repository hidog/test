mkdir ./ffmpeg ./3rd_source

sudo apt install \
  autoconf \
  automake \
  build-essential \
  cmake \
  git \
  libass-dev \
  libfreetype6-dev \
  libsdl2-dev \
  libtheora-dev \
  libtool \
  libva-dev \
  libvdpau-dev \
  libvorbis-dev \
  libxcb1-dev \
  libxcb-shm0-dev \
  libxcb-xfixes0-dev \
  mercurial \
  pkg-config \
  texinfo \
  wget \
  zlib1g-dev \
  nasm \
  yasm \
  libvpx-dev \
  libopus-dev \
  libx264-dev \
  libmp3lame-dev \
  libfdk-aac-dev 
  
# Install libaom from source.
mkdir -p ~/3rd_source/libaom && \
  cd ~/3rd_source/libaom && \
  git clone https://aomedia.googlesource.com/aom && \
  cmake ./aom && \
  make -j$(nproc) && \
  sudo make install

# Install libx265 from source.
cd ~/3rd_source && \
  git clone https://bitbucket.org/multicoreware/x265_git.git && \
  cd x265_git/build/linux && \
  cmake -G "Unix Makefiles" -DCMAKE_INSTALL_PREFIX="../../../" -DENABLE_SHARED:bool=off ../../source && \
  make -j$(nproc) && \
  make install

wget -O ffmpeg-5.1.2.tar.bz2 https://ffmpeg.org/releases/ffmpeg-5.1.2.tar.bz2 && \
  tar xjvf ffmpeg-5.1.2.tar.bz2 && \
  cd ffmpeg-5.1.2 && \
  PKG_CONFIG_PATH="../ffmpeg/lib/pkgconfig" ./configure \
    --prefix="../ffmpeg" \
    --pkg-config-flags="--static" \
    --extra-cflags="-I$HOME/3rd_source/include" \
    --extra-ldflags="-L$HOME/3rd_source/lib" \
    --extra-libs="-lpthread -lm" \
    --bindir="../ffmpeg" \
    --enable-debug \
    --enable-gpl \
    --enable-libass \
    --enable-libfdk-aac \
    --enable-libmp3lame \
    --enable-libx264 \
    --enable-libx265 \
    --enable-libtheora \
    --enable-libfreetype \
    --enable-libvorbis \
    --enable-libopus \
    --enable-libvpx \
    --enable-libaom \
    --enable-nonfree && \
  make -j$(nproc) && \
  make install && \
  hash -r
