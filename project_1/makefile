CC= gcc
CFLAGS= seed_reader.c -I.
RM= rm -vf   
SRCFILES= $(filter-out seed_reader.c, $(wildcard *.c))
PROGFILES= $(patsubst %.c, %, $(SRCFILES))


all: $(PROGFILES)

clean:
	$(RM) $(PROGFILES)  adler_duniho_project1.zip

zip:
	make clean
	zip adler_duniho_project1 *
