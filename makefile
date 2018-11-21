compile:
	gcc shelly.c

run:
	./a.out

clean:
	-rm -rf ./a.out

all:
	make clean; make compile; make run
