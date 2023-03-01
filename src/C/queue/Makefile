CFLAGS = -g
EXECUTABLE = qmain

qmain: qmain.o queue.o
	gcc -o ${EXECUTABLE} ${CFLAGS} qmain.o queue.o

qmain.o: qmain.c queue.h
	gcc -c ${CFLAGS} qmain.c

queue.o: queue.c queue.h
	gcc -c ${CFLAGS} queue.c

clean:
	rm *.o