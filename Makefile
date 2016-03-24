.PHONY: all clean test openmp

CXX := g++

SRC_DIR := src
SRC := $(wildcard $(SRC_DIR)/*.cpp)
HEADER := $(wildcard $(SRC_DIR)/*.hpp) $(wildcard $(SRC_DIR)/*.tpp)
OUT := $(SRC:%.cpp=%.o)
BIN := sim

TEST_DIR := $(SRC_DIR)/test
TEST_SRC := $(wildcard $(TEST_DIR)/*.cpp)
TEST_OUT := $(TEST_SRC:%.cpp=%.o)
TEST_HEADER := $(wildcard $(TEST_DIR)/*.hpp) $(wildcard $(TEST_DIR)/*.tpp)
TEST_BIN := $(TEST_DIR)/test

CFLAGS += -Wall -Wextra -Werror
ifeq ($(CXX),g++)
CFLAGS += -std=c++17
else
CFLAGS += -std=c++1z
endif

RELEASE ?= 0
ifeq ($(RELEASE),0)
CFLAGS += -O0 -ggdb
else
# consider adding -funsafe-loop-optimizations, -Wunsafe-loop-optimizations,
# -Wunknown-pragmas for more speed
# gcc has a bug with recognizing diagnostics changes, so we'd have to turn on
# -Wno-unknown-pragmas
CFLAGS += -Ofast -march=native -flto
LDFLAGS += $(CFLAGS) -fwhole-program
endif

all: $(BIN)

openmp: CFLAGS += -DSIM_METHOD=SIM_USE_OPENMP -fopenmp
openmp: LDFLAGS += -fopenmp
openmp: | clean all

clean:
	rm -f $(BIN)
	rm -f $(OUT)
	rm -f $(TEST_BIN)
	rm -f $(TEST_OUT)

test: $(TEST_BIN)
	$(TEST_BIN)

$(TEST_DIR)/%.o: $(TEST_DIR)/%.cpp $(TEST_HEADER) $(HEADER) $(SRC)
	$(CXX) -c $< -o $@ $(CFLAGS)

$(TEST_BIN): $(TEST_OUT)
	$(CXX) -o $@ $^ $(LDFLAGS)

%.o: %.cpp $(HEADER)
	$(CXX) -c $< -o $@ $(CFLAGS)

$(BIN): $(OUT)
	$(CXX) -o $@ $^ $(LDFLAGS)
