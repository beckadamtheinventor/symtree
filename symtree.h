/**
 * symtree.h
 * Author:       Adam "beckadamtheinventor" Beckingham
 * Description:  Fast string:string dictionary structure.
 * License:      GPL3
 */

#ifndef __SYMTREE_H__
#define __SYMTREE_H__

#include <stdint.h>
#include <string.h>
#include <stdbool.h>

// Uncomment to use 32-bit offsets instead of pointers for symbol tables.
// useful on 64-bit systems to roughly halve memory cost.
// #define _SYMTREE_USE_INT32_OFFSETS

// Uncomment to use 16-bit offsets instead of pointers for symbol tables.
// Offsets are multiplied by _SYMTREE_BLOCK_SIZE.
// Ensure symbol tables are stored end-to-end or that _SYMTREE_BLOCK_SIZE == 1
// roughly quarters memory cost, but limits the capabilities of the library.
// #define _SYMTREE_USE_INT16_OFFSETS
// #define _SYMTREE_BLOCK_SIZE 1

// Convert character to dictionary key number
#ifndef _PARSE_SYM_NAME_CHAR
#define _PARSE_SYM_NAME_CHAR(c) ((c)=='_' ? 62 : ((unsigned)(c)-'A'<26 ? (c)-'A' : ((unsigned)(c)-'a'<26 ? (c)+26-'a' : ((unsigned)(c)-'0'<10 ? (c)+52-'0' : -1))))
#endif

// Convert dictionary key number to character
#ifndef _UNPARSE_SYM_NAME_CHAR
#define _UNPARSE_SYM_NAME_CHAR(c) ((c)==62 ? '_' : ((c)<26 ? (c)+'A' : ((unsigned)(c)-26<26 ? (c)+'a'-26 : ((unsigned)(c)-52<10 ? (c)+'0'-52 : -1))))
#endif

// Number of allowed characters in dictionary keys
#ifndef _SYMTREE_NUM_CHARS
#define _SYMTREE_NUM_CHARS 63
#endif

// Dictionary value type
#ifndef VALUE_TYPE
#define VALUE_TYPE char*
#endif

typedef struct _symtree {
	VALUE_TYPE leaf;
#ifdef _SYMTREE_USE_INT32_OFFSETS
	int32_t symbols[_SYMTREE_NUM_CHARS];
#else
#ifdef _SYMTREE_USE_INT16_OFFSETS
	int16_t symbols[_SYMTREE_NUM_CHARS];
#else
	void *symbols[_SYMTREE_NUM_CHARS];
#endif
#endif
} symtree_t;

#ifndef _READ_SYMBOL_TREE
#ifdef _SYMTREE_USE_INT32_OFFSETS
#define _READ_SYMBOL_TREE(t,c) ((symtree_t*)(((uint8_t*)(t)) + (t)->symbols[c]))
#else
#ifdef _SYMTREE_USE_INT16_OFFSETS
#define _READ_SYMBOL_TREE(t,c) ((symtree_t*)((uint8_t*)(t) + (t)->symbols[c] * _SYMTREE_BLOCK_SIZE))
#else
#define _READ_SYMBOL_TREE(t,c) ((symtree_t*)((t)->symbols[c]))
#endif
#endif
#endif

#ifndef _WRITE_SYMBOL_TREE
#ifdef _SYMTREE_USE_INT32_OFFSETS
#define _WRITE_SYMBOL_TREE(t,c,v) ((t)->symbols[c] = ((uint8_t*)(v) - (uint8_t*)(t)))
#else
#ifdef _SYMTREE_USE_INT16_OFFSETS
#define _WRITE_SYMBOL_TREE(t,c,v) { int a = ((uint8_t*)(v) - (uint8_t*)(t)) / _SYMTREE_BLOCK_SIZE; if (a > -32768 && a < 32767) (t)->symbols[c] = a; else (t)->symbols[c] = 0; }
#else
#define _WRITE_SYMBOL_TREE(t,c,v) ((t)->symbols[c] = (v))
#endif
#endif
#endif

#ifndef _SYM_NULL
#ifdef _SYMTREE_USE_INT32_OFFSETS
#define _SYM_NULL 0
#else
#ifdef _SYMTREE_USE_INT16_OFFSETS
#define _SYM_NULL 0
#else
#define _SYM_NULL NULL
#endif
#endif
#endif

#ifndef _malloc
#define _malloc malloc
#endif

#ifndef _free
#define _free free
#endif

static bool dump_symtree(symtree_t *tree, uint8_t *buffer, size_t bufferlen, size_t *len) {
	size_t curlen = 0;
	size_t i = 0;
	if (curlen + 1 >= bufferlen) {
		*len = bufferlen;
		return false;
	}
	buffer[curlen++] = '[';
	for (i=0; i<_SYMTREE_NUM_CHARS; i++) {
		if (tree->symbols[i] != _SYM_NULL) {
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

static size_t symtree_size(symtree_t *tbl, bool include_value_strings) {
	size_t len = sizeof(symtree_t);
	for (uint8_t i=0; i<_SYMTREE_NUM_CHARS; i++) {
		if (tbl->symbols[i] != _SYM_NULL) {
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

static symtree_t *alloc_symtree(void) {
	symtree_t *tree = _malloc(sizeof(symtree_t));
	if (tree == NULL) {
		return NULL;
	}
	memset(tree, 0, sizeof(symtree_t));
	return tree;
}

static void free_symtree(symtree_t *tbl) {
	for (uint8_t c=0; c<_SYMTREE_NUM_CHARS; c++) {
		if (tbl->symbols[c] != _SYM_NULL) {
			free_symtree(_READ_SYMBOL_TREE(tbl, c));
		}
	}
	_free(tbl);
}

static VALUE_TYPE new_sym(symtree_t *tbl, const char *name, size_t namelen, VALUE_TYPE value) {
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
		if (tbl->symbols[c] == _SYM_NULL) {
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
	if (tbl->symbols[c] != _SYM_NULL) {
		free_symtree(_READ_SYMBOL_TREE(tbl, c));
	}
	if ((st = alloc_symtree()) == NULL) {
		return NULL;
	}
	_WRITE_SYMBOL_TREE(tbl, c, st);
	return (st->leaf = value);
}

static VALUE_TYPE find_sym(symtree_t *tbl, const char *name, size_t namelen) {
	VALUE_TYPE *sym = find_sym_addr(tbl, name, namelen);
	if (sym == NULL) {
		return NULL;
	}
	return *sym;
}

static bool del_sym(symtree_t *tbl, const char *name, size_t namelen, bool free_value) {
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

static VALUE_TYPE set_sym(symtree_t *tbl, const char *name, size_t namelen, VALUE_TYPE value) {
	VALUE_TYPE *sym = find_sym_addr(tbl, name, namelen);
	if (sym == NULL) {
		return NULL;
	}
	return (*sym = value);
}

static VALUE_TYPE *find_sym_addr(symtree_t *tbl, const char *name, size_t namelen) {
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
			if (tbl->symbols[c] == _SYM_NULL) {
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

#endif