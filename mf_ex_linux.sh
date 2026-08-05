#!/bin/sh

# Move file for use with mf, read more at https://github.com/greeenlaser/personal-stash/tree/main/mf

set -e

#
# References
#

OUT_NAME=KalaWindow-Linux
OUT_VER=1_1_0
OUT_DIR=out/${OUT_NAME}_${OUT_VER}

LIB_NAME=libkalawindow
LIB_EXT=a
LIB_ORIGIN=build/release-linux

IN_README=README.md
IN_LICENSE=LICENSE.md
IN_INCLUDE=include
IN_DOCS=docs

# Extras

DIR_ES=../external-shared
IN_KH=${DIR_ES}/KalaHeaders/include
OUT_KH_NAME=kalaheaders

#
# Core stuff
#

# Always a fresh start
mkdir -p "out"
rm -rf "${OUT_DIR}"
mkdir "${OUT_DIR}"
mkdir "${OUT_DIR}/${OUT_KH_NAME}"

# The base files
mf --f "${IN_README}" --t "${OUT_DIR}/${IN_README}"
mf --f "${IN_LICENSE}" --t "${OUT_DIR}/${IN_LICENSE}"
mf --f "${IN_INCLUDE}" --t "${OUT_DIR}"
mf --f "${IN_DOCS}" --t "${OUT_DIR}"

# The binary
if [ ! -f "${LIB_ORIGIN}/${LIB_NAME}.${LIB_EXT}" ]; then
    printf 'Error: KalaWindow binary %s not found\n' "${LIB_ORIGIN}/${LIB_NAME}.${LIB_EXT}" >&2
    exit 1
fi

mf --f "${LIB_ORIGIN}/${LIB_NAME}.${LIB_EXT}" --t "${OUT_DIR}/${LIB_NAME}.${LIB_EXT}"

# KalaHeaders
if [ ! -d "${IN_KH}" ]; then
    printf 'Error: KalaHeaders include directory not found: %s\n' "${IN_KH}" >&2
    exit 1
fi

for file in "${IN_KH}"/*; do
    name="${file##*/}"
    mf --f "$file" --t "${OUT_DIR}/${OUT_KH_NAME}/$name"
done
