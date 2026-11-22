CXX      ?= g++
CXXFLAGS ?= -O3 -march=native -Wall -Wextra -std=c++17
TARGET   := $(notdir $(CURDIR))
all: $(TARGET)
$(TARGET): $(wildcard *.cpp)
	$(CXX) $(CXXFLAGS) -o $@ $<
clean:
	rm -f $(TARGET)
.PHONY: all clean
