writer: writer.o
	gcc -g -Wall writer.o -o writer -lrt
	
writer.o: writer.c
	gcc -g -Wall -c writer.c -o writer.o

reader: reader.o
	gcc -g -Wall reader.o -o reader -lrt
	
reader.o: reader.c
	gcc -g -Wall -c reader.c -o reader.o

clean:
	rm -f *.o writer reader

run: writer reader
	./writer
	./reader
