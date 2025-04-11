#!/bin/bash

echo "Starting to copy DLLs..."
echo ""

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Origin paths

CRASH_RELEASE="${ROOT_DIR}/KalaCrashHandler/release/libKalaCrashHandler.so"
CRASH_DEBUG="${ROOT_DIR}/KalaCrashHandler/debug/libKalaCrashHandlerD.so"

XDG_RELEASE="${ROOT_DIR}/xdg-shell/release/libxdg-shell.so"
XDG_DEBUG="${ROOT_DIR}/xdg-shell/debug/libxdg-shellD.so"

# Target paths

RELEASE_DLLS="${ROOT_DIR}/../files/external dlls/release"
DEBUG_DLLS="${ROOT_DIR}/../files/external dlls/debug"

# Copy release dlls

cp -f "$CRASH_RELEASE" "$RELEASE_DLLS"
echo "Copied $CRASH_RELEASE to $RELEASE_DLLS"

cp -f "$XDG_RELEASE" "$RELEASE_DLLS"
echo "Copied $XDG_RELEASE to $RELEASE_DLLS"

# Copy debug dlls

cp -f "$CRASH_DEBUG" "$DEBUG_DLLS"
echo "Copied $CRASH_DEBUG to $DEBUG_DLLS"

cp -f "$XDG_DEBUG" "$DEBUG_DLLS"
echo "Copied $XDG_DEBUG to $DEBUG_DLLS"

echo ""
echo "Finished copying DLLs!"

echo ""
read -r -p "Press enter to exit..."
echo ""
exit 0
