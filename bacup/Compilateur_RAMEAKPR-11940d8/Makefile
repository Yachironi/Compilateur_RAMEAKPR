OBJ=tp.o tp_l.o tp_y.o print.o
CC=gcc
CFLAGS=-Wall -ansi -I./
LDFLAGS= -g -lfl
LDFLAGSMAC= -g -ll
tp : $(OBJ)
	$(CC) -o tp $(OBJ) $(LDFLAGS)

mac : $(OBJ)
	$(CC) -o tp $(OBJ) $(LDFLAGSMAC)

test_lex : tp_l.c test_lex.c tp_y.h
	$(CC) $(CFLAGS) -o test_lex tp_l.c test_lex.c $(LDFLAGS) 

test_lex_mac : tp_l.c test_lex.c tp_y.h
	$(CC) $(CFLAGS) -o test_lex tp_l.c test_lex.c $(LDFLAGSMAC) 

tp.c :
	echo ''

tp.o: tp.c tp_y.h tp.h
	$(CC) $(CFLAGS) -c tp.c

print.o: print.c tp_y.h tp.h
	$(CC) $(CFLAGS) -c print.c

tp_l.o: tp_l.c tp_y.h
	$(CC) $(CFLAGS) -c tp_l.c

tp_l.c : tp.l tp_y.h tp.h
	flex --yylineno -otp_l.c tp.l

tp_y.o : tp_y.c
	$(CC) $(CFLAGS) -c tp_y.c

tp_y.h tp_y.c : tp.y tp.h
	bison -v -b tp_y -o tp_y.c -d tp.y

.Phony: clean

clean:
	rm -f *~ tp.exe* ./tp *.o tp_l.* tp_y.* test_lex test_lex_mac
	rm -f test/*~ test/*.out test/*.res test/*/*~ test/*/*.out test/*/*.res
