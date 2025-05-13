# Compilation générale
CC=gcc
CXX=g++
CFLAGS=-Wall -g
CXXFLAGS=-Wall -g
LDFLAGS=-lsfml-graphics -lsfml-window -lsfml-system

# Fichiers sources C
C_FILES=board.c list.c ai.c movegen.c evaluate.c
C_OBJS=$(C_FILES:.c=.o)

# Interface graphique en C++
GUI=main_gui.cpp
GUI_EXEC=gui
GUI_OBJ=main_gui.o

# Programme console
MAIN=miniChess
MAIN_OBJ=miniChess.o

# Règles de compilation
all: $(MAIN) $(GUI_EXEC)

$(MAIN): $(MAIN_OBJ) $(C_OBJS)
	$(CC) -o $@ $^

$(GUI_EXEC): $(GUI_OBJ) $(C_OBJS)
	$(CXX) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -f *.o $(MAIN) $(GUI_EXEC)
