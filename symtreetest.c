
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

char buffer[256];

int main(int argc, char *argv[]) {
	int rv = 0;
	FILE *fd, *fd2;
	size_t bufferlen, treesize;
	char *sym;
	char *allocatedbuffer;
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

	if (dump_symtree(tree, &buffer, sizeof(buffer), &bufferlen)) {
		if ((fd = fopen("symtreedump1.json", "w"))) {
			fwrite(&buffer, bufferlen, 1, fd);
			fclose(fd);
		}
	} else if (bufferlen > 0) {
		printf("Failed to dump symtree due to the buffer not being large enough!\n");
	} else {
		printf("Failed to dump symtree!\n");
	}

	if ((fd = fopen("symtreeresults.txt", "w"))) {
		if (rv != 0) {
			fprintf(fd, "Failed to init symbol %d in symtree.\n", rv);
		} else {
			if ((sym = find_sym(tree, var_HelloWorld, 0)) == NULL) {
				fprintf(fd, "Failed to locate symbol \"%s\" in symtree.\n", var_HelloWorld);
				rv = 5;
			} else {
				fprintf(fd, "Found symbol \"%s\" with value \"%s\".\n", var_HelloWorld, sym);
				if ((sym = find_sym(tree, var_HowAreYou, 0)) == NULL) {
					fprintf(fd, "Failed to locate symbol \"%s\" in symtree.\n", var_HowAreYou);
					rv = 6;
				} else {
					fprintf(fd, "Found symbol \"%s\" with value \"%s\".\n", var_HowAreYou, sym);
					if ((sym = find_sym(tree, var_IAmWell, 0)) == NULL) {
						fprintf(fd, "Failed to locate symbol \"%s\" in symtree.\n", var_IAmWell);
						rv = 7;
					} else {
						fprintf(fd, "Found symbol \"%s\" with value \"%s\".\n", var_IAmWell, sym);
						if ((sym = find_sym(tree, var_NumStrings, 0)) == NULL) {
							fprintf(fd, "Failed to locate symbol \"%s\" in symtree.\n", var_NumStrings);
							rv = 8;
						} else {
							fprintf(fd, "Found symbol \"%s\" with value \"%s\".\n", var_NumStrings, sym);
							if ((set_sym(tree, var_NumStrings, 0, "#4")) == NULL) {
								fprintf(fd, "Failed to set symbol \"%s\" in symtree to \"%s\".\n", var_NumStrings, "#4");
								rv = 9;
							} else {
								fprintf(fd, "Set symbol \"%s\" in symtree to \"%s\" successfuly.\n", var_NumStrings, "#4");
								treesize = symtree_size(tree, false);
								fprintf(fd, "Symtree size = %u bytes.\n", treesize);
								treesize = symtree_size(tree, true);
								fprintf(fd, "Symtree size (+values) = %u bytes.\n", treesize);
								if (del_sym(tree, var_HelloWorld, 0, false)) {
									fprintf(fd, "Deleted symbol \"%s\" successfuly.\n", var_HelloWorld);
								} else {
									fprintf(fd, "Failed to delete symbol \"%s\".\n", var_HelloWorld);
									rv = 10;
								}
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

		if (dump_symtree(tree, &buffer, sizeof(buffer), &bufferlen)) {
			if ((fd2 = fopen("symtreedump2.json", "w"))) {
				fwrite(&buffer, bufferlen, 1, fd2);
				fclose(fd2);
			}
		} else if (bufferlen > 0) {
			printf("Failed to dump symtree due to the buffer not being large enough!\n");
		} else {
			printf("Failed to dump symtree!\n");
		}

		free_symtree(tree);

		if ((fd2 = fopen("symtreedump1.json", "r"))) {
			size_t len;
			fseek(fd2, 0, 2);
			len = ftell(fd2);
			fseek(fd2, 0, 0);
			if ((allocatedbuffer = malloc(len))) {
				fread(allocatedbuffer, len, 1, fd2);
			}
			fclose(fd2);
			if (allocatedbuffer != NULL) {
				tree = load_symtree(allocatedbuffer, len);
				free(allocatedbuffer);
				treesize = symtree_size(tree, true);
				fprintf(fd, "Successfuly loaded symbols from symtreedump1.json, totalling %u bytes.\n", treesize);
				if (dump_symtree(tree, &buffer, sizeof(buffer), &bufferlen)) {
					if ((fd2 = fopen("symtreedump3.json", "w"))) {
						fwrite(&buffer, bufferlen, 1, fd2);
						fclose(fd2);
					}
				} else if (bufferlen > 0) {
					printf("Failed to dump symtree due to the buffer not being large enough!\n");
				} else {
					printf("Failed to dump symtree!\n");
				}
				free_symtree(tree);
			} else {
				fprintf(fd, "Failed to load symbols from symtreedump1.json due to insufficient memory.\n");
			}
		}
		fclose(fd);
	}
	
	return rv;
}



