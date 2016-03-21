.PHONY: all clean test

CXX := g++

SRC_DIR := src
SRC := $(wildcard $(SRC_DIR)/*.cpp)
HEADER := $(wildcard $(SRC_DIR)/*.hpp) $(wildcard $(SRC_DIR)/*.tpp)
OUT := $(SRC:%.cpp=%.o)
BIN := sim

TEST_DIR := $(SRC_DIR)/test
TEST_SRC := $(wildcard $(TEST_DIR)/*.cpp)
TEST_OUT := $(TEST_SRC:%.cpp=%.o)
TEST_HEADER := $(wildcard $(TEST_DIR)/*.hpp)
TEST_BIN := $(TEST_DIR)/test

CFLAGS += -Wall -Wextra -Werror -std=c++17

RELEASE ?= 0
ifeq ($(RELEASE),0)
CFLAGS += -O0 -ggdb
else
CFLAGS += -Ofast -march=native -funsafe-loop-optimizations \
	-Wunsafe-loop-optimizations
LFLAGS +=
endif

all: $(BIN)

clean:
	rm -f $(BIN)
	rm -f $(OUT)

test: $(TEST_BIN)
	$(TEST_BIN)

$(TEST_BIN): $(TEST_OUT)
	$(CXX) -o $@ $^ $(LFLAGS)

%.o: %.cpp $(HEADER)
	$(CXX) -c $< -o $@ $(CFLAGS)

$(BIN): $(OUT)
	$(CXX) -o $@ $^ $(LFLAGS)
