main: main.o
	gcc -g -Wall main.o -o main
	
main.o: main.c
	gcc -g -Wall -c main.c -o main.o

signal: signal.o
	gcc -g -Wall signal.o -o signal

signal.o: signal.c
	gcc -g -Wall -c signal.c -o signal.o

clean:
	rm -f *.o main signal

run: main signal
	./main &
	./signal