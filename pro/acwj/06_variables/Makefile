SRCS= cg.c decl.c expr.c  gen.c  misc.c  scan.c  stmt.c  tree.c main.c sym.c
comp1: $(SRCS)
	cc -o comp1 -g $(SRCS)

clean:
	rm -f comp1 *.o *.s out

test: comp1 input01 input02
	./comp1 input01
	cc -o out out.s
	./out
	./comp1 input02
	cc -o out out.s
	./out