include ../Makefile.include

INCLUDE_DIR = $(TBB_DIR)/include/
LIBRARY_DIR = $(TBB_DIR)/build/$(LIBRARY_VER)

# CXX = g++
# To compile in clones:
CXX = /various/common_tools/in_path/g++-4.7.2
CXXFLAGS = -O3 -Wall -std=c++0x -DUSE_LAMBDAS 
LDFLAGS =
LIBS = -ltbb -lrt 

CXXFLAGS += -I$(INCLUDE_DIR)
 
all : parallel_for 

parallel_for : parallel_for.o
	$(CXX) $(LDFLAGS) parallel_for.o -o parallel_for -L$(LIBRARY_DIR) $(LIBS)

%.o : %.cpp
	$(CXX) $(CXXFLAGS) -c $<

clean :
	rm -f parallel_for parallel_for.o
