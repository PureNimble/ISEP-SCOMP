all: writer.o reader.o
	gcc -g -Wall writer.o -o writer -lrt -lpthread
	gcc -g -Wall reader.o -o reader -lrt -lpthread
	
all.o: writer.c reader.c
	gcc -g -Wall -c writer.c -o writer.o
	gcc -g -Wall -c reader.c -o reader.o

clean:
	rm -f *.o writer
	rm -f *.o reader
run: all
	./writer &
	./reader
