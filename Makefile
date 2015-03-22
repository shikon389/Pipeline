CCFLAGS = -ggdb -Wall

all: shell main

shell: shell.c piper.c shell.h
	gcc $(CCFLAGS) shell.c piper.c -o shell

# this will reformat your code according to the linux guidelines.
# be careful when using this command!
pretty: shell.c shell.h
	indent -linux shell.c shell.h

clean:
	rm -rf shell
