#!/bin/bash

set -e

SOURCE_DIR="$(pwd)"

BUILD_RELEASE="${SOURCE_DIR}/build-release"
BUILD_DEBUG="${SOURCE_DIR}/build-debug"
INSTALL_RELEASE="${SOURCE_DIR}/install-release"
INSTALL_DEBUG="${SOURCE_DIR}/install-debug"

rm -rf "${BUILD_RELEASE}" "${BUILD_DEBUG}" "${INSTALL_RELEASE}" "${INSTALL_DEBUG}"

mkdir -p "${BUILD_RELEASE}" "${BUILD_DEBUG}"
mkdir -p "${INSTALL_RELEASE}" "${INSTALL_DEBUG}"

build_variant() {
  local build_dir="$1"
  local install_dir="$2"
  local build_type="$3"

  echo ""
  echo "Building ${build_type} (Static)..."
  cmake -B "${build_dir}" -S "${SOURCE_DIR}" \
        -D CMAKE_BUILD_TYPE="${build_type}" \
        -D BUILD_SHARED_LIBS=OFF
  cmake --build "${build_dir}" --config "${build_type}"
  cmake --install "${build_dir}" --prefix "${install_dir}"

  echo "Building ${build_type} (Shared)..."
  cmake -B "${build_dir}" -S "${SOURCE_DIR}" \
        -D CMAKE_BUILD_TYPE="${build_type}" \
        -D BUILD_SHARED_LIBS=ON
  cmake --build "${build_dir}" --config "${build_type}"
  cmake --install "${build_dir}" --prefix "${install_dir}"
}

build_variant "${BUILD_RELEASE}" "${INSTALL_RELEASE}" "Release"
build_variant "${BUILD_DEBUG}" "${INSTALL_DEBUG}" "Debug"

echo ""
echo "All builds completed successfully."
echo "Install folders:"
echo "  - Release: ${INSTALL_RELEASE}"
echo "  - Debug:   ${INSTALL_DEBUG}"
