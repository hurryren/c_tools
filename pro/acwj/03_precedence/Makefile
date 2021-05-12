parser: expr.c interp.c  main.c scan.c tree.c
	cc -o parser -g expr.c interp.c tree.c main.c scan.c


parser2: expr2.c interp.c  main.c scan.c tree.c
	cc -o parser2 -g expr2.c interp.c tree.c main.c scan.c

test:parser
	-(./parser input01;\
		./parser input02;\
		./parser input03;\
		./parser input04;\
		./parser input05)
test2:parser2
	-(./parser2 input01;\
		./parser2 input02;\
		./parser2 input03;\
		./parser2 input04;\
		./parser2 input05)


clean:
	rm -f parser2  parser *.o