/**
 * symtree.h
 * Author:       Adam "beckadamtheinventor" Beckingham
 * Description:  Fast string:string dictionary structure.
 * License:      GPL3
 */

#ifndef __SYMTREE_H__
#define __SYMTREE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

// Define this to use 32-bit offsets instead of pointers for symbol tables.
// useful on 64-bit systems to roughly halve memory cost.
// #define _SYMTREE_USE_INT32_OFFSETS

// Define these to use 16-bit offsets instead of pointers for symbol tables.
// Offsets are multiplied by _SYMTREE_BLOCK_SIZE in this mode.
// Ensure symbol tables are stored end-to-end or that _SYMTREE_BLOCK_SIZE == 1
// roughly quarters memory cost, but limits the capabilities of the library.
// #define _SYMTREE_USE_INT16_OFFSETS
// #define _SYMTREE_BLOCK_SIZE 1


// Define this to enable json pretty-printing
#define _SYMTREE_DUMP_PRETTY_JSON

// File header and footer for json dump files
#ifdef _SYMTREE_DUMP_PRETTY_JSON
const char *symtree_file_header = "{";
const char *symtree_file_footer = "\n}";
#else
const char *symtree_file_header = "{";
const char *symtree_file_footer = "}";
#endif

// Key string to treat as the root node for json loading/unloading
const char *symtree_root_node_key = "<root>";

// Convert character to dictionary key number
#ifndef _PARSE_SYM_NAME_CHAR
const uint8_t symtree_parse_sym_name_char_tbl[256] = {255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 255, 255, 255, 255, 255, 255, 255, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 255, 255, 255, 255, 62, 255, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255};
#define _PARSE_SYM_NAME_CHAR(c) symtree_parse_sym_name_char_tbl[c]
#define _PARSE_SYM_NAME_CHAR_INVALID 255
#endif

// Convert dictionary key number to character
#ifndef _UNPARSE_SYM_NAME_CHAR
const char symtree_unparse_sym_name_char_tbl[256] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_\xff";
#define _UNPARSE_SYM_NAME_CHAR(c) symtree_unparse_sym_name_char_tbl[c]
#define _UNPARSE_SYM_NAME_CHAR_INVALID 0xff
#endif

// Number of allowed characters in dictionary keys
#ifndef _SYMTREE_NUM_CHARS
#define _SYMTREE_NUM_CHARS 63
#endif

// Dictionary value type
#ifndef VALUE_TYPE
#define VALUE_TYPE char*
#endif


// Symbol tree and subtree structure
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

// Defines how to read a subtree from a symbol tree.
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

// Defines how to write a subtree to a symbol tree.
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

// Defines what value is considered an empty subtree
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

// Define _malloc and _free to use custom malloc routines when allocating/freeing tree structures.
#ifndef _malloc
#define _malloc malloc
#endif
#ifndef _free
#define _free free
#endif

// Allocate a symbol tree.
// @returns Created and zeroed symbol tree. Returns NULL if failed to allocate memory.
static symtree_t *alloc_symtree(void);

// Clone a symbol tree recursively.
// @param tree Symbol tree to clone.
// @returns Cloned symbol tree. Returns NULL if failed to allocate memory.
// NOTE: NOT YET IMPLEMENTED.
static symtree_t *clone_symtree(symtree_t *tree);

// Locate a symbol and return its value.
// @param tree Symbol tree to search.
// @param name Dictionary key to search for.
// @param namelen Length of dictionary key in bytes. Set to 0 to substitute strlen(name).
// @returns Symbol value.
static VALUE_TYPE find_sym(symtree_t *tree, const char *name, size_t namelen);

// Locate a symbol and return a pointer to its value.
// @param tree Symbol tree to search.
// @param name Dictionary key to search for.
// @param namelen Length of dictionary key in bytes. Set to 0 to substitute strlen(name).
// @returns Pointer to symbol value.
static VALUE_TYPE *find_sym_addr(symtree_t *tree, const char *name, size_t namelen);


// Add a key to a symbol tree (if it doesn't exist) and assign a value.
// @param tree Symbol tree to add to.
// @param name Name of dictionary key.
// @param namelen Length of dictionary key in bytes. Set to 0 to substitute strlen(name).
// @param value Value to set the symbol to.
// @returns Value the symbol was set to, or NULL if failed.
static VALUE_TYPE new_sym(symtree_t *tree, const char *name, size_t namelen, VALUE_TYPE value);

