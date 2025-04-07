#!/bin/bash
# This script builds KalaWindow from source using g++ and CMake with Unix Makefiles in Debug mode on Linux.

# Set the root folder as the location of this script
WINDOW_ROOT="$(dirname "$(readlink -f "$0")")"
BUILD_DIR="$WINDOW_ROOT/build-debug"
INSTALL_DIR="$WINDOW_ROOT/install-debug"

# Record start time
TIME_START=$(date +%T)

# Create the build directory if it doesn't exist
mkdir -p "$BUILD_DIR" || { echo "[ERROR] Failed to create build directory: $BUILD_DIR"; exit 1; }
cd "$BUILD_DIR" || { echo "[ERROR] Failed to access build directory: $BUILD_DIR"; exit 1; }

# Configure KalaWindow with CMake using Unix Makefiles in Debug mode
echo "[INFO] Configuring KalaWindow with CMake in Debug mode..."
if ! cmake -G "Unix Makefiles" \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_C_COMPILER=gcc \
  -DCMAKE_CXX_COMPILER=g++ \
  -DBUILD_SHARED_LIBS=ON \
  -DCMAKE_C_FLAGS="-g -O0" \
  -DCMAKE_CXX_FLAGS="-g -O0" \
  -DCMAKE_INSTALL_PREFIX="$INSTALL_DIR" \
  -Wno-dev \
  "$WINDOW_ROOT"; then
    echo "[FATAL] CMake configuration failed!"
    if [[ -t 1 ]]; then
        read -r -p "Press enter to exit..."
    fi
    exit 1
fi

# Build KalaWindow with make
echo "[INFO] Building KalaWindow..."
if ! make -j"$(nproc)"; then
    echo "[FATAL] Build failed!"
    if [[ -t 1 ]]; then  # Only pause if in an interactive terminal
    read -r -p "Press enter to exit..."
    fi
    exit 1
fi

# Install KalaWindow
echo "[INFO] Installing KalaWindow..."
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
echo "[SUCCESS] KalaWindow built and installed successfully in Debug mode."
echo "---------------------------------------------"
echo "Shared library: $INSTALL_DIR/lib/libKalaWindow.so"
echo "Include headers: $INSTALL_DIR/include"
echo "Build duration: $TIME_START - $TIME_END"
echo "---------------------------------------------"

# Pause to allow to review the output
read -r -p "Press enter to exit..."
exit 0
