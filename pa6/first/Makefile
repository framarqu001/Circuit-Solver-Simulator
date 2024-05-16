all: first

first: first.c circuit.c
	gcc -Wall  -g -fsanitize=address,undefined first.c circuit.c  -o first

clean:
	rm -rf first
