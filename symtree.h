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

#ifndef _malloc
#define _malloc malloc
#endif

#ifndef _free
#define _free free
#endif


// Dump a symbol tree's data in a somewhat readable manner into a buffer. Returns false if the buffer isn't large enough.
bool dump_symtree(symtree_t *tree, uint8_t *buffer, size_t bufferlen, size_t *len);

// Return the size in bytes of a symbol tree recursively. if include_value_strings == true, include the length in bytes of string values.
size_t symtree_size(symtree_t *tbl, bool include_value_strings);

// Allocates a symbol tree, zeroes it, and returns it. Returns NULL if failed to allocate.
symtree_t *alloc_symtree(void);

// Frees a symbol tree recursively.
void free_symtree(symtree_t *tbl);

// Returns symbol if successfuly created and linked into the symbol tree, otherwise NULL.
// If namelen == 0, strlen(name) will be used instead.
VALUE_TYPE new_sym(symtree_t *tbl, const char *name, size_t namelen, VALUE_TYPE value);

// Returns symbol if found in the symbol tree, otherwise NULL.
// If namelen == 0, strlen(name) will be used instead.
VALUE_TYPE find_sym(symtree_t *tbl, const char *name, size_t namelen);

// Returns true if the symbol existed and was successfuly deleted, otherwise false.
// If namelen == 0, strlen(name) will be used instead.
// If free_value is true, the symbol value will be freed if it is not NULL.
bool del_sym(symtree_t *tbl, const char *name, size_t namelen, bool free_value);

// Sets and returns a symbol if found in the symbol tree, otherwise NULL.
// If namelen == 0, strlen(name) will be used instead.
VALUE_TYPE set_sym(symtree_t *tbl, const char *name, size_t namelen, VALUE_TYPE value);

// Gets a pointer to a symbol if found in the symbol tree, otherwise NULL.
// If namelen == 0, strlen(name) will be used instead.
VALUE_TYPE *find_sym_addr(symtree_t *tbl, const char *name, size_t namelen);

#endif