main: main.o
	gcc -g -Wall main.o -o main
	
main.o: main.c
	gcc -g -Wall -c main.c -o main.o

simulator: simulator.o
	gcc -g -Wall simulator.o -o simulator

simulator.o: simulator.c
	gcc -g -Wall -c simulator.c -o simulator.o

clean:
	rm -f *.o main simulator

run: main simulator
	./main &
	./simulator