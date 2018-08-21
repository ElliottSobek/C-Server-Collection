#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "s_linked_list.h"

// var == NULL; EQ to: !var

#define NT_LEN 1
#define STR_MAX 2048

static S_Ll_Node create_node(const String restrict regex, const String restrict path) {
	const S_Ll_Node restrict node = (S_Ll_Node) malloc(sizeof(s_ll_node_t));
	if (!node)
		exit(EXIT_FAILURE);

	const size_t regex_len = strnlen(regex, STR_MAX);

	node->regex = (String) calloc((regex_len + NT_LEN), sizeof(char));
	if (!node->regex)
		exit(EXIT_FAILURE);

	strncpy(node->regex, regex, regex_len);

	const size_t path_len = strnlen(path, STR_MAX);

	node->path = (String) calloc((path_len + NT_LEN), sizeof(char));
	if (!node->path)
		exit(EXIT_FAILURE);

	strncpy(node->path, path, path_len);

	node->next = NULL;

	return node;
}

static S_Ll_Node find_prev_node(S_Ll_Node const list, const String restrict regex) {
	const size_t regex_len = strnlen(regex, STR_MAX);
	S_Ll_Node restrict prev = NULL;
	S_Ll_Node cur = list;

	while (cur) {
		if (strncmp(regex, cur->regex, regex_len) == 0)
			return prev;
		prev = cur;
		cur = cur->next;
	}

	return NULL;
}

S_Ll_Node s_ll_find(const S_Ll restrict list, const String restrict regex) {
	const size_t regex_len = strnlen(regex, STR_MAX);
	S_Ll_Node cur = list->root;

	while (cur) {
		if (strncmp(regex, cur->regex, regex_len) == 0)
			return cur;
		cur = cur->next;
	}

	return NULL;
}

void s_ll_insert(const S_Ll restrict list, const String restrict regex, const String restrict path) {
	const S_Ll_Node new_node = create_node(regex, path);
	S_Ll_Node node = list->root;

	if (!node) {
		list->root = new_node;
		return;
	}

	while (node->next)
		node = node->next;

	node->next = new_node;
}

int s_ll_remove(const S_Ll restrict list, const String restrict regex) {
	if (!s_ll_find(list, regex))
		return -1;

	const S_Ll_Node root = list->root, prev = find_prev_node(root, regex);
	S_Ll_Node restrict delete_node;

	if (!prev) {
		delete_node = root;
		list->root = root->next;
	} else if (!prev->next)
		delete_node = prev;
	else {
		delete_node = prev->next;
		prev->next = delete_node->next;
	}

	free(delete_node->regex);
	delete_node->regex = NULL;

	free(delete_node->path);
	delete_node->path = NULL;

	free(delete_node);
	delete_node = NULL;

	return 0;
}

void s_ll_destroy(S_Ll restrict list) {
	S_Ll_Node restrict tmp;
	S_Ll_Node root = list->root;

	while (root) {
		tmp = root;
		root = root->next;

		free(tmp->regex);
		tmp->regex = NULL;

		free(tmp->path);
		tmp->path = NULL;

		free(tmp);
		tmp = NULL;
	}
	free(list);
	list = NULL;
}

void s_ll_print(const S_Ll restrict list) {
	for (S_Ll_Node node = list->root; node; node = node->next)
		printf("%s:%s\n", node->regex, node->path);
}

S_Ll s_ll_create(void) {
	const S_Ll restrict list = (S_Ll) malloc(sizeof(S_Ll));
	list->root = NULL;

	return list;
}
