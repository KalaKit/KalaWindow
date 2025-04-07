#!/bin/bash

# Set root folder
ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Set shell script paths
RELEASE_SH="${ROOT_DIR}/build_linux_release.sh"
DEBUG_SH="${ROOT_DIR}/build_linux_debug.sh"

# Confirm if shell script paths are valid
if [[ ! -f "$RELEASE_SH" ]]; then
    echo "[ERROR] Release shell script does not exist: $RELEASE_SH"
    read -r -p "Press enter to exit..."
    exit 1
fi
if [[ ! -f "$DEBUG_SH" ]]; then
    echo "[ERROR] Debug shell script does not exist: $DEBUG_SH"
    read -r -p "Press enter to exit..."
    exit 1
fi

# Compile in release and debug mode
if ! bash "${RELEASE_SH}"; then
    echo "[FATAL] Release build failed!"
    read -r -p "Press enter to exit..."
    exit 1
fi
if ! bash "${DEBUG_SH}"; then
    echo "[FATAL] Debug build failed!"
    read -r -p "Press enter to exit..."
    exit 1
fi

# Set headers folder
HEADER_DIR="${ROOT_DIR}/install-release/include"

# Confirm if header folder is valid
if [[ ! -d "$HEADER_DIR" ]]; then
    echo "[ERROR] Header directory does not exist: $HEADER_DIR"
    read -r -p "Press enter to exit..."
    exit 1
fi

# Set dll paths
RELEASE_DLL="${HEADER_DIR}/../lib/libKalaWindow.so"
DEBUG_DLL="${HEADER_DIR}/../../install-debug/lib/libKalaWindowD.so"

# Confirm if dll paths are valid
if [[ ! -f "$RELEASE_DLL" ]]; then
    echo "[ERROR] Release DLL does not exist: $RELEASE_DLL"
    read -r -p "Press enter to exit..."
    exit 1
fi
if [[ ! -f "$DEBUG_DLL" ]]; then
    echo "[ERROR] Debug DLL does not exist: $DEBUG_DLL"
    read -r -p "Press enter to exit..."
    exit 1
fi

# Set target folder
TARGET_DIR="${ROOT_DIR}/../KalaTestProject/_external_shared/KalaWindow"

if [[ ! -d "$TARGET_DIR" ]]; then
    echo "[ERROR] Target directory does not exist: $TARGET_DIR"
    read -r -p "Press enter to exit..."
    exit 1
fi

# Copy all origin headers to target folder
cd "$HEADER_DIR" || exit
find . -type f \( -name "*.h" -o -name "*.hpp" -o -name "*.inl" \) \
    -exec cp -f --parents {} "$TARGET_DIR" \;
echo "[INFO] Headers copied to target folder!"

# Copy release DLL
mkdir -p "$TARGET_DIR/release"
cp "$RELEASE_DLL" "$TARGET_DIR/release"
echo "[INFO] Copied release DLL to target release folder!"

# Copy debug DLL
mkdir -p "$TARGET_DIR/debug"
cp "$DEBUG_DLL" "$TARGET_DIR/debug"
echo "[INFO] Copied debug DLL to target debug folder!"

echo ""
read -r -p "Press enter to exit..."
echo ""
exit 0
