.PHONY: format all build test

all: build test

build:
	cmake -S . -B build
	cmake --build build

test:
	ctest --test-dir build

format:
	clang-format -i */*/*.h
	clang-format -i */*.cpp
