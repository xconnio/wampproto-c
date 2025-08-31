CMAKE_DIR := cmake-build-debug

.PHONY: lint format test build

build:
	cmake -S . -B $(CMAKE_DIR) -DBUILD_TESTS=ON -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
	cmake --build $(CMAKE_DIR) -j $(nproc)

format:
	cmake --build $(CMAKE_DIR) --target format

lint:
	cmake --build $(CMAKE_DIR) --target lint

test:
	ctest --test-dir $(CMAKE_DIR) --output-on-failure

clean:
	rm -rf $(CMAKE_DIR)
