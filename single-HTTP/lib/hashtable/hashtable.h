#ifndef HASHTABLE_H
#define HASHTABLE_H

#include "../types/types.h"

typedef struct ht_node_s {
	String key, value;
	struct ht_node_s *next;
} ht_node_t;

typedef ht_node_t *Ht_Node;

typedef struct hashtable_s {
	Ht_Node *data;
	unsigned int max_size, cur_size;
} hashtable_t;

typedef hashtable_t *HashTable;

extern HashTable ht_create(const unsigned int);
extern void ht_destroy(HashTable);
extern void ht_insert_set(HashTable *const, const String const, const String const);
extern String ht_get_value(HashTable const, const String const);
extern void ht_print(HashTable const);

#endif /* End HASHTABLE_H */
