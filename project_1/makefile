make generation: generation.c seed_reader.c
	gcc -g -o generation generation.c seed_reader.c

make clean:
	rm -f prolific generation explorer slug slugrace adler_duniho_project1.zip

make zip:
	make clean
	zip adler_duniho_project1 *
