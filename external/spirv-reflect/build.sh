#!/bin/sh

# Move file for use with mf, read more at https://github.com/greeenlaser/personal-stash/tree/main/mf

set -e

#
# References
#

KMAKE_ORIGIN=project.kmake

SRC_ORIGIN=..
SRC_TARGET=src

INCLUDE_ORIGIN=..
INCLUDE_TARGET=include

case "$1" in
    --linux)
        BUILD_RELEASE="--compile ${KMAKE_ORIGIN} release-linux"
        BUILD_DEBUG="--compile ${KMAKE_ORIGIN} debug-linux"

        TARGET_REL_DIR=release-linux
        TARGET_DEB_DIR=debug-linux
        ;;
    --windows-gnu)
        BUILD_RELEASE="--compile ${KMAKE_ORIGIN} release-windows-gnu"
        BUILD_DEBUG="--compile ${KMAKE_ORIGIN} debug-windows-gnu"

        TARGET_REL_DIR=release-windows-gnu
        TARGET_DEB_DIR=debug-windows-gnu
        ;;
    --windows)
        BUILD_RELEASE="--compile ${KMAKE_ORIGIN} release-windows"
        BUILD_DEBUG="--compile ${KMAKE_ORIGIN} debug-windows"

        TARGET_REL_DIR=release-windows
        TARGET_DEB_DIR=debug-windows
        ;;
    *)
        echo "Error: Argument must be --linux, --windows-gnu or --windows" >&2
        exit 1
        ;;
esac

#
# Copy dependencies
#

if [ -d "${SRC_TARGET}" ]; then
    rm -rf "${SRC_TARGET}"
fi
mkdir "${SRC_TARGET}"

if [ -d "${INCLUDE_TARGET}" ]; then
    rm -rf "${INCLUDE_TARGET}"
fi
mkdir "${INCLUDE_TARGET}"

mf --f "${SRC_ORIGIN}/spirv_reflect.c" --t "${SRC_TARGET}/spirv_reflect.c"

mf --f "${INCLUDE_ORIGIN}/spirv_reflect.h" --t "${INCLUDE_TARGET}/spirv_reflect.h"
mf --f "${INCLUDE_ORIGIN}/include" --t "${INCLUDE_TARGET}"

#
# Compile
#

kalamake ${BUILD_RELEASE} || exit 1
kalamake ${BUILD_DEBUG} || exit 1

#
# Cleanup
#

rm -rf "${SRC_TARGET}"

if [ -d "${TARGET_REL_DIR}/obj" ]; then
    rm -rf "${TARGET_REL_DIR}/obj"
fi

if [ -d "${TARGET_DEB_DIR}/obj" ]; then
    rm -rf "${TARGET_DEB_DIR}/obj"
fi

mf --o --f "${INCLUDE_TARGET}" --t "${TARGET_REL_DIR}"
mf --o --f "${INCLUDE_TARGET}" --t "${TARGET_DEB_DIR}"

mf --o --f "../LICENSE" --t "${TARGET_REL_DIR}/LICENSE"
mf --o --f "../LICENSE" --t "${TARGET_DEB_DIR}/LICENSE"

rm -rf "${INCLUDE_TARGET}"
