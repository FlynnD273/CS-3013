CC= gcc
CFLAGS= seed_reader.c -I. -pthread
RM= rm -vf   
SRCFILES= $(filter-out seed_reader.c, $(wildcard *.c))
PROGFILES= $(patsubst %.c, %, $(SRCFILES))
OFILES= $(patsubst %.c, %.o, $(SRCFILES))
DFILES= $(patsubst %.c, %.d, $(SRCFILES))


all: $(PROGFILES)

clean:
	$(RM) $(PROGFILES) $(OFILES) $(DFILES)  adler_duniho_project2.zip

zip:
	make clean
	zip adler_duniho_project2 *
