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
#include <time.h>

#include "symtree.h"

#define NUM_TESTS (65536*128)

int main(int argc, char *argv[]) {
	FILE *fd;
	size_t treesize;
	char varname[64];
	char *sym;
	clock_t start, end;
	symtree_t *tree = alloc_symtree();

	if ((fd = fopen("symtreeperftest.txt", "wb"))) {
		bool success = true;
		start = clock();
		for (int test=0; test<NUM_TESTS; test++) {
			sprintf(&varname, "var%X", test);
			if (new_sym(tree, &varname, 0, "abcdefgh") == NULL) {
				fprintf(fd, "Failed to locate symbol \"%s\".\n", &varname);
				success = false;
				break;
			}
		}
		if (success) {
			end = clock();
			fprintf(fd, "Took %f seconds to add %d symbols to tree.\n", (end-start) / (float)CLOCKS_PER_SEC, NUM_TESTS);
			start = clock();
			for (int test=0; test<NUM_TESTS; test++) {
				sprintf(&varname, "var%X", test);
				if (find_sym(tree, &varname, 0) == NULL) {
					fprintf(fd, "Failed to locate symbol \"%s\".\n", &varname);
					success = false;
					break;
				}
			}
			if (success) {
				end = clock();
				fprintf(fd, "Took %f seconds to locate %d symbols in tree.\n", (end-start) / (float)CLOCKS_PER_SEC, NUM_TESTS);
				start = clock();
				for (int test=0; test<NUM_TESTS; test++) {
					sprintf(&varname, "var%X", test);
					if (set_sym(tree, &varname, 0, "abcdefgh") == NULL) {
						fprintf(fd, "Failed to locate symbol \"%s\".\n", &varname);
						success = false;
						break;
					}
				}
				if (success) {
					end = clock();
					fprintf(fd, "Took %f seconds to locate and set %d symbols in tree.\n", (end-start) / (float)CLOCKS_PER_SEC, NUM_TESTS);
					treesize = symtree_size(tree, false);
					fprintf(fd, "Tree size: %d kb. (%f gb)\n", treesize/1024, treesize/(1024*1024*1024.0f));
					treesize = symtree_size(tree, true);
					fprintf(fd, "Tree size +values: %d kb. (%f gb)\n", treesize/1024, treesize/(1024*1024*1024.0f));
					start = clock();
					for (int test=0; test<NUM_TESTS; test++) {
						sprintf(&varname, "var%X", test);
						if (del_sym(tree, &varname, 0, false) == false) {
							fprintf(fd, "Failed to delete symbol \"%s\".\n", &varname);
							success = false;
							break;
						}
					}
					end = clock();
					fprintf(fd, "Took %f seconds to delete %d symbols in tree.\n", (end-start) / (float)CLOCKS_PER_SEC, NUM_TESTS);
				}
			}
		}
		free_symtree(tree);
		fclose(fd);
	}
	printf("Success. Results in \"symtreeperftest.txt\".\n");
	return 0;
}

