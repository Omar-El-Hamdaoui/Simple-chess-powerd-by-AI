all: miniChess

list.o: list.c list.h
	gcc -c list.c

board.o: board.c board.h
	gcc -c board.c

miniChess.o: miniChess.c board.h list.h item.h
	gcc -c miniChess.c

miniChess: miniChess.o board.o list.o
	gcc -o miniChess miniChess.o board.o list.o
