
/**
 * symtreetest.c
 * Author:       Adam "beckadamtheinventor" Beckingham
 * Description:  Fast string:string dictionary structure test file.
 * License:      GPL3
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "symtree.h"

const char *str_HelloWorld = "$Hello World!";
const char *str_HowAreYou = "$How are you?";
const char *str_IAmWell = "$I am well.";

const char *var_HelloWorld = "HelloWorld";
const char *var_HowAreYou = "HowAreYou";
const char *var_IAmWell = "IAmWell";
const char *var_NumStrings = "NumStrings";

uint8_t buffer[65536*4];

int main(int argc, char *argv[]) {
	int rv = 0;
	FILE *fd;
	size_t bufferlen, treesize;
	char *sym;
	symtree_t *tree = alloc_symtree();

	sym = new_sym(tree, var_HelloWorld, 0, str_HelloWorld);
	if (sym != NULL) {
		sym = new_sym(tree, var_HowAreYou, 0, str_HowAreYou);
		if (sym != NULL) {
			sym = new_sym(tree, var_IAmWell, 0, str_IAmWell);
			if (sym != NULL) {
				sym = new_sym(tree, var_NumStrings, 0, "#3");
				if (sym != NULL) {
				} else {
					rv = 4;
				}
			} else {
				rv = 3;
			}
		} else {
			rv = 2;
		}
	} else {
		rv = 1;
	}

	dump_symtree(tree, buffer, sizeof(buffer), &bufferlen);
	if ((fd = fopen("symtreedump1.txt", "wb"))) {
		fwrite(&buffer, bufferlen, 1, fd);
		fclose(fd);
	}

	if ((fd = fopen("symtreeresults.txt", "wb"))) {
		if (rv != 0) {
			fprintf(fd, "Failed to init symbol %d in symtree\n", rv);
		} else {
			if ((sym = find_sym(tree, var_HelloWorld, 0)) == NULL) {
				fprintf(fd, "Failed to locate symbol \"%s\" in symtree.\n", var_HelloWorld);
				rv = 5;
			} else {
				fprintf(fd, "Found symbol \"%s\" with value \"%s\"\n", var_HelloWorld, sym);
				if ((sym = find_sym(tree, var_HowAreYou, 0)) == NULL) {
					fprintf(fd, "Failed to locate symbol \"%s\" in symtree.\n", var_HowAreYou);
					rv = 6;
				} else {
					fprintf(fd, "Found symbol \"%s\" with value \"%s\"\n", var_HowAreYou, sym);
					if ((sym = find_sym(tree, var_IAmWell, 0)) == NULL) {
						fprintf(fd, "Failed to locate symbol \"%s\" in symtree.\n", var_IAmWell);
						rv = 7;
					} else {
						fprintf(fd, "Found symbol \"%s\" with value \"%s\"\n", var_IAmWell, sym);
						if ((sym = find_sym(tree, var_NumStrings, 0)) == NULL) {
							fprintf(fd, "Failed to locate symbol \"%s\" in symtree.\n", var_NumStrings);
							rv = 8;
						} else {
							fprintf(fd, "Found symbol \"%s\" with value \"%s\"\n", var_NumStrings, sym);
							treesize = symtree_size(tree, false);
							fprintf(fd, "Symtree size = %u bytes.\n", treesize);
							treesize = symtree_size(tree, true);
							fprintf(fd, "Symtree size (+values) = %u bytes.\n", treesize);
							if (del_sym(tree, var_HelloWorld, 0)) {
								fprintf(fd, "Deleted symbol \"%s\" successfuly.\n", var_HelloWorld);
							} else {
								fprintf(fd, "Failed to delete symbol \"%s\".\n", var_HelloWorld);
								rv = 9;
							}
						}
					}
				}
			}
		}
		
		treesize = symtree_size(tree, false);
		fprintf(fd, "Final symtree size = %u bytes.\n", treesize);
		treesize = symtree_size(tree, true);
		fprintf(fd, "Final symtree size (+values) = %u bytes.\n", treesize);
		fclose(fd);
	}

	dump_symtree(tree, buffer, sizeof(buffer), &bufferlen);
	if ((fd = fopen("symtreedump2.txt", "wb"))) {
		fwrite(&buffer, bufferlen, 1, fd);
		fclose(fd);
	}
	
	free_symtree(tree);
	return rv;
}



