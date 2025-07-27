.PHONY: format all build test deps

all: build test

clean:
	rm -rf build

clean-build: clean build

build:
	cmake -S . -B build
	cmake --build build

test:
	ctest --test-dir build

deps:
	bash scripts/install_deps.sh

format:
	clang-format -i */*/*.h
	clang-format -i */*.cpp
