all: second

second: second.c circuit.c
	gcc -Wall  -g -fsanitize=address,undefined second.c circuit.c -o second
clean:
	rm -rf second
