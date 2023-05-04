all: seller.o client.o
	gcc -g -Wall seller.o -o seller -lrt -lpthread
	gcc -g -Wall client.o -o client -lrt -lpthread
	
all.o: seller.c client.c
	gcc -g -Wall -c seller.c -o seller.o
	gcc -g -Wall -c client.c -o client.o

clean:
	rm -f *.o seller
	rm -f *.o client
run: all
	./seller &
	./client
