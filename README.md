
# Symtree

Implements a fast string:string dictionary structure.

## Usage

Dump a symbol tree's data in a somewhat readable manner into a buffer. Returns false if the buffer isn't large enough.
`bool dump_symtree(symtree_t *tree, uint8_t *buffer, size_t bufferlen, size_t *len);`

Return the size in bytes of a symbol tree recursively. if include_value_strings == true, include the length in bytes of string values.
`size_t symtree_size(symtree_t *tbl, bool include_value_strings);`

Allocates a symbol tree, zeroes it, and returns it. Returns NULL if failed to allocate.
`symtree_t *alloc_symtree(void);`

Frees a symbol tree recursively
`void free_symtree(symtree_t *tbl);`

Returns symbol if successfuly created and linked into the symbol tree, otherwise NULL.
If namelen == 0, strlen(name) will be substituted.
`VALUE_TYPE new_sym(symtree_t *tbl, const char *name, size_t namelen, VALUE_TYPE value);`

Returns symbol if found in the symbol tree, otherwise NULL.
If namelen == 0, strlen(name) will be substituted.
`VALUE_TYPE find_sym(symtree_t *tbl, const char *name, size_t namelen);`

Returns true if the symbol existed and was successfuly deleted, otherwise false.
If namelen == 0, strlen(name) will be substituted.
`bool del_sym(symtree_t *tbl, const char *name, size_t namelen);`

## Configuration

By default, uses malloc/free.
Define `_malloc` and `_free` to replace this behavior.

By default, uses pointers to store subtrees. On 64-bit machines this results in double the memory cost due to the larger pointer size.
I reccomend using the 32-bit offsets setting on 64-bit machines unless you require more than 4 gigabytes of symbol trees.

Define this to use 32-bit offsets instead of pointers for symbol tables.
Useful on 64-bit systems to roughly halve memory cost.
`#define _SYMTREE_USE_INT32_OFFSETS`

Define these to use 16-bit offsets instead of pointers for symbol tables.
Offsets are multiplied by `_SYMTREE_BLOCK_SIZE`.
Ensure symbol tables are stored end-to-end or that `_SYMTREE_BLOCK_SIZE == 1`
Roughly quarters memory cost, but limits the capabilities of the library unless `_SYMTREE_BLOCK_SIZE` is set to the size of the `symtree_t` structure or higher.
`#define _SYMTREE_USE_INT16_OFFSETS`
`#define _SYMTREE_BLOCK_SIZE 1`

