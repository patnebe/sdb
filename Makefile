.PHONY: format all build test deps

all: build test

clean:
	rm -rf build
	rm -rf build_x64

clean-build: clean build
clean-build-x64: clean build-x64

build:
	cmake -S . -B build
	cmake --build build

build-x64:
	cmake -S . -B build_x64 -DCMAKE_TOOLCHAIN_FILE=toolchain_x64.cmake
	cmake --build build_x64

test:
	ctest --test-dir build

test_x64:
	ctest --test-dir build_x64

deps:
	bash scripts/install_deps.sh

format:
	clang-format -i */*/*.h
	clang-format -i */*.cpp
