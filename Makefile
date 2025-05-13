all: miniChess

list.o: list.c list.h item.h board.h
	gcc -c list.c

board.o: board.c board.h list.h item.h
	gcc -c board.c

ai.o: ai.c ai.h movegen.h evaluate.h board.h list.h item.h
	gcc -c ai.c

movegen.o: movegen.c movegen.h board.h list.h item.h
	gcc -c movegen.c

evaluate.o: evaluate.c evaluate.h board.h
	gcc -c evaluate.c

miniChess.o: miniChess.c board.h list.h item.h ai.h movegen.h evaluate.h
	gcc -c miniChess.c

miniChess: miniChess.o board.o list.o ai.o movegen.o evaluate.o
	gcc -o miniChess miniChess.o board.o list.o ai.o movegen.o evaluate.o
