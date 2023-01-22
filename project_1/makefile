CC= gcc
CFLAGS= seed_reader.c -I.
RM= rm -vf   
SRCFILES= $(filter-out seed_reader.c, $(wildcard *.c))
PROGFILES= $(patsubst %.c, %, $(SRCFILES))


all: $(PROGFILES)

slugrace: slug.c slugrace.c seed_reader.c
	make slug
	$(CC) $(CFLAGS) slugrace.c -o slugrace

clean:
	$(RM) $(PROGFILES)  adler_duniho_project1.zip

zip:
	make clean
	zip adler_duniho_project1 *
