
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#define _SYMTREE_USE_INT32_OFFSETS
// #define _SYMTREE_BLOCK_SIZE 8

#define _PARSE_SYM_NAME_CHAR(c) ((c)=='-' ? 37 : ((c)==' ' ? 36 : ((unsigned)(c)-'A'<26 ? (c)-'A' : ((unsigned)(c)-'a'<26 ? (c)-'a' : ((unsigned)(c)-'0'<10 ? (c)+26-'0' : -1)))))
#define _UNPARSE_SYM_NAME_CHAR(c) ((c)==37 ? '-' : ((c)==36 ? ' ' : ((unsigned)(c)<26 ? (c)+'A' : ((unsigned)(c)-26<10 ? (c)+'0'-26 : -1))))
#define _SYMTREE_NUM_CHARS 38
#include "../symtree.h"

int main(int argc, char **argv) {
	uint8_t buffer;
	symtree_t *tree;
	FILE *fd;
	char c, *sym, *fname;
	size_t treesize;
	if (argc != 2) {
		fname = "WebstersEnglishDictionary/dictionary_compact.json";
	} else {
		if (!strcmp(argv[1], "-h") || !strcmp(argv[1], "/h")) {
			printf("Usage: %s [data.json]\n", argv[0]);
			return 0;
		}
		fname = argv[1];
	}
	if ((fd = fopen(fname, "r"))) {
		char *mallocbuffer;
		size_t len;
		fseek(fd, 0, 2);
		len = ftell(fd);
		fseek(fd, 0, 0);
		if ((mallocbuffer = malloc(len))) {
			fread(mallocbuffer, len, 1, fd);
			fclose(fd);
			tree = load_symtree(mallocbuffer, len);
			if (tree == NULL) {
				printf("Failed to load symtree from file \"%s\"! (presumeably due to a parse error)\n", fname);
				return 2;
			}
		} else {
			fclose(fd);
			printf("Insufficient memory to load dictionary_compact.json\n");
			return 1;
		}
	} else {
		return -1;
	}
	treesize = symtree_size(tree, true) / 1024;
	printf("Successfuly added json to tree, totalling %u kb. (%f gb)\n", treesize, (double)treesize/(1024*1024.0f));

	if ((fd = fopen("dictionarydump.txt", "wb")) != NULL) {
		#define BUFFER_LEN (24*1024*1024)
		size_t buffer_len = 0;
		uint8_t *buffer = malloc(BUFFER_LEN);
		if (buffer == NULL) {
			printf("Insufficient memory to malloc dump buffer!\n");
			return 1;
		}
		if (!dump_symtree(tree, buffer, BUFFER_LEN, &buffer_len)) {
			printf("Failed to dump symtree! Buffer length is insufficient to dump.\n");
		}
		fwrite(buffer, buffer_len, 1, fd);
		fclose(fd);
		#undef BUFFER_LEN
	}
	
	{
		char inputstr[512];
		do {
			double dt;
			printf("> ");
			fscanf(stdin, "%s", inputstr);
			if (*inputstr != 0) {
				clock_t start = clock();
				for (int n=0; n<9999999; n++) {
					find_sym(tree, inputstr, 0);
				}
				sym = find_sym(tree, inputstr, 0);
				dt = (1000.0f * (clock()-start)) / (float)CLOCKS_PER_SEC;
				if (sym == NULL) {
					printf("\nSymbol not found.\n");
				} else {
					printf("\n%s\n", sym);
				}
				printf("Took %f ms per ten million searches.\n", dt);
			}
		} while (*inputstr != 0);
	}

	return 0;
}


