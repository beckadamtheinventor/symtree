
# Symtree

Implements a fast string:string dictionary structure, that can locate a value given a key in time proportional to the key string length.

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
Roughly quarters memory cost, (on 64-bit machines) but limits the capabilities of the library unless `_SYMTREE_BLOCK_SIZE` is set to the size of the `symtree_t` structure or higher. (which will also require the user to implement their own malloc/free to ensure proper alignment)

`#define _SYMTREE_USE_INT16_OFFSETS`

`#define _SYMTREE_BLOCK_SIZE 1`


## Performance

Performance tests on 2^23 (8388608) symbols added, located, and deleted.
Each symbol key and value is of the format `var%X` where `%X` is set to the test loop counter represented in hexadecimal.
Note: the maximum number of symbols that can be safely addressed in 32-bit offset mode is 2^31 divided by the symbol tree size in bytes.
I have not run a performance test for the 16-bit offset mode, because this is generally intended for embedded and lospec devices, and because the number of symbols it can fit takes an insignificant amount of time performance wise on any modern hardware.

# Library defaults (Intel i7-10700KF)
Adding 2^23 symbols: 2.457 seconds
Locating 2^23 symbols: 1.705 seconds
Tree size without values: 4259842 kb. (4.06 gb)
Tree size with values: 4341762 kb. (4.14gb)
Took 0.925 seconds to delete 2^23 symbols in tree.

# 32-bit offsets (Intel i7-10700KF)
Adding 2^23 symbols: 1.966 seconds
Locating 2^23 symbols: 1.653 seconds
Tree size without values: 2228225 kb.
Tree size with values: 2310145 kb.
Took 0.881 seconds to delete 2^23 symbols in tree.