// Assign a value to a key in a symbol tree.
// @param tree Symbol tree to modify.
// @param name Name of dictionary key.
// @param namelen Length of dictionary key in bytes. Set to 0 to substitute strlen(name).
// @param value Value to set the symbol to.
// @returns Value the symbol was set to, or NULL if failed. (eg. the key doesn't exist)
static VALUE_TYPE set_sym(symtree_t *tree, const char *name, size_t namelen, VALUE_TYPE value);

// Remove a key from a symbol tree.
// @param tree Symbol tree to remove from.
// @param name Name of dictionary key.
// @param namelen Length of dictionary key in bytes. Set to 0 to substitute strlen(name).
// @param free_value Whether or not to free the value. Note: this uses free() not _free().
// @returns True if successfuly deleted the key, False if failed. (eg. the key doesn't exist)
static bool del_sym(symtree_t *tree, const char *name, size_t namelen, bool free_value);

// Get the size of a symbol tree with or without including the lengths of value strings.
// @param tree Symbol tree to get the size of.
// @param include_value_strings Whether to include value strings in the size calculation.
// @returns Size of the symbol tree in bytes.
static size_t symtree_size(symtree_t *tree, bool include_value_strings);

// Dump a symbol tree to a semi-readable text format into a buffer for debugging the tree structure.
// @param tree Symbol tree to dump.
// @param buffer Buffer to dump text into.
// @param bufferlen Length of the buffer to dump text into.
// @param len Pointer to length of dumped text.
// @returns True if success, False if the buffer isn't large enough.
static bool debug_dump_symtree(symtree_t *tree, char *buffer, size_t bufferlen, size_t *len);

// Dump a symbol tree to a buffer in json format
// @param tree Symbol tree to dump.
// @param buffer Buffer to dump data into.
// @param bufferlen Length of the buffer to dump text into.
// @param len Pointer to length of dumped data.
// @returns True if success, False if the buffer isn't large enough.
static bool dump_symtree(symtree_t *tree, char *buffer, size_t bufferlen, size_t *len);

// Load a symbol tree from json format.
// @param data Binary data to load data from.
// @param datalen Length of binary data to load from.
// @returns Pointer to new symbol tree if success, NULL if failed.
static symtree_t *load_symtree(const char *data, size_t datalen);

// Add symbols to a tree from data in json format.
// @param tree Pointer to symbol tree to add data to.
// @param data Binary data to load data from.
// @param datalen Length of binary data to load from.
// @returns Pointer to new symbol tree if success, NULL if failed.
static symtree_t *append_symtree(symtree_t *tree, const char *data, size_t datalen);


// Recursive function used internally within dump_symtree.
static bool _dump_symtree(symtree_t *tree, char *buffer, size_t bufferlen, size_t *len, const char *prefix) {
	size_t i = 0, prefixlen = 0, newlen, curlen = 0;
	char *newprefix;
	char c;
	if (tree->leaf != NULL) {
#ifdef _SYMTREE_DUMP_PRETTY_JSON
		if (curlen + 2 >= bufferlen) {
			*len = curlen;
			return false;
		}
		buffer[curlen++] = '\n';
		buffer[curlen++] = '\t';
#endif
		if (prefix == NULL) {
			// printf("Found data node %s\n", symtree_root_node_key);
			if (curlen + strlen(symtree_root_node_key) + 4 >= bufferlen) {
				*len = curlen;
				return false;
			}
			buffer[curlen++] = '"';
			memcpy(buffer, symtree_root_node_key, strlen(symtree_root_node_key));
			buffer[curlen++] = '"';
		} else {
			// printf("Found data node %s\n", prefix);
			if (curlen + strlen(prefix) + 4 >= bufferlen) {
				*len = curlen;
				return false;
			}
			buffer[curlen++] = '"';
			while (prefix[i]) {
				buffer[curlen++] = prefix[i++];
			}
			buffer[curlen++] = '"';
			i = 0;
		}
		buffer[curlen++] = ':';
		buffer[curlen++] = '"';
		while ((c = tree->leaf[i])) {
			if (curlen + 1 >= bufferlen) {
				*len = curlen;
				return false;
			}
			if (c == '\n') {
				if (curlen + 2 >= bufferlen) {
					*len = curlen;
					return false;
				}
				buffer[curlen++] = '\\';
				buffer[curlen++] = 'n';
			} else if (c == '\t') {
				if (curlen + 2 >= bufferlen) {
					*len = curlen;
					return false;
				}
				buffer[curlen++] = '\\';
				buffer[curlen++] = 't';
			} else {
				if (c == '"') {
					if (curlen + 2 >= bufferlen) {
						*len = curlen;
						return false;
					}
					buffer[curlen++] = '\\';
				}
				buffer[curlen++] = c;
			}
			i++;
		}
		if (curlen + 2 >= bufferlen) {
			*len = curlen;
			return false;
		}
		buffer[curlen++] = '"';
		buffer[curlen++] = ',';
	}
	if (prefix != NULL) {
		prefixlen += strlen(prefix);
	}
	if ((newprefix = malloc(prefixlen+2)) == NULL) {
		*len = curlen;
		return false;
	}
	if (prefixlen > 0) {
		memcpy(newprefix, prefix, prefixlen);
	}
	newprefix[prefixlen+1] = 0;
	for (i=0; i<_SYMTREE_NUM_CHARS; i++) {
		if (tree->symbols[i] != _SYM_NULL) {
			newprefix[prefixlen] = _UNPARSE_SYM_NAME_CHAR(i);
			// printf("Found tree node %s\n", newprefix);
			if (_dump_symtree(_READ_SYMBOL_TREE(tree, i), &buffer[curlen], bufferlen-curlen, &newlen, newprefix)) {
				curlen += newlen;
			} else {
				free(newprefix);
				*len = curlen + newlen;
				return false;
			}
		}
	}
	free(newprefix);
	*len = curlen;
	return true;
}

