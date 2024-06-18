

TARGET = mterm mcmdr mupld mupld_c msave scope
ALL    = $(TARGET)

all: $(ALL)

CC     = gcc
INSTDIR = ../
LIB1   = -lreadline

mterm: mterm.o mterm_sub.o
	$(CC) -O -o mterm mterm.o mterm_sub.o

msave: msave.o mterm_sub.o
	$(CC) -O -o msave msave.o mterm_sub.o

scope: scope.c
	$(CC) -O -o scope scope.c

mcmdr: mcmdr.o
	$(CC) -O -o mcmdr mcmdr.o $(LIB1)

mupld: mupld.o mterm_sub.o
	$(CC) -O -o mupld mupld.o mterm_sub.o

mupld_c: mupld_c.o mterm_sub.o
	$(CC) -O -o mupld_c mupld_c.o mterm_sub.o

mterm.o: mterm.c mterm.h
	$(CC) -c mterm.c

mterm_sub.o: mterm_sub.c mterm.h
	$(CC) -c mterm_sub.c

mcmdr.o: mcmdr.c
	$(CC) -c mcmdr.c

mupld.o: mupld.c 
	$(CC) -c mupld.c

mupld_c.o: mupld_c.c
	$(CC) -c mupld_c.c

clean:
	-rm -f *.o *.exe
	-rm -f $(ALL)

install:
	-if [ -d $(INSTDIR) ]; \
	then install -v *.exe $(INSTDIR) ; \
	else echo "Sorry, $(INSTDIR) does not exist"; fi


