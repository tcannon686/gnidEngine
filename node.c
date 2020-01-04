
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include "hashtable.h"
#include "node.h"

int NodeCopy(node_t *destination, node_t *source)
{
    if(destination->size == source->size)
    {
        memcpy(
            (void *) destination,
            (void *) source,
            source->size);

        /* Copy each child. */
        destination->children = HashTableNewDefault();
        hashtable_entry_t *entry_ptr;
        while(HashTableNext(source->children, &entry_ptr))
        {
            node_t *child = NodeClone((node_t *) entry_ptr->value.pointer);
            NodeAdd(destination, child);
        }
        
        // Call OnCreate for the node.
        if(destination != NULL)
            NodeCall(destination, OnCreate);
        return 1;
    }
    return 0;
}

node_t *NodeClone(node_t *source)
{
    node_t *node = (node_t *) malloc(source->size);

    if(node == NULL)
        return NULL;
    
    if(NodeCopy(node, source))
        return node;
    
    return NULL;
}

int NodeNextChild(node_t *parent, node_t **child)
{
    hashtable_entry_t *entry_ptr;
    if(*child != NULL)
        entry_ptr = HashTableGet(parent->children, (*child)->name);
    else
        entry_ptr = NULL;

    if(HashTableNext(parent->children, &entry_ptr))
    {
        *child = (node_t *) entry_ptr->value.pointer;
        return 1;
    }
    *child = NULL;
    return 0;
}

int NodeAdd(node_t *parent, node_t *child)
{
    hashtable_entry_t *new_entry = HashTableGetOrInsert(
            parent->children,
            child->name);

    if(new_entry->value.pointer == NULL)
    {
        new_entry->value.pointer = (void *) child;
        child->parent = parent;

        child->references ++;
        parent->references ++;

        NodeCall(child, OnAdd);
        NodeCall(parent, OnAddChild);
        return 1;
    }
    
    return 0;
}

int NodeRemove(node_t *parent, node_t *child)
{
    hashtable_entry_t *entry_ptr = HashTableGet(
            parent->children,
            child->name);
    if(entry_ptr->value.pointer == (void *) child)
    {
        NodeCall(child, OnRemove);
        NodeCall(parent, OnRemoveChild);
        child->parent = NULL;
        child->references --;
        parent->references --;
        return HashTableRemove(parent->children, child->name);
    }
    return 0;
}

int NodeFree(node_t *node)
{
    assert(node->references == 0);
    NodeCall(node, OnFree);
    HashTableFree(node->children);
    free((void *) node);
    return 1;
}

int NodeDestroy(node_t *node)
{
    NodeCall(node, OnDestroy);

    if(node->parent != NULL)
        NodeRemove(node->parent, node);
    node_t *child = NULL;
    while(NodeNextChild(node, &child))
    {
        NodeDestroy(child);
        child = NULL;
    }
    return NodeFree(node);
}

int NodeInit(node_t *node, int size, char *name)
{
    node->size = size;
    node->name = name;
    node->parent = NULL;
    node->children = HashTableNewDefault();
    node->references = 0;
    node->OnCreate = NULL;
    node->OnAdd = NULL;
    node->OnAddChild = NULL;
    node->OnRemove = NULL;
    node->OnRemoveChild = NULL;
    node->Update = NULL;
    node->FixedUpdate = NULL;
    node->OnDestroy = NULL;
    node->OnFree = NULL;
    return 1;
}

node_t *NodeNew(int size, char *name)
{
    node_t *node = (node_t *) malloc(size);
    assert(node != NULL);
    NodeInit(node, size, name);
    return node;
}

node_t *NodeFind(node_t *node, char *name)
{
    assert(node != NULL);
    assert(name != NULL);
    hashtable_entry_t *entry_ptr = HashTableGet(node->children, name);
    if(entry_ptr == NULL)
        return NULL;
    return (node_t *) entry_ptr->value.pointer;
}

node_t *NodeFindInChildren(node_t *node, char *name)
{
    node_t *ret = NodeFind(node, name);
    if(ret != NULL)
        return ret;

    node_t *child;
    while(NodeNextChild(node, &child))
    {
        ret = NodeFindInChildren(child, name);
        if(ret != NULL)
            return ret;
    }
    return NULL;
}

node_t *NodeFindRoot(node_t *node)
{
    if(node->parent == NULL)
        return node;
    return NodeFindRoot(node->parent);
}

#define CREATE_ALL_FUNCTION(fun)\
int Node##fun##All(node_t *node)\
{\
    int result = 1;\
    node_t *child = NULL;\
    if(node->fun != NULL)\
        result = result && NodeCall(node, fun);\
    while(NodeNextChild(node, &child))\
    {\
        result = result && Node##fun##All(child);\
    }\
    return result;\
}

CREATE_ALL_FUNCTION(Update)
CREATE_ALL_FUNCTION(FixedUpdate)

