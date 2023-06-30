CFLAGS=-std=c17 -Wall -Wextra
LIBS=.\SDL2\lib -lmingw32 -lSDL2main -lSDL2
INCLUDES=.\SDL2\include\SDL2

all:
	gcc main.c -o main $(CFLAGS) -L$(LIBS) -I$(INCLUDES)