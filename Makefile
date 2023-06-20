all: main0

main0: main0.o ObjectManager.o
	gcc -Wall -g -o main0 main0.o ObjectManager.o

main0.o: main0.c
	gcc -Wall -g -c main0.c

ObjectManager.o: ObjectManager.c
	gcc -Wall -g -c ObjectManager.c

clean:
	rm -f main0 *.o

