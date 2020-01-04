
#include <stdlib.h>
#include <glad/glad.h>
#include "matrix/matrix.h"
#include "linkedlist.h"
#include "hashtable.h"
#include "node.h"
#include "root_node.h"

typedef struct camera_node
{
    node_t node;
    vector_t background_color;
} camera_node_t;

int CameraNodeOnAdd(node_t *self)
{
    root_node_t *root = (root_node_t *) NodeFindRoot(self);
    RootNodeAddCamera(root, self);
    return 1;
}

int CameraNodeOnRemove(node_t *self)
{
    root_node_t *root = (root_node_t *) NodeFindRoot(self);
    RootNodeRemoveCamera(root, self);
    return 1;
}

node_t *CameraNodeNew()
{
    camera_node_t *node = (camera_node_t *) NodeNew(
            sizeof(camera_node_t),
            "Camera");
    node->node.OnAdd = CameraNodeOnAdd;
    node->node.OnRemove = CameraNodeOnRemove;

    node->background_color.x = 0;
    node->background_color.y = 0;
    node->background_color.z = 0;
    node->background_color.w = 0;

    return (node_t *) node;
}

void CameraNodeClear(camera_node_t *node)
{
    glClearColor(
            node->background_color.x,
            node->background_color.y,
            node->background_color.z,
            node->background_color.w);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

