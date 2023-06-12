main: main.o
	gcc -g -Wall main.o -o main -lpthread
	
main.o: main.c
	gcc -g -Wall -c main.c -o main.o

clean:
	rm -f *.o main

run: main
	./main