static bool dump_symtree(symtree_t *tree, char *buffer, size_t bufferlen, size_t *len) {
	size_t curlen = 0, newlen;
	if (curlen + sizeof(symtree_file_header) >= bufferlen) {
		*len = curlen;
		return false;
	}
	memcpy(buffer, symtree_file_header, strlen(symtree_file_header));
	curlen += strlen(symtree_file_header);
	if (!(_dump_symtree(tree, &buffer[curlen], bufferlen-curlen, &newlen, NULL))) {
		*len = curlen + newlen;
		return false;
	}
	if (newlen > 0) {
		curlen = curlen + newlen - 1; // rewind a byte to remove extra comma
	}
	if (curlen + strlen(symtree_file_footer) >= bufferlen) {
		*len = curlen;
		return false;
	}
	memcpy(&buffer[curlen], symtree_file_footer, strlen(symtree_file_footer));
	*len = curlen + strlen(symtree_file_footer);
	return true;
}

static symtree_t *load_symtree(const char *data, size_t datalen) {
	symtree_t *tree = alloc_symtree();
	if (tree == NULL) {
		return NULL;
	}
	return append_symtree(tree, data, datalen);
}

// Used internally by append_symtree to read quoted strings
static char *_read_until(const char *data, size_t datalen, char end, size_t *read) {
	char c, *str;
	size_t i = 0;
	
	while (i < datalen) {
		c = data[i++];
		if (c == '\\') {
			if (i+1 >= datalen) {
				return NULL;
			}
			i++;
		} else if (c == end) {
			break;
		}
	}
	*read = i;
	str = malloc(i);
	if (str != NULL) {
		if (i <= 1) {
			return NULL;
		}
		memcpy(str, data, i-1);
		str[i-1] = 0;
	}
	return str;
}

static symtree_t *append_symtree(symtree_t *tree, const char *data, size_t datalen) {
	char c;
	size_t i = 0;
	char *key = NULL;
	if (i+1 >= datalen) {
		return NULL;
	}
	if (data[i++] != '{') {
		return NULL;
	}
	while (i < datalen) {
		c = data[i++];
		if (c == ' ' || c == '\t' || c == '\n' || c == ':' || c == ',') {
			continue;
		} else if (c == '"') {
			size_t read;
			if (key == NULL) {
				key = _read_until(&data[i], datalen-i, '"', &read);	
				if (key == NULL) {
					return NULL;
				}
				// printf("Got key %s\n", key);
			} else {
				char *value = _read_until(&data[i], datalen-i, '"', &read);
				if (value == NULL) {
					return NULL;
				}
				if (strcmp(key, symtree_root_node_key)) {
					// if not the root node, add to the tree normally
					new_sym(tree, key, 0, value);
				} else {
					// if the root node, add manually to the tree
					tree->leaf = value;
				}
				free(key);
				key = NULL;
			}
			i += read;
		} else if (c == '}') {
			break;
		} else {
			return NULL;
		}
	}
	return tree;
}

static symtree_t *alloc_symtree(void) {
	symtree_t *tree = _malloc(sizeof(symtree_t));
	if (tree == NULL) {
		return NULL;
	}
	memset(tree, 0, sizeof(symtree_t));
	return tree;
}

