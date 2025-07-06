.PHONY: format
format:
	clang-format -i */*/*.h
	clang-format -i */*.cpp
