#!/bin/sh

# Move file for use with mf, read more at https://github.com/greeenlaser/personal-stash/tree/main/mf

set -e

#
# References
#

VERSION=1-7-0
BIN_NAME=kalawindow

BUILD_DIR=build
TEMP_DIR=${BUILD_DIR}/temp
EXTERNAL_DIR=external

KMAKE_ORIGIN=project.kmake

KH_DIR=${EXTERNAL_DIR}/kalaheaders
VK_DIR=${EXTERNAL_DIR}/vulkan

if [ ! -d "${EXTERNAL_DIR}" ]; then
    echo "[ERROR] Failed to compile ${BIN_NAME} because '/external' was not found!"
    exit 1
fi

case "$1" in
    --linux)
        BIN_NAME_FRONT=lib
        BIN_NAME_BACK=
        BIN_EXT=.a

        BUILD_RELEASE="--compile ${KMAKE_ORIGIN} release-linux"
        BUILD_DEBUG="--compile ${KMAKE_ORIGIN} debug-linux"

        TEMP_REL_DIR=${TEMP_DIR}/release-linux
        TEMP_DEB_DIR=${TEMP_DIR}/debug-linux

        TARGET_REL_DIR=${BUILD_DIR}/${VERSION}/release-linux
        TARGET_DEB_DIR=${BUILD_DIR}/${VERSION}/debug-linux
        ;;
    --windows-gnu)
        BIN_NAME_FRONT=
        BIN_NAME_BACK=-gnu
        BIN_EXT=.lib

        BUILD_RELEASE="--compile ${KMAKE_ORIGIN} release-windows-gnu"
        BUILD_DEBUG="--compile ${KMAKE_ORIGIN} debug-windows-gnu"

        TEMP_REL_DIR=${TEMP_DIR}/release-windows-gnu
        TEMP_DEB_DIR=${TEMP_DIR}/debug-windows-gnu

        TARGET_REL_DIR=${BUILD_DIR}/${VERSION}/release-windows-gnu
        TARGET_DEB_DIR=${BUILD_DIR}/${VERSION}/debug-windows-gnu
        ;;
    --windows)
        BIN_NAME_FRONT=
        BIN_NAME_BACK=
        BIN_EXT=.lib

        BUILD_RELEASE="--compile ${KMAKE_ORIGIN} release-windows"
        BUILD_DEBUG="--compile ${KMAKE_ORIGIN} debug-windows"

        TEMP_REL_DIR=${TEMP_DIR}/release-windows
        TEMP_DEB_DIR=${TEMP_DIR}/debug-windows

        TARGET_REL_DIR=${BUILD_DIR}/${VERSION}/release-windows
        TARGET_DEB_DIR=${BUILD_DIR}/${VERSION}/debug-windows
        ;;
    *)
        echo "Error: Argument must be --linux, --windows-gnu or --windows" >&2
        exit 1
        ;;
esac

case "$2" in
    --export)
        ;;
    "")
        ;;
    *)
        echo "Error: Second argument must be --export or empty" >&2
        exit 1
        ;;
esac

#
# Compile
#

if [ ! -d "${BUILD_DIR}" ]; then
    mkdir "${BUILD_DIR}"
fi

if [ "$2" = "--export" ]; then
    if [ -d "${TEMP_DIR}" ]; then
        rm -rf "${TEMP_DIR}"
    fi
fi

if [ ! -d "${BUILD_DIR}/${VERSION}" ]; then
    mkdir "${BUILD_DIR}/${VERSION}"
fi

kalamake ${BUILD_RELEASE} || exit

if [ "$2" = "" ]; then
    kalamake ${BUILD_DEBUG} || exit 1
fi

# Release

BIN_REL=${BIN_NAME_FRONT}${BIN_NAME}${BIN_NAME_BACK}${BIN_EXT}

if [ ! -d "${TARGET_REL_DIR}" ]; then
    mkdir "${TARGET_REL_DIR}"
fi

mf --o --f "${TEMP_REL_DIR}/${BIN_REL}" --t "${TARGET_REL_DIR}/${BIN_REL}"

mf --o --f "README.md" --t "${TARGET_REL_DIR}/README.md"
mf --o --f "LICENSE.md" --t "${TARGET_REL_DIR}/LICENSE.md"
mf --o --f "CHANGES.md" --t "${TARGET_REL_DIR}/CHANGES.md"

mf --o --f "docs" --t "${TARGET_REL_DIR}"

mf --o --f "${KH_DIR}" --t "${TARGET_REL_DIR}"

if [ "$1" = "--windows-gnu" ]; then
    mf --o --f "${VK_DIR}" --t "${TARGET_REL_DIR}"
fi

# Debug

BIN_DEB=${BIN_NAME_FRONT}${BIN_NAME}${BIN_NAME_BACK}d${BIN_EXT}

if [ "$2" = "--export" ]; then
    if [ -d "${TARGET_DEB_DIR}" ]; then
        rm -rf "${TARGET_DEB_DIR}"
    fi
else
    if [ -d "${TARGET_DEB_DIR}" ]; then
        rm -rf "${TARGET_DEB_DIR}"
    fi
    mkdir "${TARGET_DEB_DIR}"

    mf --o --f "${TEMP_DEB_DIR}/${BIN_DEB}" --t "${TARGET_DEB_DIR}/${BIN_DEB}"

    mf --o --f "README.md" --t "${TARGET_DEB_DIR}/README.md"
    mf --o --f "LICENSE.md" --t "${TARGET_DEB_DIR}/LICENSE.md"
    mf --o --f "CHANGES.md" --t "${TARGET_DEB_DIR}/CHANGES.md"

    mf --o --f "docs" --t "${TARGET_DEB_DIR}"

    mf --o --f "${KH_DIR}" --t "${TARGET_DEB_DIR}"

    if [ "$1" = "--windows-gnu" ]; then
        mf --o --f "${VK_DIR}" --t "${TARGET_DEB_DIR}"
    fi
fi

#
# Cleanup
#

if [ "$2" = "--export" ]; then
    if [ -d "${TARGET_REL_DIR}/obj" ]; then
        rm -rf "${TARGET_REL_DIR}/obj"
    fi

    if [ -d "${TARGET_DEB_DIR}/obj" ]; then
        rm -rf "${TARGET_DEB_DIR}/obj"
    fi

    if [ -d "${TEMP_DIR}" ]; then
        rm -rf "${TEMP_DIR}"
    fi
fi
