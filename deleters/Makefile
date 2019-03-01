

CC = g++
WARNINGS=-Wall -Wextra -Werror -Wcast-align -Wcast-qual -Wconversion -Wshadow -Wsign-conversion -Wstrict-overflow=3
CXXFLAGS = -Os
override CXXFLAGS += -std=c++14 $(WARNINGS)
BDIR=./build

BUILDLIST = attempt1 attempt2 attempt3 correct
TARGETS = $(patsubst %, $(BDIR)/%, $(BUILDLIST))

.PHONY: all
all: $(TARGETS)
	@echo "Successfuly built into $(BDIR)"

$(BDIR)/%: %.cxx
	$(CC) -o $@ $^ $(CXXFLAGS)

.PHONY: all clean
clean:
	rm -f $(BDIR)/*
