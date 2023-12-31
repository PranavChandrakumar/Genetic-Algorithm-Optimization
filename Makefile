CC = gcc 
CFLAGS = -Wall -Wextra -O3 #-O3 flag automatically slightly optimizes the code while compiling
LDFLAGS = -lm #Need an extra set of flags for the OF.c and GA.c files (instead of just including -lm in CFLAGS)
SOURCES = functions.h OF.c functions.c GA.c
OBJECTS = $(SOURCES:.c =.o)
EXECUTABLE = GA

all : $(EXECUTABLE)

$(EXECUTABLE):$(OBJECTS)
	$(CC) $(OBJECTS) -o $(EXECUTABLE) $(LDFLAGS) 

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
clean :
	rm -f $(EXECUTABLE)
