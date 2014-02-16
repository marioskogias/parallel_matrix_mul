CC=g++
CFLAGS=-O3 -fopenmp -Wall -g -std=c++0x
LIBS= -ltbb -lrt
EXECUTABLES= task_group task_object

all: $(EXECUTABLES)

task_group: task_group.o util.o
	$(CC) $^ -o task_group $(LIBS)

task_object: task_object.o util.o
	$(CC) $^  -o task_object $(LIBS)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< 

clean:
	rm  *.o

distclean:
	rm *.o res* $(EXECUTABLES)
	
