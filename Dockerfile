FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive \
    QT_QPA_PLATFORM=xcb \
    XDG_RUNTIME_DIR=/tmp/runtime-root

# Install ALL required dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    pkg-config \
    qt6-base-dev \
    qt6-multimedia-dev \
    libqt6opengl6-dev \
    libgl1-mesa-dev \
    libglvnd-dev \
    libdrm-dev \
    mesa-utils \
    libegl1 \
    libxcb1 \
    libx11-xcb1 \
    libxcb-randr0 \
    libxcb-xinerama0 \
    libsdl2-dev \
    xauth \
    # GStreamer dependencies
    libgstreamer1.0-dev \
    libgstreamer-plugins-base1.0-dev \
    gstreamer1.0-plugins-good \
    # Cleanup
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY . .

WORKDIR /app/JoystickIdentifier
RUN mkdir build && cd build && cmake .. && make

CMD ["./build/JoystickIdentifier"]
