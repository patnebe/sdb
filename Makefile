.PHONY: format all build test deps

all: build test

build:
	cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=$(VCPKG_ROOT)/scripts/buildsystems/vcpkg.cmake
	cmake --build build

test:
	ctest --test-dir build

deps:
	bash scripts/install_deps.sh

format:
	clang-format -i */*/*.h
	clang-format -i */*.cpp
