all: landermake run

landermake: lander.c levels.c
	gcc -o lander lander.c levels.c -lncurses

run: lander
	./lander
