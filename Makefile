CROSSPREFIX=m68k-atari-mintelf-

CC=$(CROSSPREFIX)gcc
AS=$(CC)
WARN=-Wall -W -Wstrict-prototypes -Wmissing-prototypes -Wdeclaration-after-statement -Werror
CFLAGS= -O2 -fomit-frame-pointer $(WARN) -I.
LDFLAGS=-s -Wl,-stack,16k
LIBS= -lgem

OBJS = \
	about.o \
	util.o \
	window.o \
	main.o \
	scsi.o \
	dd.o \
	$(empty)

all: wifi_da.acc

$(OBJS): util.h wi-fi.h wifi_da.h

wifi_da.acc: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)
	cp wifi_da.acc wifi_da.prg

clean distclean::
	$(RM) *.o *.acc *.prg *.pdb *.app *.tos *.ttp *.gtp purec/*.o socklib/*.o

