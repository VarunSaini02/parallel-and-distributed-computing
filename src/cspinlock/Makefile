CFLAGS = -g -pthread
EXECUTABLE = spintest

${EXECUTABLE}: spintest.o spinlock.o
	gcc -o ${EXECUTABLE} ${CFLAGS} spintest.o spinlock.o

spintest.o: spintest.c spinlock.h rc.h
	gcc -c ${CFLAGS} spintest.c 

spinlock.o: spinlock.c spinlock.h rc.h
	gcc -c ${CFLAGS} spinlock.c

clean:
	rm *.o ${EXECUTABLE}