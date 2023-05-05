all: A.o B.o
	gcc -g -Wall A.o -o A -lrt -lpthread
	gcc -g -Wall B.o -o B -lrt -lpthread
	
all.o: A.c B.c
	gcc -g -Wall -c A.c -o A.o
	gcc -g -Wall -c B.c -o B.o

clean:
	rm -f *.o A
	rm -f *.o B
run: all
	./A &
	./B
