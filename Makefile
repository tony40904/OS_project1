CC      = gcc
CFLAGS  = -Wall -pthread -std=gnu99

OBJECT  = _sched.o

EXEC    = main.out

default : $(EXEC)

$(EXEC) : $(OBJECT) 
	$(CC) $(CFLAGS) -o $(EXEC) $(OBJECT)

_sched.o : _sched.c _sched.h list.h

	$(CC) $(CFLAGS) -c _sched.c -o _sched.o

clean :
	$(RM) $(EXEC) *.o *.out
try :
	sudo ./$(EXEC) < FIFO_test.txt
