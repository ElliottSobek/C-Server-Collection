#ifndef HASHTABLE_H
#define HASHTABLE_H

#include "../types/types.h"

typedef struct ll_node_s {
	String key, value;
	struct ll_node_s *next;
} ll_node_t;

typedef ll_node_t *Ll_Node;

typedef struct ll_s {
	Ll_Node root;
} ll_t;

typedef ll_t *Ll;

typedef struct hashtable_s {
	Ll *bins;
	unsigned int max_size, cur_size;
} hashtable_t;

typedef hashtable_t *HashTable;

extern HashTable ht_create(const unsigned int);
extern void ht_destroy(HashTable);
extern void ht_insert(HashTable *const, const String, const String);
extern void ht_remove(const HashTable, const String);
extern String ht_get_value(HashTable const, const String);
extern void ht_print(HashTable const);

#endif /* End HASHTABLE_H */
