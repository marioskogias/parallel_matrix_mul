CC=g++
CFLAGS=-O3 -fopenmp -Wall -g -std=c++0x
LIBS= -ltbb -lrt
OBJFILES=util.o task_group.o

all: task_group

task_group: task_group.o util.o
	$(CC)  $(OBJFILES) -o task_group $(LIBS)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< 

clean:
	rm  *.o

distclean:
	rm *.o res* task_group
	
