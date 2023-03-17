CFLAGS = -g
EXECUTABLE = barriertest

barriertest: barriertest.o barrier.o cqueue.o spinlock.o
	gcc -o ${EXECUTABLE} ${CFLAGS} -pthread barriertest.o barrier.o cqueue.o spinlock.o

barriertest.o: barriertest.c 
	gcc -c ${CFLAGS} barriertest.c

barrier.o: barrier.c barrier.h cqueue.h spinlock.h rc.h
	gcc -c ${CFLAGS} barrier.c

cqueue.o: cqueue.c cqueue.h spinlock.h rc.h
	gcc -c ${CFLAGS} cqueue.c

spinlock.o: spinlock.c spinlock.h rc.h
	gcc -c ${CFLAGS} spinlock.c 

clean:
	rm -f *.o barriertest
	rm -f core*