CC = g++ -std=c++11
DEBUG = -g
CFLAGS = -Wall -c $(DEBUG)
LFLAGS = -Wall $(DEBUG)

main : main.o Database.o Table.o
	$(CC) $(LFLAGS) main.o -o main

main.o : main.cpp Database.cpp Table.cpp sim.cpp
	$(CC) $(CFLAGS) main.cpp

Database.o: Database.cpp Database.h
	$(CC) $(CFLAGS) Database.cpp

Table.o: Table.cpp Table.h
	$(CC) $(CFLAGS) Table.cpp

clean: 
	\rm *.o main
