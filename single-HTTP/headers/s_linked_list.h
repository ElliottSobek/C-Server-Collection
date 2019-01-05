#ifndef S_LINKED_LIST_H
#define S_LINKED_LIST_H

#include "types.h"

typedef struct s_ll_node_s {
	String regex, path;
	struct s_ll_node_s *next;
} s_ll_node_t;

typedef s_ll_node_t *S_Ll_Node;

typedef struct s_ll_s {
	S_Ll_Node root;
} s_ll_t;

typedef s_ll_t *S_Ll;

extern S_Ll s_ll_create(void);
extern void s_ll_destroy(S_Ll);
extern void s_ll_insert(S_Ll, const String, const String);
extern S_Ll_Node s_ll_find(S_Ll const, const String);
extern void s_ll_print(S_Ll);

#endif /* End S_LINKED_LIST_H */
