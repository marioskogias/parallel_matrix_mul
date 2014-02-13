CC=g++
CFLAGS=-O3 -fopenmp -Wall -g
LIBS= -ltbb -lrt
OBJFILES=util.o mm_recursive.o

all: task_group

task_group: mm_recursive.o util.o
	$(CC) $(CFLAGS) $(LIBS) -o task_group $(OBJFILES) 

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< 

clean:
	rm  *.o

distclean:
	rm *.o res* task_group
	
