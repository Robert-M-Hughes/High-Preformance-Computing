# The variable CC specifies which compiler will be used.
# (because different unix systems may use different compilers)
#If using math libraries and later MPI, use g++
CC=gcc

# The variable CFLAGS specifies compiler options
#   -c :    Only compile (don't link)
#   -Wall:  Enable all warnings about lazy / dangerous C programming 
#  You can add additional options on this same line..
#  WARNING: NEVER REMOVE THE -c FLAG, it is essential to proper operation
CFLAGS=-c -Wall

# All of the .h header files to use as dependencies
HEADERS=

# All of the object files to produce as intermediary work. If you have source files like
#main.c; factorial.c, then objects are main.o and factorial.o
OBJECTS= 

# The final program to build
EXECUTABLE=

# --------------------------------------------

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(EXECUTABLE)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -rf *.o $(EXECUTABLE)

