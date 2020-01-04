
#ifndef NODE_H
#define NODE_H
typedef struct node node_t;

typedef struct node
{
    /* The total size in bytes of the node. */
    int size;
    /* The name of the node. Must be unique to parent node. */
    char *name;
    
    node_t *parent;
    hashtable_t *children;
    int references;

    int enabled;
    
    /* Should be called every time a node is cloned or created. */
    int(*OnCreate)(node_t *self);
    int(*Update)(node_t *self);
    int(*FixedUpdate)(node_t *self);
    /* Called after the node is added to its parent. */
    int(*OnAdd)(node_t *self);
    /* Called after a child node is added. */
    int(*OnAddChild)(node_t *self);
    /* Called before the node is removed from its parent.  */
    int(*OnRemove)(node_t *self);
    /* Called before a node is removed from the node. */
    int(*OnRemoveChild)(node_t *self);
    /* Should be called just before the node is destroyed. */
    int(*OnDestroy)(node_t *self);
    /* Should be called just before the node is freed. */
    int(*OnFree)(node_t *self);
} node_t;

int NodeCopy(node_t *destination, node_t *source);
node_t *NodeClone(node_t *source);

int NodeNextChild(node_t *parent, node_t **child);
int NodeAdd(node_t *parent, node_t *child);
/* Remove a child node from the parent. */
int NodeRemove(node_t *parent, node_t *child);
/* Free the given node. */
int NodeFree(node_t *node);
/* Destroy the node and its children, freeing them.  It is unsafe to continue
 * processing the node after this function is called. */
int NodeDestroy(node_t *node);

int NodeInit(node_t *destination, int size, char *name);
/* Creates a new empty node. */
node_t *NodeNew(int size, char *name);

int NodeUpdateAll(node_t *node);
int NodeFixedUpdateAll(node_t *node);

node_t *NodeFind(node_t *node, char *name);
node_t *NodeFindInChildren(node_t *node, char *name);
node_t *NodeFindRoot(node_t *node);

#define NodeCall(node, fun) node->fun != NULL ? node->fun(node) : 0

#endif
