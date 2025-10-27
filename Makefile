CMAKE_DIR := cmake-build-debug
NPROC := $(shell nproc 2>/dev/null || sysctl -n hw.ncpu)

.PHONY: setup lint format test build clean

setup:
	sudo apt update
	sudo apt install -y cmake clang-format clang-tidy build-essential libmsgpack-dev libcjson-dev cmake-format libsodium-dev uthash-dev libmbedtls-dev

	mkdir -p deps
	git clone https://github.com/intel/tinycbor.git ./deps/tinycbor -b v0.6.1
	$(MAKE) -C deps/tinycbor -j$$(nproc)
	mkdir -p deps/tinycbor/install/lib
	mkdir -p deps/tinycbor/install/include/tinycbor
	cp deps/tinycbor/lib/libtinycbor.a deps/tinycbor/install/lib/
	cp deps/tinycbor/src/*.h deps/tinycbor/install/include/tinycbor/

build:
	cmake -S . -B $(CMAKE_DIR) -DWAMMPROTO_BUILD_TESTS=OFF -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
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
	cmake -S . -B $(CMAKE_DIR) -DWAMMPROTO_BUILD_TESTS=ON -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
	cmake --build $(CMAKE_DIR) -j$(NPROC)
	ctest --test-dir $(CMAKE_DIR) --output-on-failure -V

clean:
	rm -rf $(CMAKE_DIR)
