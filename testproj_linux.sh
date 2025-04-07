#!/bin/bash

# Set root folder
ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Set headers folder
HEADER_DIR="${ROOT_DIR}/install-release/include"

# Set dll paths
RELEASE_DLL="${HEADER_DIR}/../lib/libKalaWindow.so"
DEBUG_DLL="${HEADER_DIR}/../../install-debug/lib/libKalaWindowD.so"

# Compile in release and debug mode
if ! bash "${ROOT_DIR}/build_linux_release.sh"; then
    echo "[FATAL] Release build failed!"
    read -r -p "Press enter to exit..."
    exit 1
fi

if ! bash "${ROOT_DIR}/build_linux_debug.sh"; then
    echo "[FATAL] Debug build failed!"
    read -r -p "Press enter to exit..."
    exit 1
fi

# Set target folder
TARGET_DIR="${ROOT_DIR}/../KalaTestProject/_external_shared/KalaWindow"

# Copy all origin headers to target folder
if [[ -d "$HEADER_DIR" ]]; then
    cd "$HEADER_DIR"
    find . -type f \( -name "*.h" -o -name "*.hpp" -o -name "*.inl" \) \
        -exec cp -f --parents {} "$TARGET_DIR" \;
    echo "[INFO] Headers copied to target folder!"
else
    echo "[ERROR] Header folder not found: $HEADER_DIR"
fi

# Copy release DLL
if [[ -f "$RELEASE_DLL" ]]; then
    cp "$RELEASE_DLL" "$TARGET_DIR/release"
    echo "[INFO] Copied release DLL to target release folder!"
else
    echo "[ERROR] Release DLL not found: $RELEASE_DLL"
fi

# Copy debug DLL
if [[ -f "$DEBUG_DLL" ]]; then
    cp "$DEBUG_DLL" "$TARGET_DIR/debug"
    echo "[INFO] Copied debug DLL to target debug folder!"
else
    echo "[ERROR] Debug DLL not found: $DEBUG_DLL"
fi

echo ""
read -r -p "Press enter to exit..."
echo ""
exit 0
