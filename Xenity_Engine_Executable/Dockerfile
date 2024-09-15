# Install the latest version of ubuntu
FROM ubuntu:latest

# Update packets
RUN apt-get update

# Install mc
RUN apt install -y mc

# Sudo is not installed on docker by default
RUN apt install -y sudo

# --------------------------- INSTALL PSP SDK

RUN sudo DEBIAN_FRONTEND=noninteractive apt install -y \
    git gzip libarchive-dev libcurl4 libcurl4-openssl-dev libelf-dev libgpgme-dev libncurses5-dev libreadline-dev libssl-dev \
    libtool-bin libusb-dev m4 make patch pkg-config python3 python3-venv subversion tar tcl texinfo unzip wget xz-utils \
    sudo fakeroot libarchive-tools curl libgmp3-dev libmpfr-dev libmpc-dev python3-pip autoconf automake bison bzip2 cmake doxygen flex g++ gcc

# Install wget and tar to download and open the archive
RUN sudo apt install -y wget tar

# Download pspdev
#RUN cd /usr/local && wget https://github.com/pspdev/pspdev/releases/download/v20230818/pspdev-ubuntu-latest.tar.gz
#RUN cd /usr/local && wget https://github.com/Fewnity/Xenity-Engine-SDK/releases/download/v1/pspdev-ubuntu-latest.tar.gz
RUN cd /usr/local && wget https://github.com/pspdev/pspdev/releases/download/latest/pspdev-ubuntu-latest-x86_64.tar.gz
# Extract files
#RUN cd /usr/local && tar -xvzf pspdev-ubuntu-latest.tar.gz && rm pspdev-ubuntu-latest.tar.gz
RUN cd /usr/local && tar -xvzf pspdev-ubuntu-latest-x86_64.tar.gz && rm pspdev-ubuntu-latest-x86_64.tar.gz

# Add env variables
RUN echo "export PSPDEV=/usr/local/pspdev" >> /root/.bashrc
RUN echo "export PATH=\$PATH:\$PSPDEV/bin" >> /root/.bashrc

# --------------------------- INSTALL PSVITA SDK
# RUN cd /home/ && git clone https://github.com/vitasdk/vdpm
# RUN cd /home/vdpm/ && ./bootstrap-vitasdk.sh

# Download vitasdk
RUN cd /usr/local && wget https://github.com/Fewnity/Xenity-Engine-SDK/releases/download/v1/vitasdk-ubuntu-latest3.tar.gz
# Extract files
RUN cd /usr/local && tar -xvzf vitasdk-ubuntu-latest3.tar.gz && rm vitasdk-ubuntu-latest3.tar.gz

ENV VITASDK /usr/local/vitasdk
ENV PATH {$VITASDK}/bin:$PATH
RUN echo "export VITASDK=/usr/local/vitasdk" >> /root/.bashrc
RUN echo "export PATH=\$VITASDK/bin:\$PATH" >> /root/.bashrc
# RUN cd /home/vdpm/ && ./install-all.sh

# RUN rm -rf /home/vdpm/

# --------------------------- Create build folders
RUN cd /home/ && mkdir XenityBuild && cd XenityBuild && mkdir Source && mkdir build && mkdir include

# Clean temp files
RUN rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*

# Default command
CMD ["/bin/bash"]