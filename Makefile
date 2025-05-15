# Compilateurs et flags
CC      = gcc
CXX     = g++
CFLAGS  = -Wall -g
CXXFLAGS= -Wall -g
LDFLAGS = -lsfml-graphics -lsfml-window -lsfml-system

# Sources C et objets
C_FILES  = board.c list.c ai.c movegen.c evaluate.c zobrist.c tt.c
C_OBJS   = $(C_FILES:.c=.o)

# Sources C++ pour l'UI
GUI       = main_gui.cpp
GUI_EXEC  = gui
GUI_OBJ   = main_gui.o

# Programme console
MAIN      = miniChess
MAIN_OBJ  = miniChess.o

# Tout
all: $(MAIN) $(GUI_EXEC)

# MiniChess (console)
$(MAIN): $(MAIN_OBJ) $(C_OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# GUI
$(GUI_EXEC): $(GUI_OBJ) $(C_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# Règles générales
%.o: %.c
	$(CC) $(CFLAGS) -c $<

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<

# Nettoyage
clean:
	rm -f *.o $(MAIN) $(GUI_EXEC)
