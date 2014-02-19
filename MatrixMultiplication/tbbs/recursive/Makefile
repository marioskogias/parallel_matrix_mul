include ../Makefile.include

INCLUDE_DIR = $(TBB_DIR)/include/
LIBRARY_DIR = $(TBB_DIR)/build/$(LIBRARY_VER)

CC=g++
CXXFLAGS=-O3 -Wall -std=c++0x
LIBS= -ltbb -lrt
EXECUTABLES= task_group task_object

CXXFLAGS += -I$(INCLUDE_DIR)

all: $(EXECUTABLES)

task_group: task_group.o util.o
	$(CC) $^ -o task_group -L$(LIBRARY_DIR) $(LIBS)

task_object: task_object.o util.o
	$(CC) $^  -o task_object -L$(LIBRARY_DIR) $(LIBS)

%.o: %.cpp
	$(CC) $(CXXFLAGS) -c $< 

clean:
	rm  *.o

distclean:
	rm *.o res* $(EXECUTABLES)
	
