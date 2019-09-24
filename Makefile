#TODO: Simplify build process, make a list of c files and compile off of that.
#TODO: Simplify linking and binary process by making a list of objects based on the list of c files
#TODO: Order?

ifndef VERBOSE
.SILENT:
endif

GTKINC=`pkg-config --cflags --libs gtk+-3.0`

CC=gcc
CFLAGS+= -rdynamic -export-dynamic 
CFLAGS+= -Wall 
CFLAGS+= $(GTKINC)

default: all

roasterSettings.o: roasterSettings.c
	echo Building Roaster Settings Module
	$(CC) $(CFLAGS) -c roasterSettings.c -o roasterSettings.o

roastEvent.o: roastEvent.c
	echo Building Roasting Event Module
	$(CC) $(CFLAGS) -c roastEvent.c -o roastEvent.o

app.o: app.c
	echo Building App
	$(CC) $(CFLAGS) -c app.c -o app.o
		
main.o: main.c
	echo Building Main
	$(CC) $(CFLAGS) -c main.c -o main.o

all: roasterSettings.o roastEvent.o app.o main.o
	echo Building Binary
	$(CC) $(CFLAGS) main.o roastEvent.o roasterSettings.o app.o -o roasterProgram

.PHONY: all

clean:
	-rm -f *.o
	-rm -f roasterProgram
