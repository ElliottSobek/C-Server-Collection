#ifndef HASHTABLE_H
#define HASHTABLE_H

#include "types.h"

typedef struct ll_node_s {
	String key;
	Generic value;
	DataType dt;
	struct ll_node_s *next;
} ll_node_t;

typedef ll_node_t *Ll_Node;

typedef struct {
	Ll_Node root;
} ll_t;

typedef ll_t *Ll;

typedef struct {
	Ll *bins;
	unsigned int max_size, cur_size;
} hashtable_t;

typedef hashtable_t *HashTable;

extern HashTable ht_create(const unsigned int);
extern void ht_destroy(HashTable);
extern void ht_insert(HashTable *const, const String, const Generic, const DataType);
extern void ht_remove(const HashTable, const String);
extern Generic ht_get_value(HashTable const, const String);
extern void ht_print(HashTable const);

#endif /* End HASHTABLE_H */
