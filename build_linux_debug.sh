#!/bin/bash

# Set the root folder as the location of this script
PROJECT_ROOT="$(dirname "$(readlink -f "$0")")"
BUILD_DIR="$PROJECT_ROOT/build-debug"
INSTALL_DIR="$PROJECT_ROOT/install-debug"

# Record start time
TIME_START=$(date +%T)

cd "$PROJECT_ROOT"

# Always start with a clean build and install directory
echo "[INFO] Deleting build directory"
rm -rf "$BUILD_DIR"
echo "[INFO] Deleting install directory"
rm -rf "$INSTALL_DIR"
mkdir -p "$BUILD_DIR" "$INSTALL_DIR"

cmake --preset=linux-debug
if [ $? -ne 0 ]; then
    echo "[ERROR] Configuration failed."
    if [[ "$1" == "pause" ]]; then
		read -r -p "Press enter to exit..."'
	fi
    exit 1
fi

cmake --build --preset=linux-debug -- -j$(nproc)
if [ $? -ne 0 ]; then
    echo "[ERROR] Build failed."
    if [[ "$1" == "pause" ]]; then
		read -r -p "Press enter to exit..."'
	fi
    exit 1
fi

#Don't install executables
cmake --install build-debug
if [ $? -ne 0 ]; then
	echo "[ERROR] Install failed."
    if [[ "$1" == "pause" ]]; then
		read -r -p "Press enter to exit..."'
	fi
    exit 1
fi

# Record end time
TIME_END=$(date +%T)

# Success message
echo "[SUCCESS] Project built and installed successfully."
echo "---------------------------------------------"
echo "Shared library: $INSTALL_DIR/lib/"
echo "Include headers: $INSTALL_DIR/include"
echo "Build duration: $TIME_START - $TIME_END"
echo "---------------------------------------------"

# Pause to allow to review the output
if [[ "$1" == "pause" ]]; then
	read -r -p "Press enter to exit..."'
fi
exit 0
