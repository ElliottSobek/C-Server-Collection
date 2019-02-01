#ifndef S_LINKED_LIST_H
#define S_LINKED_LIST_H

#include "types.h"

typedef struct s_ll_node_s {
	Generic value;
	DataType dt;
	struct s_ll_node_s *next;
} s_ll_node_t;

typedef s_ll_node_t *S_Ll_Node;

typedef struct {
	S_Ll_Node root;
} s_ll_t;

typedef s_ll_t *S_Ll;

S_Ll s_ll_create(void);
void s_ll_destroy(S_Ll);
int s_ll_insert(S_Ll, const Generic, const DataType);
S_Ll_Node s_ll_find(S_Ll const, const Generic, const DataType);
int s_ll_remove(const S_Ll, const Generic, const DataType);
void s_ll_print(S_Ll);

#endif /* End S_LINKED_LIST_H */
