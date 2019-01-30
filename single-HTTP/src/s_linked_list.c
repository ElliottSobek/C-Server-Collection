#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "s_linked_list.h"

// var == NULL; EQ to: !var

#define NT_LEN 1
#define STR_MAX 2048
#define COL_MAX 32

static S_Ll_Node create_node(const Generic value, const DataType dt) {
	S_Ll_Node node = (S_Ll_Node) malloc(sizeof(s_ll_node_t));

	if (!node) {
		fprintf(stderr, "Memory Error: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	if (!value) {
		exit(EXIT_FAILURE);
	}

	if (dt == STRING_ARRAY) {
		String *new_value = (String*) value;
		unsigned short value_len = 0;

		// for (String *i = (String*) value; *i != 0; i++) {
		// 	if (COL_MAX < value_len)
		// 		break;
		// 	value_len++;
		// }

		for (unsigned short i = 0; i < COL_MAX; i++) {
			if (new_value[i] == NULL) {
				puts("BAD");
				break;
			}
			value_len++;
		}

		printf("This is value: %d\n", value_len);

		String *const node_value = (String*) malloc(value_len * sizeof(String));

		if (!node_value) {
			fprintf(stderr, "Memory Error: %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}

		for (unsigned int i = 0; i < value_len; i++) {
			size_t value_elem_len = strnlen(new_value[i], STR_MAX);
			node_value[i] = (String) calloc(value_elem_len + NT_LEN, sizeof(char));

			if (!node_value[i]) {
				fprintf(stderr, "Memory Error: %s\n", strerror(errno));
				exit(EXIT_FAILURE);
			}
			strncpy(node_value[i], new_value[i], value_elem_len + NT_LEN);
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

// static S_Ll_Node find_prev_node(S_Ll_Node const list, const Generic value) {
// 	const size_t regex_len = strnlen(regex, STR_MAX);
// 	S_Ll_Node restrict prev = NULL;
// 	S_Ll_Node cur = list;

// 	while (cur) {
// 		if (strncmp(regex, cur->regex, regex_len) == 0)
// 			return prev;
// 		prev = cur;
// 		cur = cur->next;
// 	}

// 	return NULL;
// }

// S_Ll_Node s_ll_find(const S_Ll restrict list, const Generic value) {
// 	const size_t regex_len = strnlen(regex, STR_MAX);
// 	S_Ll_Node cur = list->root;

// 	while (cur) {
// 		if (strncmp(regex, cur->regex, regex_len) == 0)
// 			return cur;
// 		cur = cur->next;
// 	}

// 	return NULL;
// }

void s_ll_insert(const S_Ll restrict list, const Generic value, const DataType dt) {
	const S_Ll_Node new_node = create_node(value, dt);
	S_Ll_Node node = list->root;

	if (!node) {
		list->root = new_node;
		return;
	}

	while (node->next)
		node = node->next;

	node->next = new_node;
}

// int s_ll_remove(const S_Ll restrict list, const Generic value) {
// 	if (!s_ll_find(list, regex))
// 		return -1;

// 	const S_Ll_Node root = list->root, prev = find_prev_node(root, regex);
// 	S_Ll_Node restrict delete_node;

// 	if (!prev) {
// 		delete_node = root;
// 		list->root = root->next;
// 	} else if (!prev->next)
// 		delete_node = prev;
// 	else {
// 		delete_node = prev->next;
// 		prev->next = delete_node->next;
// 	}

// 	free(delete_node->value);
// 	delete_node->value = NULL;

// 	free(delete_node);
// 	delete_node = NULL;

// 	return 0;
// }

void s_ll_destroy(S_Ll restrict list) {
	S_Ll_Node restrict tmp;
	S_Ll_Node root = list->root;

	while (root) {
		tmp = root;
		root = root->next;

		if (tmp->dt == STRING_ARRAY) {
			for (unsigned int i = 0; i < (sizeof((String*) tmp->value) / sizeof(String)); i ++) {
				free(((String*) tmp->value)[i]);
				((String*) tmp->value)[i] = NULL;
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

void s_ll_print(const S_Ll restrict list) {
	printf("[");

	for (S_Ll_Node node = list->root; node; node = node->next) {
		if (node->dt == STRING_ARRAY) {
			size_t value_len = (sizeof((String*) node->value) / sizeof(String));

			printf("(");
			for (unsigned int i = 0; i < (value_len - 1); i++)
				printf("%s, ", ((String*) node->value)[value_len]);
			printf("%s)", ((String*) node->value)[value_len - 1]);
		}
	}
	puts("]");
}

S_Ll s_ll_create(void) {
	const S_Ll restrict list = (S_Ll) malloc(sizeof(S_Ll));
	list->root = NULL;

	return list;
}
