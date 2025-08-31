CMAKE_DIR := cmake-build-debug
NPROC := $(shell nproc 2>/dev/null || sysctl -n hw.ncpu)

.PHONY: lint format test build clean

build:
	cmake -S . -B $(CMAKE_DIR) -DBUILD_TESTS=ON -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
	cmake --build $(CMAKE_DIR) -j$(NPROC)

format: build
	cmake --build $(CMAKE_DIR) --target format

lint: build
	cmake --build $(CMAKE_DIR) --target lint

test: build
	ctest --test-dir $(CMAKE_DIR) --output-on-failure

clean:
	rm -rf $(CMAKE_DIR)
