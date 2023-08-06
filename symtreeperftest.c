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

// #define _SYMTREE_USE_INT32_OFFSETS
#include "symtree.h"

#define NUM_TESTS (65536*128)
#define TEST_KEY_STR "var%X"
#define TEST_KEY_LEN 12

int main(int argc, char *argv[]) {
	FILE *fd;
	size_t treesize;
	char *varnames;
	char *sym;
	clock_t start, end;
	symtree_t *tree = alloc_symtree();

	if ((varnames = malloc(NUM_TESTS * TEST_KEY_LEN)) == NULL) {
		printf("Failed to malloc test symbol names\n");
		return 1;
	}
	for (int test=0; test<NUM_TESTS; test++) {
		sprintf(&varnames[test * TEST_KEY_LEN], TEST_KEY_STR, test);
		varnames[test * TEST_KEY_LEN + TEST_KEY_LEN - 1] = 0;
	}

	if ((fd = fopen("symtreeperftest.txt", "wb"))) {
		bool success = true;
		start = clock();
		for (int test=0; test<NUM_TESTS; test++) {
			if (new_sym(tree, &varnames[test * TEST_KEY_LEN], 0, "abcdefgh") == NULL) {
				fprintf(fd, "Failed to locate symbol \"%s\".\n", &varnames[test * TEST_KEY_LEN]);
				success = false;
				break;
			}
		}
		if (success) {
			end = clock();
			fprintf(fd, "Took %f seconds to add %d symbols to tree.\n", (end-start) / (float)CLOCKS_PER_SEC, NUM_TESTS);
			start = clock();
			for (int test=0; test<NUM_TESTS; test++) {
				if (find_sym(tree, &varnames[test * TEST_KEY_LEN], 0) == NULL) {
					fprintf(fd, "Failed to locate symbol \"%s\".\n", &varnames[test * TEST_KEY_LEN]);
					success = false;
					break;
				}
			}
			if (success) {
				end = clock();
				fprintf(fd, "Took %f seconds to locate %d symbols in tree.\n", (end-start) / (float)CLOCKS_PER_SEC, NUM_TESTS);
				start = clock();
				for (int test=0; test<NUM_TESTS; test++) {
					if (set_sym(tree, &varnames[test * TEST_KEY_LEN], 0, "abcdefgh") == NULL) {
						fprintf(fd, "Failed to locate symbol \"%s\".\n", &varnames[test * TEST_KEY_LEN]);
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
						if (del_sym(tree, &varnames[test * TEST_KEY_LEN], 0, false) == false) {
							fprintf(fd, "Failed to delete symbol \"%s\".\n", &varnames[test * TEST_KEY_LEN]);
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

