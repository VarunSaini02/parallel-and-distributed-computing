CFLAGS = -g
EXECUTABLE = pooltest

pooltest: pooltest.o pool.o cqueue.o spinlock.o
	gcc -o ${EXECUTABLE} ${CFLAGS} -pthread pooltest.o pool.o cqueue.o spinlock.o

pooltest.o: pooltest.c 
	gcc -c ${CFLAGS} pooltest.c

pool.o: pool.c pool.h cqueue.h spinlock.h rc.h
	gcc -c ${CFLAGS} pool.c

cqueue.o: cqueue.c cqueue.h spinlock.h rc.h
	gcc -c ${CFLAGS} cqueue.c

spinlock.o: spinlock.c spinlock.h rc.h
	gcc -c ${CFLAGS} spinlock.c 

clean:
	rm -f *.o qmain
	rm -f core*