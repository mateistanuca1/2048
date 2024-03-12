build:
	gcc 2048.c -o 2048 -lncurses -Wall
	touch saved_game.bin
run:
	./2048
clean:
	rm 2048
	rm saved_game.bin