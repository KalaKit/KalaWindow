#!/bin/sh

# Move file for use with mf, read more at https://github.com/greeenlaser/personal-stash/tree/main/mf

set -e

#
# References
#

case "$1" in
    --linux)
        LIB_NAME=libkalawindow
        LIB_EXT=a
        LIB_ORIGIN=linux
        ;;
    --win-on-linux)
        LIB_NAME=kalawindow-gnu
        LIB_EXT=lib
        LIB_ORIGIN=win-on-linux
        ;;
    --windows)
        LIB_NAME=kalawindow
        LIB_EXT=lib
        LIB_ORIGIN=windows
        ;;
    *)
        echo "Error: Argument must be --linux, --win-on-linux or --windows" >&2
        exit 1
        ;;
esac

TARGET_NAME=KalaWindow
LIB_DEST=../external-shared/${TARGET_NAME}

#
# Core stuff
#

mf --o --f README.md --t "${LIB_DEST}/README.md"
mf --o --f LICENSE.md --t "${LIB_DEST}/LICENSE.md"
mf --o --f include --t "${LIB_DEST}"
mf --o --f docs --t "${LIB_DEST}"

#
# Binaries
#

mf --o --f "build/debug-${LIB_ORIGIN}/${LIB_NAME}d.${LIB_EXT}" --t "${LIB_DEST}/debug/${LIB_NAME}d.${LIB_EXT}"
mf --o --f "build/release-${LIB_ORIGIN}/${LIB_NAME}.${LIB_EXT}" --t "${LIB_DEST}/release/${LIB_NAME}.${LIB_EXT}"
