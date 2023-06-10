
all: test perftest

test:
	gcc symtreetest.c symtree.c -o symtree

perftest:
	gcc symtreeperftest.c symtree.c -o symtreeperftest
