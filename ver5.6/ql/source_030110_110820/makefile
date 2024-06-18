# Makefile of hiroplot
# by Hiro Miyasaka
TARGET = hiroplot ph_hist
all: $(TARGET)


# Which compiler
CC  = gcc

# Where to install
#INSTDIR = ../../bin/
INSTDIR = ../bin/

# Lib options
LIB = -lreadline

# Options
# for Mac
#OSOPT = -DOSX -L/opt/local/lib
#OSOPT = -DOSX
# for Linux
#OSOPT = -DLINUX
# for Windows/Cygwin
OSOPT = -DCYGWIN

#TGOPT = -DHEFT
TGOPT = -DDB

#HIROP = hiroplot-heft
HIROP = hiroplot-db
#PHHIS = ph_hist-heft
PHHIS = ph_hist-db

#
hiroplot: hiroplot.o gptcall.o argset.o hiroplot_sub.o
	$(CC) -O -o $(HIROP) hiroplot.o gptcall.o argset.o hiroplot_sub.o $(OSOPT) $(LIB)

ph_hist: ph_hist.c defs.h define_struct.h ph_hist.h read_event_2.c temp.o kbhit.o
	$(CC) $(OSOPT) $(TGOPT) -o $(PHHIS) ph_hist.c temp.o kbhit.o

hiroplot.o: hiroplot_2.1.c argset.c hiroplot.h gptcall.h
	$(CC) -o hiroplot.o $(OSOPT) $(TGOPT) -c hiroplot_2.1.c

hiroplot_sub.o: hiroplot_sub.c hiroplot.h
	$(CC) $(OSOPT) $(TGOPT) -c hiroplot_sub.c

argset.o: argset.c
	$(CC) -c argset.c

temp.o: temp.c
	$(CC) -c  temp.c

kbhit.o: kbhit.c
	$(CC) -c  kbhit.c

gptcall.o: gptcall.c gptcall.h
	$(CC) -c gptcall.c

clean:
	-rm *.o $(HIROP) $(PHHIS)

install:
	@if [ -d $(INSTDIR) ];\
        then \
                cp $(HIROP) $(INSTDIR);\
                cp $(PHHIS) $(INSTDIR);\
                cp .hist.gnu $(INSTDIR);\
                cp .temp.gnu $(INSTDIR);\
                cp .wave.gnu $(INSTDIR);\
                cp .cnt.gnu  $(INSTDIR);\
                cp .map-db.gnu $(INSTDIR);\
                cp .map-heft.gnu $(INSTDIR);\
                cp .leak-db.gnu $(INSTDIR);\
                cp .leak-heft.gnu $(INSTDIR);\
                echo "Installed $(TARGET) $(GNUFILE) in $(INSTDIR)";\
        else \
                echo "Sorry, $(INSTDIR) does not exist";\
        fi

