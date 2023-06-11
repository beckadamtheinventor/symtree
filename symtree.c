/**
 * symtree.c
 * Author:       Adam "beckadamtheinventor" Beckingham
 * Description:  Fast string:string dictionary structure.
 * License:      GPL3
 */

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "symtree.h"

bool dump_symtree(symtree_t *tree, uint8_t *buffer, size_t bufferlen, size_t *len) {
	size_t curlen = 0;
	uint8_t i = 0;
	if (curlen + 1 >= bufferlen) {
		*len = bufferlen;
		return false;
	}
	buffer[curlen++] = '[';
	for (i=0; i<_SYMTREE_NUM_CHARS; i++) {
		if (tree->symbols[i] != NULL) {
			size_t newlen;
			if (curlen + 2 >= bufferlen) {
				*len = bufferlen;
				return false;
			}
			buffer[curlen++] = _UNPARSE_SYM_NAME_CHAR(i);
			buffer[curlen++] = '{';
			if (curlen + 1 >= bufferlen) {
				*len = bufferlen;
				return false;
			}
			buffer[curlen++] = '\n';
			if (!dump_symtree(_READ_SYMBOL_TREE(tree, i), &buffer[curlen], bufferlen-curlen, &newlen)) {
				return false;
			}
			curlen += newlen;
			if (curlen + 3 >= bufferlen) {
				*len = bufferlen;
				return false;
			}
			buffer[curlen++] = '}';
			buffer[curlen++] = ',';
			buffer[curlen++] = '\n';
		}
	}
	if (curlen + 1 >= bufferlen) {
		*len = bufferlen;
		return false;
	}
	buffer[curlen++] = ']';
	if (tree->leaf != NULL) {
		char c;
		if (curlen + 3 >= bufferlen) {
			*len = bufferlen;
			return false;
		}
		buffer[curlen++] = '=';
		buffer[curlen++] = '"';
		i = 0;
		while ((c = tree->leaf[i++]) != 0) {
			if (c == '\n' || c == '\t' || c == '"') {
				if (curlen + 2 >= bufferlen) {
					*len = bufferlen;
					return false;
				}
				buffer[curlen++] = '\\';
				if (c == '\n') {
					buffer[curlen++] = 'n';
				} else if (c == '\t') {
					buffer[curlen++] = 't';
				} else {
					buffer[curlen++] = c;
				}
			} else {
				if (curlen + 1 >= bufferlen) {
					*len = bufferlen;
					return false;
				}
				buffer[curlen++] = c;
			}
		}
		buffer[curlen++] = '"';
	}
	*len = curlen;
	return true;
}

size_t symtree_size(symtree_t *tbl, bool include_value_strings) {
	size_t len = sizeof(symtree_t);
	for (uint8_t i=0; i<_SYMTREE_NUM_CHARS; i++) {
		if (tbl->symbols[i] != NULL) {
			len += symtree_size(_READ_SYMBOL_TREE(tbl, i), include_value_strings);
		}
	}
	if (tbl->leaf != NULL) {
		len += sizeof(VALUE_TYPE);
		if (include_value_strings) {
			len += strlen(tbl->leaf) + 1;
		}
	}
	return len;
}

symtree_t *alloc_symtree(void) {
	symtree_t *tree = _malloc(sizeof(symtree_t));
	if (tree == NULL) {
		return NULL;
	}
	memset(tree, 0, sizeof(symtree_t));
	return tree;
}

void free_symtree(symtree_t *tbl) {
	for (uint8_t c=0; c<_SYMTREE_NUM_CHARS; c++) {
		if (tbl->symbols[c] != NULL) {
			free_symtree(_READ_SYMBOL_TREE(tbl, c));
		}
	}
	_free(tbl);
}

VALUE_TYPE new_sym(symtree_t *tbl, const char *name, size_t namelen, VALUE_TYPE value) {
	symtree_t *st;
	int8_t c;
	size_t i = 0;
	if (namelen == 0) {
		namelen = strlen(name);
	}
	while (i < namelen) {
		c = _PARSE_SYM_NAME_CHAR(name[i]);
		if (c == -1)
			return NULL;
		if (tbl->symbols[c] == NULL) {
			while (i < namelen) {
				if ((st = alloc_symtree()) == NULL) {
					return NULL;
				}
				c = _PARSE_SYM_NAME_CHAR(name[i]);
				i++;
				_WRITE_SYMBOL_TREE(tbl, c, st);
				tbl = st;
			}
			return (tbl->leaf = value);
		} else {
			tbl = _READ_SYMBOL_TREE(tbl, c);
			i++;
		}
	}
	if (tbl->symbols[c] != NULL) {
		free_symtree(_READ_SYMBOL_TREE(tbl, c));
	}
	if ((st = alloc_symtree()) == NULL) {
		return NULL;
	}
	_WRITE_SYMBOL_TREE(tbl, c, st);
	return (st->leaf = value);
}

VALUE_TYPE find_sym(symtree_t *tbl, const char *name, size_t namelen) {
	VALUE_TYPE *sym = find_sym_addr(tbl, name, namelen);
	if (sym == NULL) {
		return NULL;
	}
	return *sym;
}

bool del_sym(symtree_t *tbl, const char *name, size_t namelen, bool free_value) {
	VALUE_TYPE *sym = find_sym_addr(tbl, name, namelen);
	if (sym == NULL) {
		return false;
	}
	if (free_value && *sym != NULL) {
		free(*sym);
	}
	*sym = NULL;
	return true;
}

VALUE_TYPE set_sym(symtree_t *tbl, const char *name, size_t namelen, VALUE_TYPE value) {
	VALUE_TYPE *sym = find_sym_addr(tbl, name, namelen);
	if (sym == NULL) {
		return NULL;
	}
	return (*sym = value);
}

VALUE_TYPE *find_sym_addr(symtree_t *tbl, const char *name, size_t namelen) {
	int8_t c;
	size_t i = 0;
	if (namelen == 0) {
		namelen = strlen(name);
	}
	while (i < namelen) {
		c = _PARSE_SYM_NAME_CHAR(name[i]);
		i++;
		if (c == -1) {
			return NULL;
		} else {
			if (tbl->symbols[c] == NULL) {
				return NULL;
			} else {
				tbl = _READ_SYMBOL_TREE(tbl, c);
				if (i >= namelen) {
					return &tbl->leaf;
				}
			}
		}
	}
	return NULL;
}

