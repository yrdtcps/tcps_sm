#CC = gcc
CC = g++
CXX = g++
CFLAGS = -I/usr/local/ssl/include -L/usr/local/ssl/lib -Wall -g -Wno-char-subscripts -m32
#LDFLAGS = -lcrypto -lssl
LDFLAGS = -lcrypto -Wl,-rpath=/lib/i386-linux-gnu

#wildcard c files expand
SRCS = $(wildcard src/*.c) $(wildcard src/*.cpp)
#remove path info
dir=$(notdir $(SRCS))
# .c to .o
OBJS = $(patsubst %.c, obj/%.o, $(patsubst %.cpp, obj/%.o,$(dir)) )

TARGET = main

all: $(TARGET)
	@echo $(SRCS)
	@echo $(dir)
	@echo $(OBJS)

main : obj/main.o $(filter-out obj/main.o, $(OBJS))
	$(CXX) $(CFLAGS) -o $@ $^ $(LDFLAGS)

obj/%.o : src/%.c
	$(CC) -c $(CFLAGS) $^ -o $@
obj/%.o : src/%.cpp
	$(CXX) -c $(CFLAGS) $^ -o $@

.PHONY : clean
clean :
	rm -f obj/*.o
	rm -f main



