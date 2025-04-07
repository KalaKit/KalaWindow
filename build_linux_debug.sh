#!/bin/bash

# Set the root folder as the location of this script
PROJECT_ROOT="$(dirname "$(readlink -f "$0")")"
BUILD_DIR="$PROJECT_ROOT/build-debug"
INSTALL_DIR="$PROJECT_ROOT/install-debug"

# Record start time
TIME_START=$(date +%T)

# Always start with a clean build and install directory
rm -rf "$BUILD_DIR" "$INSTALL_DIR"
mkdir -p "$BUILD_DIR" "$INSTALL_DIR"

# Configure Project with CMake using Unix Makefiles in Debug mode
echo "[INFO] Configuring Project with CMake in Debug mode..."
if ! cmake -G "Unix Makefiles" \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_C_COMPILER=gcc \
  -DCMAKE_CXX_COMPILER=g++ \
  -DBUILD_SHARED_LIBS=ON \
  -DCMAKE_C_FLAGS="-g -O0" \
  -DCMAKE_CXX_FLAGS="-g -O0" \
  -DCMAKE_INSTALL_PREFIX="$INSTALL_DIR" \
  -Wno-dev \
  "$PROJECT_ROOT"; then
    echo "[FATAL] CMake configuration failed!"
    if [[ -t 1 ]]; then
        read -r -p "Press enter to exit..."
    fi
    exit 1
fi

# Build Project with make
echo "[INFO] Building Project..."
if ! make -j"$(nproc)"; then
    echo "[FATAL] Build failed!"
    if [[ -t 1 ]]; then  # Only pause if in an interactive terminal
    read -r -p "Press enter to exit..."
    fi
    exit 1
fi

# Install Project
echo "[INFO] Installing Project..."
if ! make install; then
    echo "[FATAL] Install failed!"
    if [[ -t 1 ]]; then  # Only pause if in an interactive terminal
    read -r -p "Press enter to exit..."
    fi
    exit 1
fi

# Record end time
TIME_END=$(date +%T)

# Success message
echo "[SUCCESS] Project built and installed successfully in Debug mode."
echo "---------------------------------------------"
echo "Shared library: $INSTALL_DIR/lib/"
echo "Include headers: $INSTALL_DIR/include"
echo "Build duration: $TIME_START - $TIME_END"
echo "---------------------------------------------"

# Pause to allow to review the output
read -r -p "Press enter to exit..."
exit 0
