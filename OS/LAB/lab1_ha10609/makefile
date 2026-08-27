# The compiler we want to use
CC = gcc

# Compiler flags (-Wall turns on warnings to help you catch bugs)
CFLAGS = -Wall

# The list of all the programs we want to build
TARGETS = task1 task2 task3 task4 myhead copy

# 'all' is the default target that runs when you just type 'make'
all: $(TARGETS)

# This is a magic rule that tells Make how to build ANY program from a .c file.
# $@ means "the target name" and $< means "the .c file"
%: %.c
	$(CC) $(CFLAGS) -o $@ $<

# A command to easily clean up your folder by deleting the compiled programs
clean:
	rm -f $(TARGETS)
