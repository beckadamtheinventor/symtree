
# Symtree

Implements a fast string-indexed dictionary structure, that can locate a value given a key in time proportional to the key string length.

## Usage


Allocates a symbol tree, zeroes it, and returns it. Returns NULL if failed to allocate.

`symtree_t *alloc_symtree(void);`


Frees a symbol tree recursively

`void free_symtree(symtree_t *tbl);`


Returns symbol if found in the symbol tree, otherwise NULL.
If namelen == 0, strlen(name) will be substituted.

`VALUE_TYPE find_sym(symtree_t *tbl, const char *name, size_t namelen);`


Gets a pointer to a symbol if found in the symbol tree, otherwise NULL.
If namelen == 0, strlen(name) will be used instead.

`VALUE_TYPE *find_sym_addr(symtree_t *tbl, const char *name, size_t namelen);`


Returns symbol if successfuly created and linked into the symbol tree, otherwise NULL.
If namelen == 0, strlen(name) will be substituted.

`VALUE_TYPE new_sym(symtree_t *tbl, const char *name, size_t namelen, VALUE_TYPE value);`


Sets and returns a symbol if found in the symbol tree, otherwise NULL.
If namelen == 0, strlen(name) will be used instead.

`VALUE_TYPE set_sym(symtree_t *tbl, const char *name, size_t namelen, VALUE_TYPE value);`


Returns true if the symbol existed and was successfuly deleted, otherwise false.
If namelen == 0, strlen(name) will be substituted.
If free_value is true, the symbol value will be freed if it is not NULL.

`bool del_sym(symtree_t *tbl, const char *name, size_t namelen, bool free_value);`


Return the size in bytes of a symbol tree recursively. if include_value_strings == true, include the length in bytes of string values.

`size_t symtree_size(symtree_t *tbl, bool include_value_strings);`


Dump a symbol tree's data in a semi-readable text format into a buffer for debugging the tree structure. Returns false if the buffer isn't large enough.

`bool debug_dump_symtree(symtree_t *tree, uint8_t *buffer, size_t bufferlen, size_t *len);`


Dump a symbol tree's data in json format into a buffer. Returns false if the buffer isn't large enough.

`bool dump_symtree(symtree_t *tree, uint8_t *buffer, size_t bufferlen, size_t *len);`


Load a symbol tree from json format. Returns a pointer to a new symbol tree, or NULL if failed.

`static symtree_t *load_symtree(const char *data, size_t datalen);`


Append symbols to a symbol tree from json format. Returns a pointer to the symbol tree, or NULL if failed.

`static symtree_t *append_symtree(symtree_t *tree, const char *data, size_t datalen);`


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
Roughly quarters memory cost, (on 64-bit machines) but limits the capabilities of the library unless `_SYMTREE_BLOCK_SIZE` is set to the size of the `symtree_t` structure or higher. (which will also require the user to implement their own `_malloc` and `_free` to ensure proper alignment of symbol trees)

`#define _SYMTREE_USE_INT16_OFFSETS`

`#define _SYMTREE_BLOCK_SIZE 1`


## Performance

Performance tests are run with 2^23 (8388608) symbols added, located, and deleted.

Each symbol key is of the format `var%X` where `%X` is set to the test loop counter represented in hexadecimal, and each symbol value is a pointer to an 8-character string.
Tree size with values accounts for each usage of the 8-character string, which will be higher than what task manager shows; in practice with unique strings per symbol this discrepancy is not the case.

Note: the maximum number of symbols that can be safely addressed in 32-bit offset mode is 2^31 divided by the symbol tree size in bytes.
I have not run a performance test for the 16-bit offset mode, because this is generally intended for embedded and lospec devices, and because the number of symbols it can fit takes an insignificant amount of time performance wise on any modern hardware.

## Library defaults (Intel i7-10700KF)

Adding 2^23 symbols: 1.6 seconds.

Locating 2^23 symbols: 0.904 seconds.

Locating and setting 2^23 symbols: 0.575 seconds.

Deleting 2^23 symbols: 0.317 seconds.

Tree size without values: 4259842 kb. (4.06 gb)

Tree size with values: 4333570 kb. (4.13gb)

## 32-bit offsets (Intel i7-10700KF)

Adding 2^23 symbols: 1.102 seconds.

Locating 2^23 symbols: 0.845 seconds.

Locating and setting 2^23 symbols: 0.534 seconds.

Deleting 2^23 symbols: 0.297 seconds.

Tree size without values: 2228225 kb. (2.125 gb)

Tree size with values: 2301953 kb. (2.195 gb)