static void free_symtree(symtree_t *tree) {
	for (uint8_t c=0; c<_SYMTREE_NUM_CHARS; c++) {
		if (tree->symbols[c] != _SYM_NULL) {
			free_symtree(_READ_SYMBOL_TREE(tree, c));
		}
	}
	_free(tree);
}

static symtree_t *clone_symtree(symtree_t *tree) {
	return NULL;
}

static bool debug_dump_symtree(symtree_t *tree, char *buffer, size_t bufferlen, size_t *len) {
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
			if (!debug_dump_symtree(_READ_SYMBOL_TREE(tree, i), &buffer[curlen], bufferlen-curlen, &newlen)) {
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

static size_t symtree_size(symtree_t *tree, bool include_value_strings) {
	size_t len = sizeof(symtree_t);
	for (uint8_t i=0; i<_SYMTREE_NUM_CHARS; i++) {
		if (tree->symbols[i] != _SYM_NULL) {
			len += symtree_size(_READ_SYMBOL_TREE(tree, i), include_value_strings);
		}
	}
	if (tree->leaf != NULL) {
		len += sizeof(VALUE_TYPE);
		if (include_value_strings) {
			len += strlen(tree->leaf) + 1;
		}
	}
	return len;
}

static VALUE_TYPE new_sym(symtree_t *tree, const char *name, size_t namelen, VALUE_TYPE value) {
	symtree_t *st;
	int8_t c;
	size_t i = 0;
	if (namelen == 0) {
		namelen = strlen(name);
	}
	if (namelen > 1) {
		while (i < namelen) {
			c = _PARSE_SYM_NAME_CHAR(name[i]);
			if (c == _PARSE_SYM_NAME_CHAR_INVALID)
				return NULL;
			if (tree->symbols[c] == _SYM_NULL) {
				while (i < namelen) {
					if ((st = alloc_symtree()) == NULL) {
						return NULL;
					}
					c = _PARSE_SYM_NAME_CHAR(name[i]);
					if (c == _PARSE_SYM_NAME_CHAR_INVALID) {
						free_symtree(st);
						return NULL;
					}
					i++;
					_WRITE_SYMBOL_TREE(tree, c, st);
					tree = st;
				}
				return (tree->leaf = value);
			} else {
				tree = _READ_SYMBOL_TREE(tree, c);
				i++;
			}
		}
	} else if (namelen > 0) {
		c = _PARSE_SYM_NAME_CHAR(name[0]);
		if (c == _PARSE_SYM_NAME_CHAR_INVALID)
			return NULL;
	} else {
		return (tree->leaf = value);
	}
	if (tree->symbols[c] == _SYM_NULL) {
		if ((st = alloc_symtree()) == NULL) {
			return NULL;
		}
		_WRITE_SYMBOL_TREE(tree, c, st);
	} else {
		st = _READ_SYMBOL_TREE(tree, c);
	}
	return (st->leaf = value);
}

static VALUE_TYPE find_sym(symtree_t *tree, const char *name, size_t namelen) {
	VALUE_TYPE *sym = find_sym_addr(tree, name, namelen);
	if (sym == NULL) {
		return NULL;
	}
	return *sym;
}

static bool del_sym(symtree_t *tree, const char *name, size_t namelen, bool free_value) {
	VALUE_TYPE *sym = find_sym_addr(tree, name, namelen);
	if (sym == NULL) {
		return false;
	}
	if (free_value && *sym != NULL) {
		free(*sym);
	}
	*sym = NULL;
	return true;
}

static VALUE_TYPE set_sym(symtree_t *tree, const char *name, size_t namelen, VALUE_TYPE value) {
	VALUE_TYPE *sym = find_sym_addr(tree, name, namelen);
	if (sym == NULL) {
		return NULL;
	}
	return (*sym = value);
}

static VALUE_TYPE *find_sym_addr(symtree_t *tree, const char *name, size_t namelen) {
	int8_t c;
	size_t i = 0;
	if (namelen == 0) {
		namelen = strlen(name);
	}
	while (i < namelen) {
		c = _PARSE_SYM_NAME_CHAR(name[i]);
		i++;
		if (c == _PARSE_SYM_NAME_CHAR_INVALID) {
			return NULL;
		} else {
			if (tree->symbols[c] == _SYM_NULL) {
				return NULL;
			} else {
				tree = _READ_SYMBOL_TREE(tree, c);
				if (i >= namelen) {
					return &tree->leaf;
				}
			}
		}
	}
	return NULL;
}

#ifdef __cplusplus
}
#endif

#endif


