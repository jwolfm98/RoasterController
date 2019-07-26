#TODO: Simplify build process, make a list of c files and compile off of that.
#TODO: Simplify linking and binary process by making a list of objects based on the list of c files
#TODO: Order?

ifndef VERBOSE
.SILENT:
endif

GTKINC=`pkg-config --cflags --libs gtk+-3.0`

CC=gcc
CFLAGS+= -rdynamic -export-dynamic
CFLAGS+= -Wall -lm
CFLAGS+= $(GTKINC)

FILES=roasterSettings.c \
      roastEvent.c      \
      app.c             \
      main.c

OBS=./objects

default: all

deviceHandler.o : deviceHandler.c
	echo Building Device Handler
	$(CC) $(CFLAGS) -c deviceHandler.c -o $(OBS)deviceHandler.o

chart.o : chart.c
	echo Building Chart Utility
	$(CC) $(CFLAGS) -c chart.c -o $(OBS)chart.o

roasterSettings.o: roasterSettings.c
	echo Building Roaster Settings Module
	$(CC) $(CFLAGS) -c roasterSettings.c -o $(OBS)roasterSettings.o

roastEvent.o: roastEvent.c
	echo Building Roasting Event Module
	$(CC) $(CFLAGS) -c roastEvent.c -o $(OBS)roastEvent.o

app.o: app.c
	echo Building App
	$(CC) $(CFLAGS) -c app.c -o $(OBS)app.o

main.o: main.c
	echo Building Main
	$(CC) $(CFLAGS) -c main.c -o $(OBS)main.o

all: deviceHandler.o chart.o roasterSettings.o roastEvent.o app.o main.o
	echo Building Binary
	$(CC) $(CFLAGS) $(OBS)main.o $(OBS)roastEvent.o $(OBS)roasterSettings.o $(OBS)app.o $(OBS)chart.o $(OBS)deviceHandler.o -o roasterProgram

.PHONY: all

clean:
	-rm -f $(OBS)*.o
	-rm -f roasterProgram
