CMAKE ?= cmake
BUILD_DIR ?= build

.PHONY: all configure test demo benchmark coverage lint clean

all: configure
	$(CMAKE) --build $(BUILD_DIR)

configure:
	$(CMAKE) -S cpp -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=Debug

test: all
	$(CMAKE) --build $(BUILD_DIR) --target lab3_tests
	cd $(BUILD_DIR) && ctest --output-on-failure

demo: all
	$(BUILD_DIR)/lab3_demo

benchmark: all
	$(BUILD_DIR)/lab3_benchmark

coverage:
	$(CMAKE) -S cpp -B $(BUILD_DIR)-coverage -DCMAKE_BUILD_TYPE=Debug -DLAB3_COVERAGE=ON
	$(CMAKE) --build $(BUILD_DIR)-coverage
	find $(BUILD_DIR)-coverage -name '*.gcda' -delete
	cd $(BUILD_DIR)-coverage && ctest --output-on-failure
	$(BUILD_DIR)-coverage/lab3_demo
	$(BUILD_DIR)-coverage/lab3_benchmark
	mkdir -p coverage/cpp
	gcovr -r . --filter 'cpp/(src|include|benchmarks)/' --html-details coverage/cpp/index.html --json-summary coverage/cpp/summary.json $(BUILD_DIR)-coverage
	python3 scripts/check_coverage.py cpp coverage/cpp/summary.json

lint: configure
	clang-tidy -p $(BUILD_DIR) cpp/src/*.cpp cpp/benchmarks/*.cpp

clean:
	$(CMAKE) -E remove_directory $(BUILD_DIR)
	$(CMAKE) -E remove_directory $(BUILD_DIR)-coverage
