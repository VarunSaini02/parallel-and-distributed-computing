CFLAGS = -g
EXECUTABLE = functester

functester: functester.o func.o
	gcc -o ${EXECUTABLE} ${CFLAGS} functester.o func.o

functester.o: func.h functester.c
	gcc -c ${CFLAGS} functester.c

func.o: func.c func.h
	gcc -c ${CFLAGS} func.c

clean:
	rm *.o