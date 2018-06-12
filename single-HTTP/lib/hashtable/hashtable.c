#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hashtable.h"

// var == NULL; EQ to: !var

#define NT_LEN 1
#define HT_DELTA 2
#define STR_MAX 2048
#define DEFAULT_SIZE 10
#define PERCENT_CUTOFF 80

static Ht_Node create_node(const String const key, const String const value) {
	const size_t key_len = strnlen(key, STR_MAX), value_len = strnlen(value, STR_MAX);

	Ht_Node node = (Ht_Node) malloc(sizeof(ht_node_t));
	if (!node)
		return NULL;

	node->key = (String) calloc(key_len + NT_LEN, sizeof(char));
	if (!node->key)
		return NULL;
	strncpy(node->key, key, key_len);

	node->value = (String) calloc(value_len + NT_LEN, sizeof(char));
	if (!node->value)
		return NULL;
	strncpy(node->value, value, value_len);

	node->next = NULL;

	return node;
}

// D. J. Bernstein Hash, Modified
static unsigned int get_hash(HashTable const ht, String value) {
	unsigned int result = 5381;

	while (*value)
		result = (33 * result) ^ (unsigned char) *value++;

	return result % ht->max_size;
}

static void destroy_list(Ht_Node list) {
	Ht_Node tmp;

	while (list) {
		tmp = list;
		list = list->next;

		free(tmp->key);
		tmp->key = NULL;

		free(tmp->value);
		tmp->value = NULL;

		free(tmp);
		tmp = NULL;
	}
}

static void add_collision_node(Ht_Node const list, Ht_Node const new_node) {
	Ht_Node node = list;

	while (node->next)
		node = node->next;

	node->next = new_node;
}

static void deep_copy(HashTable new_table, HashTable const ht) {

	for (unsigned int i = 0; i < ht->max_size; i++) {
		if (!ht->data[i])
			continue;
		for (ht_node_t *node = ht->data[i]; node; node = node->next)
			ht_insert_set(&new_table, node->key, node->value);
	}
}

static void print_list(Ht_Node const list) {
	for (Ht_Node node = list->next; node; node = node->next)
		printf("->%s:%s", node->key, node->value);
	printf("\n");
}

HashTable ht_create(const unsigned int max_size) {
	if (1 > max_size)
		return NULL;

	HashTable ht = (HashTable) malloc(sizeof(hashtable_t));
	if (!ht)
		exit(EXIT_FAILURE);

	ht->data = (Ht_Node*) malloc(sizeof(Ht_Node) * DEFAULT_SIZE);
	if (!ht->data)
		exit(EXIT_FAILURE);

	for (unsigned int i = 0; i < DEFAULT_SIZE; i++)
		ht->data[i] = NULL;

	ht->max_size = DEFAULT_SIZE;
	ht->cur_size = 0;

	return ht;
}

void ht_destroy(HashTable ht) {
	for (unsigned int i = 0; i < ht->max_size; i++) {
		if (!ht->data[i])
			continue;
		else if (ht->data[i]->next)
			destroy_list(ht->data[i]);
		else {
			free(ht->data[i]->key);
			ht->data[i]->key = NULL;

			free(ht->data[i]->value);
			ht->data[i]->value = NULL;

			free(ht->data[i]);
			ht->data[i] = NULL;
		}
	}
	free(ht->data);
	ht->data = NULL;

	free(ht);
	ht = NULL;
}

void ht_insert_set(HashTable *const ht_head, const String const key, const String const value) {
	HashTable const ht = *ht_head;

	if (ht->cur_size >= ((ht->max_size * PERCENT_CUTOFF) / 100)) {
		HashTable new_table = ht_create(ht->max_size * HT_DELTA);

		deep_copy(new_table, ht);
		ht_insert_set(&new_table, key, value);
		ht_destroy(ht);
		*ht_head = new_table;
		return;
	}

	Ht_Node const node = create_node(key, value);
	const unsigned int bin = get_hash(ht, key);

	if (!ht->data[bin])
		ht->data[bin] = node;
	else
		add_collision_node(ht->data[bin], node);

	ht->cur_size++;
}

String ht_get_value(HashTable const ht, const String const key) {
	const unsigned int bin = get_hash(ht, key);
	int cmp_res;

	for (Ht_Node node = ht->data[bin]; node; node = node->next) {
		cmp_res = strncmp(key, node->key, strnlen(key, STR_MAX));

		if (0 == cmp_res)
			return node->value;
	}

	return NULL;
}

void ht_print(HashTable const ht) {
	if (!ht->data[0])
		printf("nil\n");
	else {
		printf("%s:%s", ht->data[0]->key, ht->data[0]->value);
		print_list(ht->data[0]);
	}

	for (unsigned int i = 1; i < ht->max_size; i++) {

		if (!ht->data[i])
			printf("|\nv\nnil\n");
		else {
			printf("|\nv\n%s:%s", ht->data[i]->key, ht->data[i]->value);
			print_list(ht->data[i]);
		}
	}
}
