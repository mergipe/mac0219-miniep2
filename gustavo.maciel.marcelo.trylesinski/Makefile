# Nome: Gustavo Mendes Maciel
# NUSP: 9298062
#
# Nome: Marcelo Baiano Pastorino Trylesinski
# NUSP: 9297996
#
# MAC0219 - MiniEP2
#

CC=gcc
CFLAGS=-std=c99 -pthread
N=3

frogs: frogs.c
	$(CC) -o frogs frogs.c $(CFLAGS)

clean:
	rm frogs

run: frogs
	./frogs $(N)
