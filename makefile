compile: shelly.o
	gcc shelly.o

run:
	./a.out

clean:
	-rm -rf ./a.out *.o

all:
	make clean; make compile; make run

shelly.o:
	gcc -c shelly.c
