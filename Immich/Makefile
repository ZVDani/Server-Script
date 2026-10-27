CXX ?= g++
CXXFLAGS ?= -std=c++11 -Wall -Wextra -O2
TARGET := immich-updater
SOURCE := Source.cpp

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SOURCE)
	$(CXX) $(CXXFLAGS) -o $@ $<

clean:
	rm -f $(TARGET)
