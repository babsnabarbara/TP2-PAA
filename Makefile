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

# Statistical analysis sources
STAT_SRCS = statistical_analysis.cpp
STAT_EXEC = statistical_analysis

# Advanced statistical analysis sources
ADV_STAT_SRCS = advanced_statistical_analysis.cpp
ADV_STAT_EXEC = advanced_statistical_analysis

.PHONY: all clean

all: $(EXEC) $(GENERATOR_EXEC) $(GENERATOR2_EXEC) $(STAT_EXEC) $(ADV_STAT_EXEC)

$(EXEC): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

$(GENERATOR_EXEC): $(GENERATOR_SRCS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

$(GENERATOR2_EXEC): $(GENERATOR2_SRCS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

$(STAT_EXEC): $(STAT_SRCS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

$(ADV_STAT_EXEC): $(ADV_STAT_SRCS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(EXEC) $(GENERATOR_EXEC) $(GENERATOR2_EXEC) $(STAT_EXEC) $(ADV_STAT_EXEC)
