.PHONY: all clean

CXX := g++

SRC_DIR := src
SRC := $(wildcard $(SRC_DIR)/*.cpp)
HEADER := $(wildcard $(SRC_DIR)/*.hpp) $(wildcard $(SRC_DIR)/*.tpp)
OUT := $(SRC:%.cpp=%.o)
BIN := sim

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

%.o: %.cpp $(HEADER)
	$(CXX) -c $< -o $@ $(CFLAGS)

$(BIN): $(OUT)
	$(CXX) -o $@ $^ $(LFLAGS)
