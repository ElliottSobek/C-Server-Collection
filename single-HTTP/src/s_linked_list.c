#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "s_linked_list.h"

#define NT_LEN 1
#define STR_MAX 2048
#define COL_MAX 32

static S_Ll_Node create_node(const Generic value, const DataType dt) {
	S_Ll_Node node = (S_Ll_Node) malloc(sizeof(s_ll_node_t));

	if (!node)
		return NULL;

	if (dt == STRING_ARRAY) {
		String *typed_value = (String*) value;
		unsigned short value_len = 0;

		for (String *i = typed_value; *i; i++) {
			if (COL_MAX < value_len)
				break;
			value_len++;
		}
		String *node_value = (String*) malloc((value_len + NT_LEN) * sizeof(String));

		if (!node_value) {
			free(node);
			node = NULL;

			return NULL;
		}

		for (unsigned short i = 0; i < value_len + NT_LEN; i++)
			node_value[i] = NULL;

		for (unsigned int i = 0; i < value_len; i++) {
			size_t value_elem_len = strnlen(typed_value[i], STR_MAX);
			node_value[i] = (String) calloc(value_elem_len + NT_LEN, sizeof(char));

			if (!node_value[i]) {
				for (unsigned int j = 0; j < i; j++) {
					free(node_value[j]);
					node_value[j] = NULL;
				}
				free(node_value);
				node_value = NULL;

				free(node);
				node = NULL;

				return NULL;
			}
			strncpy(node_value[i], typed_value[i], value_elem_len + NT_LEN);
		}
		node->value = node_value;
	} else {
		free(node);
		node = NULL;

		return NULL;
	}
	node->dt = dt;
	node->next = NULL;

	return node;
}

static S_Ll_Node find_prev_node(S_Ll_Node const list, const Generic value, const DataType dt) {
	S_Ll_Node prev = NULL, cur = list;

	while (cur) {
		if (dt == STRING_ARRAY) {
			bool match = false;

			for (String *i = (String*) cur->value, *j = (String*) value; *i && *j; i++, j++) {
				if (strncmp(*i, *j, strnlen(*i, STR_MAX)) != 0) {
					match = false;

					break;
				}
				match = true;
			}

			if (match)
				return prev;
		}
		prev = cur;
		cur = cur->next;
	}

	return NULL;
}

S_Ll_Node s_ll_find(const S_Ll list, const Generic value, const DataType dt) {
	S_Ll_Node cur = list->root;

	while (cur) {
		if (dt == STRING_ARRAY) {
			bool match = false;

			for (String *i = (String*) cur->value, *j = (String*) value; *i && *j; i++, j++) {
				if (strncmp(*i, *j, strnlen(*i, STR_MAX)) != 0) {
					match = false;

					break;
				}
				match = true;
			}

			if (match)
				return cur;
		}
		cur = cur->next;
	}

	return NULL;
}

int s_ll_insert(const S_Ll list, const Generic value, const DataType dt) {
	const S_Ll_Node new_node = create_node(value, dt);

	if (!new_node)
		return -1;
	S_Ll_Node node = list->root;

	if (!node) {
		list->root = new_node;
		return 0;
	}

	while (node->next)
		node = node->next;
	node->next = new_node;

	return 0;
}

int s_ll_remove(const S_Ll list, const Generic value, const DataType dt) {
	if (!s_ll_find(list, value, dt))
		return -1;

	const S_Ll_Node root = list->root, prev = find_prev_node(root, value, dt);
	S_Ll_Node delete_node;

	if (!prev) {
		delete_node = root;
		list->root = root->next;
	} else if (!prev->next)
		delete_node = prev;
	else {
		delete_node = prev->next;
		prev->next = delete_node->next;
	}

	free(delete_node->value);
	delete_node->value = NULL;

	free(delete_node);
	delete_node = NULL;

	return 0;
}

void s_ll_destroy(S_Ll list) {
	S_Ll_Node tmp;
	S_Ll_Node root = list->root;

	while (root) {
		tmp = root;
		root = root->next;

		if (tmp->dt == STRING_ARRAY) {
			for (String *i = (String*) tmp->value; *i; i++) {
				free(*i);
				*i = NULL;
			}
		}
		free(tmp->value);
		tmp->value = NULL;

		free(tmp);
		tmp = NULL;
	}
	free(list);
	list = NULL;
}

void s_ll_print(const S_Ll list) {
	bool end = false;

	printf("[");

	for (S_Ll_Node node = list->root; node; node = node->next) {
		if (!node->next)
			end = true;

		if (node->dt == STRING_ARRAY) {
			String tmp_carray;

			printf("(");
			for (String *i = (String*) node->value; *i; i++) {
				printf("%s, ", *i);
				tmp_carray = *i;
			}
			if (end)
				printf("%s)", tmp_carray);
			else
				printf("%s), ", tmp_carray);
		}
	}
	puts("]");
}

S_Ll s_ll_create(void) {
	const S_Ll list = (S_Ll) malloc(sizeof(S_Ll));

	if (!list)
		return NULL;
	list->root = NULL;

	return list;
}
