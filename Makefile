## Makefile supports following:
# make all # or link   - to compile and link
# make build           - to build only
# make clean           - to delete all output files
# make cleanall        - to delete output files as well as TAGS and cscope files
# make TAGS            - to create emacs TAGS
# make cscope          - to create cscope files
# make dependencies    - to install dependencies (debian system)

# This is the main compiler
CC = gcc

# Source file directory
SRCDIR = src

# Include directories
# Don't remove -I
INC = -I inc

# Build directory - to store object files
BUILDDIR = build

# Output directory
OUTPUT = bin

# Target or Executible file
TARGET := $(OUTPUT)/shell

# Source file extension
SRCEXT = c

# Compiler flags
CFLAGS = -g -Wall

# Include libraries
LIB = -lreadline

# Creating list of c files
SOURCES := $(shell find $(SRCDIR) -type f -name "*.$(SRCEXT)")

# Creating list of object files to be built
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))

# Default task
all: build link

# Install dependencies
dependencies:
	@echo "# Installing dependencies:"
	@sudo apt-get install libreadline-dev
	@echo "# Install complete.\n"

# Building (Compiling)
build: $(OBJECTS)

# Linking
link: $(TARGET)

# Cleaning binary and object files
clean:
	@echo "# Cleaning"
	$(RM) -r -v $(BUILDDIR) $(OUTPUT)

cleanall: clean
	$(RM) -v cscope.* TAGS

$(TARGET): $(OBJECTS)
	@echo "# Linking"
	@mkdir -p $(OUTPUT)
	$(CC) $^ -o $(TARGET) $(LIB)
	@echo "# Executable Binary: $(TARGET)"
	@echo "# To run just invoke: ./$(TARGET)"

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@if [ ! -f $@ ]; then mkdir -p $@; rm -r $@; fi
	$(CC) $(CFLAGS) $(INC) -c -o $@ $<

.PHONY: clean TAGS install cscope

help:
	@echo 'make <all, clean, build, link, dependencies, TAGS, cscope>'

TAGS:
	find . -name "*.[chS]" | xargs etags -a

cscope:
	find . -name "*.[chS]" > cscope.files
	cscope -b -q -k
