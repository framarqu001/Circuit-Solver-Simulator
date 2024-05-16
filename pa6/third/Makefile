all: third

third: third.c circuit.c
	gcc -Wall  -g -fsanitize=address,undefined third.c circuit.c -o third
clean:
	rm -rf third
