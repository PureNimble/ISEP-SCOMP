all: insert.o consult.o consult_all.o
	gcc -g -Wall insert.o -o insert -lrt -lpthread
	gcc -g -Wall consult.o -o consult -lrt -lpthread
	gcc -g -Wall consult_all.o -o consult_all -lrt -lpthread
	
all.o: insert.c consult.c consult_all.c
	gcc -g -Wall -c insert.c -o insert.o
	gcc -g -Wall -c consult.c -o consult.o
	gcc -g -Wall -c consult_all.c -o consult_all.o

clean:
	rm -f *.o insert
	rm -f *.o consult
	rm -f *.o consult_all
run: all
	./insert
