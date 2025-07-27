.PHONY: format all build test deps

all: build test

build:
	cmake -S . -B build
	cmake --build build

test:
	ctest --test-dir build

deps:
	sudo apt-get update && sudo apt-get install -y cmake g++ catch2

format:
	clang-format -i */*/*.h
	clang-format -i */*.cpp
