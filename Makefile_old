CCFLAGS = -ggdb -Wall

all: compile

compile: main.c main.h
	gcc $(CCFLAGS) main.c -o shell

# this will reformat your code according to the linux guidelines.
# be careful when using this command!
pretty: main.c main.h
	indent -linux main.c main.h

clean:
	rm -rf shell

