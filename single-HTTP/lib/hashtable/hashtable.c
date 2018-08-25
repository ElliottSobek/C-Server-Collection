#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "hashtable.h"

// var == NULL; EQ to: !var

#define NT_LEN 1
#define HT_DELTA 2
#define STR_MAX 2048
#define DEFAULT_SIZE 10
#define PERCENT_CUTOFF 80

static Ll_Node create_node(const String restrict key, const String restrict value) {
	const Ll_Node restrict node = (Ll_Node) malloc(sizeof(ll_node_t));
	if (!node)
		exit(EXIT_FAILURE);

	const size_t key_len = strnlen(key, STR_MAX);

	node->key = (String) calloc((key_len + NT_LEN), sizeof(char));
	if (!node->key)
		exit(EXIT_FAILURE);

	strncpy(node->key, key, key_len);

	const size_t value_len = strnlen(value, STR_MAX);

	node->value = (String) calloc((value_len + NT_LEN), sizeof(char));
	if (!node->value)
		exit(EXIT_FAILURE);

	strncpy(node->value, value, value_len);

	node->next = NULL;

	return node;
}

static Ll_Node find_prev_node(Ll_Node const list, const String restrict key) {
	const size_t key_len = strnlen(key, STR_MAX);
	Ll_Node restrict prev = NULL;
	Ll_Node cur = list;

	while (cur) {
		if (strncmp(key, cur->key, key_len) == 0)
			return prev;
		prev = cur;
		cur = cur->next;
	}

	return NULL;
}

static Ll_Node s_ll_find(const Ll restrict list, const String restrict key) {
	const size_t key_len = strnlen(key, STR_MAX);
	Ll_Node cur = list->root;

	while (cur) {
		if (strncmp(key, cur->key, key_len) == 0)
			return cur;
		cur = cur->next;
	}

	return NULL;
}

static void s_ll_insert(const Ll restrict list, const String restrict key, const String restrict value) {
	const Ll_Node new_node = create_node(key, value);
	Ll_Node node = list->root;

	if (!node) {
		list->root = new_node;
		return;
	}

	while (node->next)
		node = node->next;

	node->next = new_node;
}

static int s_ll_remove(const Ll restrict list, const String restrict key) {
	if (!s_ll_find(list, key))
		return -1;

	const Ll_Node root = list->root, prev = find_prev_node(root, key);
	Ll_Node restrict delete_node;

	if (!prev) {
		delete_node = root;
		list->root = root->next;
	} else if (!prev->next)
		delete_node = prev;
	else {
		delete_node = prev->next;
		prev->next = delete_node->next;
	}

	free(delete_node->key);
	delete_node->key = NULL;

	free(delete_node->value);
	delete_node->value = NULL;

	free(delete_node);
	delete_node = NULL;

	return 0;
}

static void s_ll_destroy(Ll restrict list) {
	Ll_Node tmp, root = list->root;

	while (root) {
		tmp = root;
		root = root->next;

		free(tmp->key);
		tmp->key = NULL;

		free(tmp->value);
		tmp->value = NULL;

		free(tmp);
		tmp = NULL;
	}
	free(list);
	list = NULL;
}

static void s_ll_print(const Ll restrict list) {
	for (Ll_Node node = list->root; node; node = node->next)
		printf("%s:%s\n", node->key, node->value);
}

static Ll s_ll_create(void) {
	const Ll restrict list = (Ll) malloc(sizeof(Ll));
	list->root = NULL;

	return list;
}

// HASHTABLE IMPLEMENTATION

// D. J. Bernstein Hash, Modified
static unsigned int get_hash(const HashTable restrict ht, String restrict value) {
	unsigned int result = 5381;

	while (*value)
		result = (33 * result) ^ (unsigned char) *value++;

	return result % ht->max_size;
}

static void deep_copy(const HashTable restrict new_table, const HashTable restrict ht) {
	unsigned int bin;

	for (unsigned int i = 0; i < ht->max_size; i++) {
		if (!ht->bins[i])
			continue;
		for (Ll_Node node = ht->bins[i]->root; node; node = node->next) {
			bin = get_hash(new_table, node->key);

			s_ll_insert(new_table->bins[bin], node->key, node->value);
			new_table->cur_size++;
		}
	}
}

void ht_remove(const HashTable restrict ht, const String restrict key) {
	const unsigned int bin = get_hash(ht, key);

	if (s_ll_remove(ht->bins[bin], key) == 0)
		ht->cur_size--;
}

String ht_get_value(const HashTable restrict ht, const String restrict key) {
	const unsigned int bin = get_hash(ht, key);

	for (Ll_Node node = ht->bins[bin]->root; node; node = node->next)
		if (strncmp(key, node->key, strnlen(key, STR_MAX)) == 0)
			return node->value;

	return NULL;
}

void ht_print(const HashTable restrict ht) {
	for (unsigned int bin = 0; bin < ht->max_size; bin++)
		s_ll_print(ht->bins[bin]);
}

void ht_destroy(HashTable restrict ht) {
	for (unsigned int bin = 0; bin < ht->max_size; bin++)
		s_ll_destroy(ht->bins[bin]);

	free(ht->bins);
	ht->bins = NULL;

	free(ht);
	ht = NULL;
}

HashTable ht_create(const unsigned int max_size) {
	if (max_size < 1)
		return NULL;

	const HashTable ht = (HashTable) malloc(sizeof(hashtable_t));
	if (!ht)
		exit(EXIT_FAILURE);

	ht->bins = (Ll*) malloc(sizeof(Ll) * max_size);
	if (!ht->bins)
		exit(EXIT_FAILURE);

	for (unsigned int i = 0; i < max_size; i++)
		ht->bins[i] = s_ll_create();

	ht->max_size = max_size;
	ht->cur_size = 0;

	return ht;
}

void ht_insert(HashTable *ht_head, const String restrict key, const String restrict value) {
	const HashTable ht = *ht_head;
	const unsigned int bin = get_hash(ht, key);

	s_ll_insert(ht->bins[bin], key, value);
	ht->cur_size++;

	if (ht->cur_size >= ((ht->max_size * PERCENT_CUTOFF) / 100)) {
		HashTable new_table = ht_create(ht->max_size * HT_DELTA);

		deep_copy(new_table, ht);
		ht_destroy(ht);
		*ht_head = new_table;
	}
}
