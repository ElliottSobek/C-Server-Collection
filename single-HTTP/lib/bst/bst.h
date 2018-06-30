#ifndef BST_H
#define BST_H

#include "../types/types.h"

typedef struct bst_node_s {
	String regex, path;
	struct bst_node_s *left, *right;
} bst_node_t;

typedef bst_node_t *Bst_Node;

typedef struct bst_s {
	Bst_Node root;
} bst_t;

typedef bst_t *Bst;

extern Bst bst_create(void);
extern void bst_destroy(Bst);
extern void bst_insert(Bst, const String const, const String const);
extern String bst_get_value(Bst_Node const, const String const);
extern void bst_print(Bst);

#endif /* End BST_H */
