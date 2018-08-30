.PHONY: all
all: PA1 myprintftest

PA1: t.c myprintf.c ts.s 
	gcc -std=c99 -m32 -o $@ t.c myprintf.c ts.s

myprintftest: myprintftest.c myprintf.c
	gcc -std=c99 -m32 -o $@  myprintftest.c myprintf.c
