.PHONY: all
all: PA1 myprintftest

PA1: t.c myprintf.c ts.s myprintf.h 
	gcc -m32 -o $@ t.c myprintf.c ts.s

myprintftest: myprintftest.c myprintf.c myprintf.h
	gcc -m32 -o $@ t.c myprintftest.c myprintf.c