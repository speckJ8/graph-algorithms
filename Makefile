CUDA_PATH ?= /usr/local/cuda

HOST_COMPILER ?= g++
NVCC          := $(CUDA_PATH)/bin/nvcc -ccbin $(HOST_COMPILER)
NVPROF        := $(CUDA_PATH)/bin/nvprof
CC            := cc

NVCCFLAGS := -m64 --threads 0
CCFLAGS   :=
CFLAGS    := -lm -Wall -O3
LDFLAGS   :=

ifeq ($(dbg),1)
      CCFLAGS += -g -G -DDEBUG
      CFLAGS += -g -DDEBUG
endif

ALL_CCFLAGS = $(NVCCFLAGS) $(CCFLAGS)

SMS ?= 35 37 50 52 60 61 70 75 80 86

ifeq ($(GENCODE_FLAGS),)
$(foreach sm,$(SMS),$(eval GENCODE_FLAGS += -gencode arch=compute_$(sm),code=sm_$(sm)))
HIGHEST_SM := $(lastword $(sort $(SMS)))
ifneq ($(HIGHEST_SM),)
GENCODE_FLAGS += -gencode arch=compute_$(HIGHEST_SM),code=compute_$(HIGHEST_SM)
endif
endif

all: build

build: boruvka

boruvka: boruvka.cu
	$(EXEC) $(NVCC) $(ALL_CCFLAGS) $(GENCODE_FLAGS) -o $@ $<

boruvka-cpu: boruvka.c
	$(EXEC) $(CC) $(CFLAGS) -o $@ $<


run-boruvka: boruvka
	$(EXEC) ./boruvka

run-boruvka-cpu: boruvka-cpu
	$(EXEC) ./boruvka-cpu

clean:
	rm -f boruvka boruvka-cpu

clobber: clean
