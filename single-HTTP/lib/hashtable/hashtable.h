#ifndef HASHTABLE_H
#define HASHTABLE_H

#include "../types/types.h"

typedef struct node {
	String key, value;
	struct node *next;
} node_t;

typedef node_t *Node;

typedef struct hashtable_s {
	Node *data;
	unsigned int max_size, cur_size;
} hashtable_t;

typedef hashtable_t *HashTable;

extern HashTable create_ht(const unsigned int);
extern void destroy_table(HashTable);
extern void insert_set(HashTable *const, const String const, const String const);
extern String get_value(HashTable const, const String const);
extern void print_table(HashTable const);

#endif /* End HASHTABLE_H */
