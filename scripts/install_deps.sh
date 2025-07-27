#!/usr/bin/env bash
set -e

# Install vcpkg if not present
if [ -z "$VCPKG_ROOT" ]; then
    VCPKG_ROOT="$(pwd)/vcpkg"
fi
if [ ! -d "$VCPKG_ROOT" ]; then
    git clone https://github.com/microsoft/vcpkg.git "$VCPKG_ROOT"
    cd "$VCPKG_ROOT" && ./bootstrap-vcpkg.sh
fi

# Install system dependencies
if command -v dnf &> /dev/null; then
    sudo dnf install -y cmake gcc-c++ libedit-devel
elif command -v yum &> /dev/null; then
    sudo yum install -y cmake gcc-c++ libedit-devel
elif command -v apt-get &> /dev/null; then
    sudo apt-get update && sudo apt-get install -y cmake g++ libedit-dev
else
    echo "No supported package manager found (dnf, yum, apt-get). Please install dependencies manually." >&2
    exit 1
fi
