

TARGET = dmvs
ALL    = $(TARGET)

all: $(ALL)

CC     = gcc
INSTDIR = ./
#LIB1   = -lreadline

dmvs: dmvs.c dmvs_sub.c dmvs.h
	$(CC) -O -o dmvs dmvs.c


clean:
	-rm -f *.o
	-rm -f $(ALL)

install:
	-#if [-d $(INSTDIR) ]; \
	then install -v $(ALL) $(INSTDIR) ; \
	else echo "Sorry, $(INSTDIR) does not exist"; fi


