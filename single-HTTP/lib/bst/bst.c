#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bst.h"

// var == NULL; EQ to: !var

#define NT_LEN 1
#define STR_MAX 2048

static Bst_Node create_node(const String const regex, const String const path) {
	Bst_Node const node = (Bst_Node) malloc(sizeof(bst_node_t));

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

	node->left = NULL;
	node->right = NULL;

	return node;
}

static void bst_destroy_tree(Bst_Node root) {
	if (!root)
		return;

	bst_destroy_tree(root->left);
	bst_destroy_tree(root->right);
	free(root->regex);
	root->regex = NULL;

	free(root->path);
	root->path = NULL;

	free(root);
	root = NULL;
}

static void bst_print_in_order(Bst_Node const root) {
	if (!root)
		return;
	bst_print_in_order(root->left);
	printf("%s->%s\n", root->regex, root->path);
	bst_print_in_order(root->right);
}

void bst_print(Bst bst) {
	bst_print_in_order(bst->root);
}

void bst_insert(Bst bst, const String const regex, const String const path) {
	Bst_Node const new_node = create_node(regex, path);

	if (!bst->root) {
		bst->root = new_node;
		return;
	}
	int cmp_res;
	Bst_Node cur = bst->root, parent;

	while (cur) {
		cmp_res = strncmp(path, cur->path, strnlen(path, STR_MAX));
		parent = cur;

		if (cmp_res < 0)
			cur = cur->left;
		else
			cur = cur->right;
	}

	if (cmp_res < 0)
		parent->left = new_node;
	else
		parent->right = new_node;
}

String bst_get_value(Bst bst, const String const key) {
	if (!bst->root) {
		return "";
	}
	int cmp_res;
	Bst_Node cur = bst->root;

	while (cur) {
		cmp_res = strncmp(key, cur->regex, strnlen(key, STR_MAX));

		if (cmp_res == 0)
			return cur->path;
		else if (cmp_res < 0)
			cur = cur->left;
		else
			cur = cur->right;
	}

	return "";
}

void bst_destroy(Bst bst) {
	bst_destroy_tree(bst->root);
	free(bst);
	bst = NULL;
}

Bst bst_create(void) {
	Bst bst = (Bst) malloc(sizeof(bst_t));

	if (!bst)
		exit(EXIT_FAILURE);
	bst->root = NULL;

	return bst;
}
