
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
#include "../symtree.c"

int main(int argc, char **argv) {
	uint8_t buffer;
	symtree_t *tree;
	FILE *fd;
	char *sym;
	char c;
	size_t treesize, entries = 0;
	if (argc != 2) {
		printf("Usage: %s data.bin\n", argv[0]);
		return 0;
	}
	if ((fd = fopen(argv[1], "rb")) == NULL) {
		return -1;
	}
	if ((tree = alloc_symtree()) == NULL) {
		memoryerror:;
		printf("Error: Ran out of memory.\n");
		fclose(fd);
		return 1;
	}
	do {
		char *key = malloc(256);
		char *value = malloc(32768);
		size_t i = 0;
		if (key == NULL || value == NULL) {
			goto memoryerror;
		}
		do {
			c = fgetc(fd);
			if (c == EOF) {
				key[i++] = 0;
				break;
			}
			key[i++] = c;
		} while (c != 0 && i < 256);
		if (i >= 256) {
			value[255] = 0;
		}
		if (c != EOF) {
			i = 0;
			do {
				c = fgetc(fd);
				if (c == EOF) {
					value[i++] = 0;
					break;
				}
				value[i++] = c;
			} while (c != 0 && i < 32768);
			if (i >= 32768) {
				value[32767] = 0;
			}
			if (c != EOF) {
				char *valstr = malloc(strlen(value)+1);
				if (valstr == NULL) {
					goto memoryerror;
				}
				strcpy(valstr, value);
				if ((new_sym(tree, key, 0, valstr)) == NULL) {
					printf("Error: Failed to add symbol to tree: \"%s\"\nIf the symbol is valid, this is a memory error.\n", &key);
					fclose(fd);
					return 2;
				}
				// printf("Added symbol \"%s\" to tree.\n", &key);
				entries++;
			}
		}
		free(value);
	} while (c != EOF);
	fclose(fd);
	treesize = symtree_size(tree, true) / 1024;
	printf("Successfuly added %u entries to tree, totalling %u kb. (%f gb)\n", entries, treesize, (double)treesize/(1024*1024.0f));

	if ((fd = fopen("dictionarydump.txt", "wb")) != NULL) {
		#define BUFFER_LEN (32*1024*1024)
		size_t buffer_len = 0;
		uint8_t *buffer = malloc(BUFFER_LEN);
		if (buffer == NULL) {
			goto memoryerror;
		}
		if (!dump_symtree(tree, buffer, BUFFER_LEN, &buffer_len)) {
			printf("Failed to dump symtree!\n");
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


