CUDA_PATH ?= /usr/local/cuda

HOST_COMPILER ?= g++
NVCC          := $(CUDA_PATH)/bin/nvcc -ccbin $(HOST_COMPILER)
NVPROF        := $(CUDA_PATH)/bin/nvprof
CC            := cc

NVCCFLAGS := -m64 --threads 0
CCFLAGS   :=
CFLAGS    := -Wall -O3
LDLIBS      := -lm

BUILD_TYPE = debug

ifeq ($(BUILD_TYPE),debug)
      CCFLAGS += -g -G -DDEBUG
      CFLAGS += -g -DDEBUG
endif

CUDACCFLAGS = $(NVCCFLAGS) $(CCFLAGS)

SMS ?= 35 37 50 52 60 61 70 75 80 86

ifeq ($(GENCODE_FLAGS),)
$(foreach sm,$(SMS),$(eval GENCODE_FLAGS += -gencode arch=compute_$(sm),code=sm_$(sm)))
HIGHEST_SM := $(lastword $(sort $(SMS)))
ifneq ($(HIGHEST_SM),)
GENCODE_FLAGS += -gencode arch=compute_$(HIGHEST_SM),code=compute_$(HIGHEST_SM)
endif
endif

OBJS = boruvka.o gabow.o
BUILD_DIR = build
GEN_OBJS = $(patsubst %.o, $(BUILD_DIR)/%.o, $(OBJS))

.PHONY: clean all install
.SUFFIXES:

all: tester

%.o: %.c
	$(CC) -c $< -o $(BUILD_DIR)/$@ -std=gnu11 $(CFLAGS)
%: %.c
	$(CC) -c $< -o $(BUILD_DIR)/$@ -std=gnu11 $(CFLAGS)

tester: tester.c $(OBJS)
	$(CC) $< -o $(BUILD_DIR)/$@ -std=gnu11 $(CFLAGS) $(LDLIBS) $(GEN_OBJS)

gpu-boruvka: boruvka.cu
	$(EXEC) $< -o $(BUILD_DIR)/$@ $(NVCC) $(CUDAFLAGS) $(GENCODE_FLAGS)

clean:
	rm -f *.o $(GEN_OBJS)
	rm -f $(BUILD_DIR)/tester
	rm -f $(BUILD_DIR)/gpu-boruvka
