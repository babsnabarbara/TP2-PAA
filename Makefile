# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -O2
LDFLAGS = -lstdc++fs

# Main executable sources
SRCS = main.cpp backtracking.cpp branchandbound.cpp dynamic.cpp
EXEC = main

# Generator 1 sources
GENERATOR_SRCS = generator.cpp
GENERATOR_EXEC = generator

# Generator 2 sources
GENERATOR2_SRCS = generator2.cpp
GENERATOR2_EXEC = gen2

.PHONY: all clean

all: $(EXEC) $(GENERATOR_EXEC) $(GENERATOR2_EXEC)

$(EXEC): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

$(GENERATOR_EXEC): $(GENERATOR_SRCS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

$(GENERATOR2_EXEC): $(GENERATOR2_SRCS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(EXEC) $(GENERATOR_EXEC) $(GENERATOR2_EXEC)
