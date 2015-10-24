# This make file was adopted to run in a MinGW environment.  Assumes your current working
# directory is the root of your project.

CXX       = g++-5.1.0
CXXFLAGS  = -g3 -O0 -ansi -std=c++14 -pedantic -Wall -Wold-style-cast -Woverloaded-virtual -Wextra -I. -DUSING_TOMS_SUGGESTIONS
SOURCES   = $(wildcard *.cpp) $(wildcard */*.cpp) $(wildcard */*/*.cpp) $(wildcard */*/*/*.cpp) $(wildcard */*/*/*/*.cpp)
args      = -include "to_string.hxx"

.PHONY: project_$(CXX).exe
project_$(CXX).exe: $(SOURCES)
	@echo $(SOURCES)
	@$(CXX) --version
	@$(CXX) $(CXXFLAGS) $(args) $(SOURCES) -o $@

# options to consider:
#       -Weffc++