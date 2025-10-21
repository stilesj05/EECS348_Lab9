# Lab 9 - Simple Makefile
CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra

all: lab9

lab9: main.cpp
	$(CXX) $(CXXFLAGS) -o lab9 main.cpp

clean:
	rm -f lab9
