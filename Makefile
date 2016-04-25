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

define do_flags
$(1) ?= 0
ifeq ($$($1),0)
CFLAGS += $(2)
LDFLAGS += $(3)
else
CFLAGS += $(4)
LDFLAGS += $(5)
endif
endef

# consider adding -funsafe-loop-optimizations, -Wunsafe-loop-optimizations,
# -Wunknown-pragmas for more speed
# gcc has a bug with recognizing diagnostics changes, so we'd have to turn on
# -Wno-unknown-pragmas

$(eval $(call do_flags,RELEASE,-O0 -ggdb,,\
	-Ofast -march=native -flto,$$(CFLAGS) -fwhole-program))

PARALLEL_METHOD ?= NONE
ifeq ($(PARALLEL_METHOD),NONE)
else ifeq ($(PARALLEL_METHOD),OPENMP)
OMP_SCHEDULE_TYPE ?= static
CFLAGS += -DSIM_METHOD=SIM_USE_OPENMP \
	-DSIM_OMP_SCHEDULE_TYPE=$(OMP_SCHEDULE_TYPE) \
	-fopenmp
LDFLAGS += -fopenmp
else ifeq ($(PARALLEL_METHOD),OPENCL)
CFLAGS += -DSIM_METHOD=SIM_USE_OPENCL
else
$(error invalid PARALLEL_METHOD specified!)
endif

CHUNK_SIZE ?= 50000
CHUNK_NUM ?= 50
NUM_THREADS ?= 1
CFLAGS += -DSIM_CHUNK_SIZE=$(CHUNK_SIZE) -DSIM_CHUNK_NUM=$(CHUNK_NUM) \
	-DSIM_NUM_THREADS=$(NUM_THREADS)

NO_OUTPUT ?= 0
ifneq ($(NO_OUTPUT), 0)
CFLAGS += -DNO_OUTPUT
endif

all: $(BIN)

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
