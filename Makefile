make:
	gcc -Wall -pedantic -ansi -D_XOPEN_SOURCE=600 passageiro.c -o passageiro
clean:
	rm passageiro
