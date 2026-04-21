CC = gcc
CFLAGS = -Wall -Wextra -g -I.
LDFLAGS = -lssl -lcrypto

# Source files
SRCS = pes.c object.c tree.c index.c commit.c
OBJS = $(SRCS:.c=.o)

# Targets
TARGET = pes
TEST_OBJECTS = test_objects
TEST_TREE = test_tree

.PHONY: all clean test

all: $(TARGET) $(TEST_OBJECTS) $(TEST_TREE)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(TEST_OBJECTS): test_objects.o object.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(TEST_TREE): test_tree.o tree.o index.o object.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(TEST_OBJECTS) $(TEST_TREE) *.o
	rm -rf .pes

test: all
	./test_objects
	./test_tree
