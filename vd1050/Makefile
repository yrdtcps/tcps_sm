#CC = gcc
CC := arm-fsl-linux-gnueabi-g++
#CXX = g++
CXX := arm-fsl-linux-gnueabi-g++
#CFLAGS = -I/usr/local/ssl/include -L/usr/local/ssl/lib -Wall -g
#CFLAGS = -I/usr/local/ssl/include -L/usr/local/ssl/lib -O2 -Werror  -Wall -Wno-char-subscripts
CFLAGS = -I/usr/local/ssl/include -L./lib -O2 -Wall -Wno-char-subscripts
#LDFLAGS = -lcrypto -lssl
LDFLAGS = -lcrypto

ROOT = ../

#wildcard c files expand
SRCS = $(wildcard $(ROOT)src/*.c) $(wildcard $(ROOT)src/*.cpp)
#remove path info
dir=$(notdir $(SRCS))
# .c to .o
OBJS = $(patsubst %.c, obj/%.o, $(patsubst %.cpp, obj/%.o,$(dir)) )

TARGET = main

all: $(TARGET)
	@echo $(SRCS)
	@echo $(dir)
	@echo $(OBJS)
	set -x

main : obj/main.o $(filter-out obj/main.o, $(OBJS))
	$(CXX) $(CFLAGS) -o $@ $^ $(LDFLAGS)

obj/%.o : $(ROOT)src/%.c
	$(CC) -c $(CFLAGS) $^ -o $@
obj/%.o : $(ROOT)src/%.cpp
	$(CXX) -c $(CFLAGS) $^ -o $@

.PHONY : clean
clean :
	rm -f obj/*.o
	rm -f main



