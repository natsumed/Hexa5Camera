#!/bin/bash
set -e

PROJECT_DIR=$(pwd)
BUILD_DIR="$PROJECT_DIR/build"
DOCKERFILE="deploy/docker/Dockerfile-build-ubuntu"
DOCKER_IMAGE="joystick-builder"

# Clean build directory
rm -rf "$BUILD_DIR"
mkdir -p "$BUILD_DIR"

# Build Docker image
docker build -f $DOCKERFILE -t $DOCKER_IMAGE .

# Run build process
docker run --rm \
    --privileged \
    -v "$PROJECT_DIR:/project" \
    -v "$BUILD_DIR:/project/build" \
    $DOCKER_IMAGE /bin/bash -c \
    "cd build && \
    cmake -DCMAKE_PREFIX_PATH=/usr/lib/x86_64-linux-gnu/qt6 -DCMAKE_BUILD_TYPE=Release .. && \
    cmake --build . -- -j$(nproc) && \
    mkdir -p AppDir/usr/bin
    cp JoystickIdentifier AppDir/usr/bin/
    linuxdeploy \
      --appdir AppDir \
      --executable AppDir/usr/bin/JoystickIdentifier \
      --create-desktop-file \
      --icon-file ../deploy/icons/joystickidentifier.png \
      --icon-filename JoystickIdentifier \
      --output appimage"


# Final output
ls -lh $BUILD_DIR/*.AppImage
