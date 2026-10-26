# Build and run environment for the Linux and browser builds.
# Ubuntu 26.04 is the first Ubuntu release that packages SDL3.
FROM ubuntu:26.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update \
  && apt-get install -y --no-install-recommends \
    build-essential cmake gdb git ca-certificates python3 xz-utils \
    libsdl3-dev libjpeg-dev libgl1-mesa-dri libegl1 libgl1 \
  && rm -rf /var/lib/apt/lists/*

ARG EMSDK_VERSION=6.0.13
ENV EMSDK=/opt/emsdk
RUN git clone --depth 1 https://github.com/emscripten-core/emsdk.git $EMSDK \
  && cd $EMSDK \
  && ./emsdk install $EMSDK_VERSION \
  && ./emsdk activate $EMSDK_VERSION \
  && rm -rf $EMSDK/downloads
ENV PATH=$EMSDK:$EMSDK/upstream/emscripten:$PATH

# Build the Emscripten ports the game uses now, so a fresh container does
# not download and compile them on every web build.
RUN embuilder build sdl3 libjpeg

# SDL and Mesa expect a private runtime directory.
RUN mkdir -m 700 -p /run/user/0
ENV XDG_RUNTIME_DIR=/run/user/0
