CFLAGS = -g -pthread
EXECUTABLE = cqueuetest

${EXECUTABLE}: cqueuetest.o cqueue.o spinlock.o
	gcc -o ${EXECUTABLE} ${CFLAGS} cqueuetest.o cqueue.o spinlock.o

cqueuetest.o: cqueuetest.c cqueue.h rc.h
	gcc -c ${CFLAGS} cqueuetest.c 

cqueue.o: cqueue.c cqueue.h spinlock.h rc.h
	gcc -c ${CFLAGS} cqueue.c

spinlock.o: spinlock.c spinlock.h rc.h
	gcc -c ${CFLAGS} spinlock.c

clean:
	rm *.o ${EXECUTABLE}