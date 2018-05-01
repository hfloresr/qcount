CC	= gcc
CFLAGS	= -g -Wall -Wextra -std=c89

SRCS	= MAIN.c
OBJS	= MAIN.o
TARGET	= MAIN

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -r $(OBJS) $(TARGET)
