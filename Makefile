CC      = gcc
CFLAGS  = -Wall -pthread -std=gnu99

OBJECT  = my_sched.o

EXEC    = main.out

default : $(EXEC)

$(EXEC) : $(OBJECT) 
	$(CC) $(CFLAGS) -o $(EXEC) $(OBJECT)

my_sched.o : my_sched.c my_sched.h list.h

	$(CC) $(CFLAGS) -c my_sched.c -o my_sched.o

clean :
	$(RM) $(EXEC) *.o *.out
