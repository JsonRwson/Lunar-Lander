all: landermake run

landermake: main.c game.c render.c input.c levels.h
	gcc -o lander main.c game.c render.c input.c levels.c -lncurses

run: lander
	./lander