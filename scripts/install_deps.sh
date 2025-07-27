#!/usr/bin/env bash
set -e

# Install system dependencies (generic package manager detection)
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
