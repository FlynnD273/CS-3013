CC= gcc
CFLAGS= -I.
RM= rm -vf   
SRCFILES= $(filter-out seed_reader.c, $(wildcard *.c))
PROGFILES= $(patsubst %.c, %, $(SRCFILES))
OFILES= $(patsubst %.c, %.o, $(SRCFILES))
DFILES= $(patsubst %.c, %.d, $(SRCFILES))


all: $(PROGFILES)

clean:
	$(RM) $(PROGFILES) $(OFILES) $(DFILES)  adler_duniho_project4.zip

zip:
	make clean
	zip adler_duniho_project4 *
