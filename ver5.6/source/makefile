TARGET = mterm mcmdr mupld mupld_c msave scope
ALL    = $(TARGET)

all: $(ALL)

CC     = gcc
INSTDIR = ../
LIB1   = -lreadline

mterm: mterm.c mterm_sub.c mterm.h
	$(CC) -O -o mterm mterm.c

msave: msave.c mterm_sub.c mterm.h
	$(CC) -O -o msave msave.c

scope: scope.c
	$(CC) -O -o scope scope.c

mcmdr: mcmdr.c mterm.h
	$(CC) -O -o mcmdr mcmdr.c $(LIB1)

mupld: mupld.c mterm.h
	$(CC) -O -o mupld mupld.c

mupld_c: mupld_c.c mterm.h
	$(CC) -O -o mupld_c mupld_c.c


clean:
	-rm -f *.o *.exe
	-rm -f $(ALL)

