#!/bin/bash

# Set root folder
ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Set shell script paths
RELEASE_SH="${ROOT_DIR}/build_linux_release.sh"
DEBUG_SH="${ROOT_DIR}/build_linux_debug.sh"
COPY_SH="${ROOT_DIR}/_external_shared/copy_linux.sh"

# Confirm if shell script paths are valid
if [[ ! -f "$COPY_SH" ]]; then
    echo "[ERROR] Copy shell script does not exist: $COPY_SH"
    read -r -p "Press enter to exit..."
    exit 1
fi
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

# Copy external shared
if ! bash "${COPY_SH}"; then
    echo "[FATAL] Copy failed!"
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