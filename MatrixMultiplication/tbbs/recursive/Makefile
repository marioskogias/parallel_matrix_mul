include ../Makefile.include

INCLUDE_DIR = $(TBB_DIR)/include/
LIBRARY_DIR = $(TBB_DIR)/build/$(LIBRARY_VER)

CXX=g++
CXXFLAGS= -DCLS=64 -O3 -Wall -std=c++0x  -Wextra
LIBS= -ltbb -lrt
EXECUTABLES= task_group task_object

CXXFLAGS += -I$(INCLUDE_DIR)

all: $(EXECUTABLES)

task_group: task_group.o util.o
	$(CXX) $^ -o task_group -L$(LIBRARY_DIR) $(LIBS)

task_object: task_object.o util.o
	$(CXX) $^  -o task_object -L$(LIBRARY_DIR) $(LIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< 

clean:
	rm  *.o

distclean:
	rm *.o res* $(EXECUTABLES)
	
