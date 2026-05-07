CXX ?= c++
CPPFLAGS ?= -Iinclude
CXXFLAGS ?= -std=c++20 -Wall -Wextra -Wpedantic

SFML_PREFIX ?= $(shell brew --prefix sfml 2>/dev/null || echo /usr/local)
SFML_CPPFLAGS := -I$(SFML_PREFIX)/include
SFML_LDFLAGS := -L$(SFML_PREFIX)/lib -Wl,-rpath,$(SFML_PREFIX)/lib
SFML_LDLIBS := -lsfml-graphics -lsfml-window -lsfml-system

BUILD_DIR := build
COVERAGE_BUILD_DIR := $(BUILD_DIR)/coverage
BIN_DIR := bin
COVERAGE_DIR := coverage

APP := $(BIN_DIR)/algorithm_visualizer
GUI_APP := $(BIN_DIR)/algorithm_visualizer_gui
TEST_APP := $(BIN_DIR)/algorithm_visualizer_tests
TEST_COVERAGE_APP := $(BIN_DIR)/algorithm_visualizer_tests_coverage

SRC := $(shell find src -name '*.cpp' ! -name 'main.cpp' | sort)
APP_SRC := src/main.cpp
GUI_SRC := src/gui_main.cpp
TEST_SRC := tests/test_main.cpp

CORE_SRC := $(filter-out $(GUI_SRC),$(SRC))
CORE_OBJ := $(patsubst src/%.cpp,$(BUILD_DIR)/src/%.o,$(CORE_SRC))
GUI_OBJ := $(BUILD_DIR)/src/gui_main.o
APP_OBJ := $(BUILD_DIR)/src/main.o
TEST_OBJ := $(BUILD_DIR)/tests/test_main.o

COVERAGE_CORE_OBJ := $(patsubst src/%.cpp,$(COVERAGE_BUILD_DIR)/src/%.o,$(CORE_SRC))
COVERAGE_TEST_OBJ := $(COVERAGE_BUILD_DIR)/tests/test_main.o
COVERAGE_CXXFLAGS := $(CXXFLAGS) -O0 -fprofile-arcs -ftest-coverage

all: $(APP) $(GUI_APP)

$(BUILD_DIR) $(BIN_DIR):
	mkdir -p $@

$(BUILD_DIR)/src/%.o: src/%.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/tests/%.o: tests/%.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/src/gui_main.o: src/gui_main.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(SFML_CPPFLAGS) -c $< -o $@

$(COVERAGE_BUILD_DIR)/src/%.o: src/%.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(COVERAGE_CXXFLAGS) -c $< -o $@

$(COVERAGE_BUILD_DIR)/tests/%.o: tests/%.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(COVERAGE_CXXFLAGS) -c $< -o $@

$(APP): $(CORE_OBJ) $(APP_OBJ) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(GUI_APP): $(CORE_OBJ) $(GUI_OBJ) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(SFML_LDFLAGS) $^ $(SFML_LDLIBS) -o $@

$(TEST_APP): $(CORE_OBJ) $(TEST_OBJ) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(TEST_COVERAGE_APP): $(COVERAGE_CORE_OBJ) $(COVERAGE_TEST_OBJ) | $(BIN_DIR)
	$(CXX) $(COVERAGE_CXXFLAGS) $^ -o $@

test: $(TEST_APP)
	./$(TEST_APP)

coverage: $(TEST_COVERAGE_APP)
	mkdir -p $(COVERAGE_DIR)
	./$(TEST_COVERAGE_APP)
	./scripts/generate_coverage.sh $(COVERAGE_BUILD_DIR) $(COVERAGE_DIR)

run-gui: $(GUI_APP)
	./$(GUI_APP)

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR) $(COVERAGE_DIR)

.PHONY: all test coverage clean run-gui
