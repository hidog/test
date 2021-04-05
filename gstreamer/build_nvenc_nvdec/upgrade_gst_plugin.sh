GST_VERSION=1.16.2

# Install compiler etc.
sudo apt-get install -y \
  autoconf \
  automake \
  autopoint \
  bison \
  flex \
  libtool \
  yasm \
  nasm \
  git-core \
  build-essential \
  gettext

# Install dependencies necessary to build our custom GStreamer build
sudo apt-get install -y \
  libglib2.0-dev \
  libpthread-stubs0-dev \
  libssl-dev \
  liborc-dev \
  libmpg123-dev \
  libmp3lame-dev \
  libsoup2.4-dev \
  libshout3-dev \
  libpulse-dev

sudo apt-get install -y gtk-doc-tools


# Fetch and build GStreamer
#git clone git://anongit.freedesktop.org/git/gstreamer/gstreamer 
#cd gstreamer 
#git checkout $GST_VERSION
#./autogen.sh 
#./configure
#make 
#sudo make install
#cd ..


# Fetch and build gst-plugins-base
git clone git://anongit.freedesktop.org/git/gstreamer/gst-plugins-base
cd gst-plugins-base && \
git checkout $GST_VERSION 
./autogen.sh 
#./configure
make 
#sudo make install
cd ..


# Fetch and build gst-plugins-good
git clone git://anongit.freedesktop.org/git/gstreamer/gst-plugins-good 
cd gst-plugins-good 
git checkout $GST_VERSION
./autogen.sh 
#./configure
make 
#sudo make install
cd ..


# Fetch and build gst-plugins-bad
git clone git://anongit.freedesktop.org/git/gstreamer/gst-plugins-bad 
cd gst-plugins-bad
git checkout $GST_VERSION 
./autogen.sh 
#./configure
make 
#sudo make install
cd ..



# Fetch and build gst-plugins-ugly
git clone git://anongit.freedesktop.org/git/gstreamer/gst-plugins-ugly
cd gst-plugins-ugly 
git checkout $GST_VERSION 
./autogen.sh 
#./configure
make 
#sudo make install
cd ..


