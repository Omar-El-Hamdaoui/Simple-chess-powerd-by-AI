# --- Variables ---
CC      := gcc
CFLAGS  := -Wall -g
# Tes sources “métier”
SRC     := miniChess.c board.c list.c ai.c movegen.c evaluate.c zobrist.c tt.c chess.c
OBJS    := $(SRC:.c=.o)

# Le CGI d’entrée
CGI_EXEC := chess.cgi

# Tout
all: miniChess gui $(CGI_EXEC)

# console
miniChess: miniChess.o board.o list.o ai.o movegen.o evaluate.o zobrist.o tt.o
	$(CC) $(CFLAGS) -o $@ $^

# GUI (ton interface SFML)
gui: main_gui.o board.o list.o ai.o movegen.o evaluate.o zobrist.o tt.o
	g++ $(CFLAGS) -o $@ $^ -lsfml-graphics -lsfml-window -lsfml-system

# CGI
$(CGI_EXEC): chess.o cgic.o board.o list.o ai.o movegen.o evaluate.o zobrist.o tt.o
	$(CC) $(CFLAGS) -o $@ $^

# Règles génériques
%.o: %.c
	$(CC) $(CFLAGS) -c $<

%.o: %.cpp
	g++ $(CFLAGS) -c $<

clean:
	rm -f *.o miniChess gui $(CGI_EXEC)
