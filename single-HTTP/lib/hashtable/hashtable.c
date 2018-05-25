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

static Node create_node(const String const key, const String const value) {
	const size_t key_len = strnlen(key, STR_MAX), value_len = strnlen(value, STR_MAX);

	Node node = (Node) malloc(sizeof(node_t));
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

static void destroy_list(Node list) {
	Node tmp;

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

// static Node find_prev_node(Node const list, const String const entry) {
// 	const size_t entry_len = strnlen(entry, STR_MAX);
// 	Node cur = list, prev = NULL;

// 	for (Node node = list; node->next; node = node->next) {
// 		if (strncmp(entry, node->key, entry_len) == 0)
// 			return prev;
// 		prev = cur;
// 		cur = node->next;
// 	}

// 	return prev;
// }

// static void remove_collision_node(Node *const list, Node const prev) {
// 	Node delete_node;

// 	if (!prev) {
// 		delete_node = *list;
// 		*list = (*list)->next;
// 	} else if (!prev->next)
// 		delete_node = prev;
// 	else {
// 		delete_node = prev->next;
// 		prev->next = prev->next->next;
// 	}

// 	free(delete_node->key);
// 	delete_node->key = NULL;

// 	free(delete_node->value);
// 	delete_node->value = NULL;

// 	free(delete_node);
// 	delete_node = NULL;
// }

static void add_collision_node(Node const list, Node const new_node) {
	Node node = list;

	while (node->next)
		node = node->next;

	node->next = new_node;
}

static void deep_copy(HashTable new_table, HashTable const ht) {

	for (unsigned int i = 0; i < ht->max_size; i++) {
		if (!ht->data[i])
			continue;
		for (node_t *node = ht->data[i]; node; node = node->next)
			insert_set(&new_table, node->key, node->value);
	}
}

// static void remove_set(HashTable const ht, const String const key) {
// 	const unsigned int bin = get_hash(ht, key);

// 	if (ht->data[bin]->next)
// 		remove_collision_node(&ht->data[bin], find_prev_node(ht->data[bin], key));
// 	else {
// 		free(ht->data[bin]->key);
// 		ht->data[bin]->key = NULL;

// 		free(ht->data[bin]->value);
// 		ht->data[bin]->value = NULL;

// 		free(ht->data[bin]);
// 		ht->data[bin] = NULL;
// 	}
// 	ht->cur_size--;
// }

static void print_list(Node const list) {
	for (Node node = list->next; node; node = node->next)
		printf("->%s:%s", node->key, node->value);
	printf("\n");
}

HashTable create_ht(const unsigned int max_size) {
	if (1 > max_size)
		return NULL;

	HashTable ht = (HashTable) malloc(sizeof(hashtable_t));
	if (!ht)
		exit(EXIT_FAILURE);

	ht->data = (Node*) malloc(sizeof(Node) * DEFAULT_SIZE);
	if (!ht->data)
		exit(EXIT_FAILURE);

	for (unsigned int i = 0; i < DEFAULT_SIZE; i++)
		ht->data[i] = NULL;

	ht->max_size = DEFAULT_SIZE;
	ht->cur_size = 0;

	return ht;
}

void destroy_table(HashTable ht) {
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

void insert_set(HashTable *const ht_head, const String const key, const String const value) {
	HashTable const ht = *ht_head;

	if (ht->cur_size >= ((ht->max_size * PERCENT_CUTOFF) / 100)) {
		HashTable new_table = create_ht(ht->max_size * HT_DELTA);

		deep_copy(new_table, ht);
		insert_set(&new_table, key, value);
		destroy_table(ht);
		*ht_head = new_table;
		return;
	}

	Node const node = create_node(key, value);
	const unsigned int bin = get_hash(ht, key);

	if (!ht->data[bin])
		ht->data[bin] = node;
	else
		add_collision_node(ht->data[bin], node);

	ht->cur_size++;
}

String get_value(HashTable const ht, const String const key) {
	const unsigned int bin = get_hash(ht, key);
	int cmp_res;

	for (Node node = ht->data[bin]; node; node = node->next) {
		cmp_res = strncmp(key, node->key, strnlen(key, STR_MAX));

		if (0 == cmp_res)
			return node->value;
	}

	return NULL;
}

void print_table(HashTable const ht) {
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
