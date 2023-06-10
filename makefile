
all: bosmalloc symboltable symtree

bosmalloc:
	gcc bosmalloctest.c -o bosmalloc.exe

hashedsymlist:
	gcc hashedsymlisttest.c hashedsymlist.c -o hashedsymlist.exe

symboltable:
	gcc symboltabletest.c symboltable.c -o symboltable.exe

symtree:
	gcc symtreetest.c symtree.c -o symtree.exe

