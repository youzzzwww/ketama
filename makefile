CC = g++
KETAMA_PATH = $(shell pwd)
OUT_PATH = ${BUILD_PATH}/ketama

TARGET_CXXFLAGS = -g -Wall --std=c++11
TARGET_LDFLAGS = -L/usr/local/lib -ldl
TARGET_INCLUDE = -I/usr/local/include -I$(KETAMA_PATH) 
TARGET_NAME = ${OUT_PATH}/test_ketama

SRC := $(wildcard *.cc)
OBJ := $(SRC:.cc=.o)
OBJS := $(patsubst %,$(OUT_PATH)/%, ${OBJ})

test_ketama : ${OBJS}
	$(CC) -o $(TARGET_NAME) $^ $(TARGET_LDFLAGS)

$(OUT_PATH)/%.o : ${KETAMA_PATH}/%.cc
	mkdir -p ${OUT_PATH}
	$(CC) $(TARGET_CXXFLAGS) -c -o $@ $^ $(TARGET_INCLUDE)


.PHONY : clean

clean :
	rm -rf ${OUT_PATH}/*
