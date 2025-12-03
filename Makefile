CMAKE_DIR := build
NPROC := $(shell nproc 2>/dev/null || sysctl -n hw.ncpu)

.PHONY: setup lint format test build clean

setup:
	sudo apt update
	sudo apt install -y cmake clang-format clang-tidy build-essential cmake-format

install:
	sudo cmake --install $(CMAKE_DIR)


build:
	cmake -S . -B $(CMAKE_DIR) -DWAMPPROTO_BUILD_TESTS=OFF -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
	cmake --build $(CMAKE_DIR) -j$(NPROC)

format:
	cmake -S . -B $(CMAKE_DIR)at -DCMAKE_BUILD_TYPE=Format -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
	cmake --build $(CMAKE_DIR)at --target format
	cmake-format -i CMakeLists.txt

lint:
	cmake -S . -B $(CMAKE_DIR) -DCMAKE_BUILD_TYPE=Lint -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
	cmake --build $(CMAKE_DIR) --target lint
	cmake-lint CMakeLists.txt

test:
	cmake -S . -B $(CMAKE_DIR) -DWAMPPROTO_BUILD_TESTS=ON -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
	cmake --build $(CMAKE_DIR) -j$(NPROC)
	ctest --test-dir $(CMAKE_DIR) --output-on-failure -V

clean:
	rm -rf $(CMAKE_DIR)
