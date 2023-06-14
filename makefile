
all: test perftest

test:
	gcc symtreetest.c -o symtree

perftest:
	gcc symtreeperftest.c -o symtreeperftest
