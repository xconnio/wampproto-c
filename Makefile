CMAKE_DIR := cmake-build-debug
NPROC := $(shell nproc 2>/dev/null || sysctl -n hw.ncpu)

.PHONY: setup lint format test build clean

setup:
	sudo apt update
	sudo apt install -y cmake clang-format clang-tidy build-essential libmsgpack-dev libcjson-dev

	mkdir -p deps
	git clone https://github.com/intel/tinycbor.git ./deps/tinycbor -b v0.6.1
	$(MAKE) -C deps/tinycbor -j$$(nproc)
	mkdir -p deps/tinycbor/install/lib
	mkdir -p deps/tinycbor/install/include/tinycbor
	cp deps/tinycbor/lib/libtinycbor.a deps/tinycbor/install/lib/
	cp deps/tinycbor/src/*.h deps/tinycbor/install/include/tinycbor/

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
