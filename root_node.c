
#include <stdlib.h>
#include "matrix/matrix.h"
#include "linkedlist.h"
#include "hashtable.h"
#include "node.h"
#include "camera_node.h"

typedef struct root_node
{
    node_t node;
    list_t *list_cameras;
    node_t *main_camera;
} root_node_t;

node_t *RootNodeNew()
{
    root_node_t *node = (root_node_t *) NodeNew(
            sizeof(root_node_t),
            "RootNode");

    node->list_cameras = ListNew(NULL, ListCmpPtr);
    node->main_camera = NULL;

    return (node_t *) node;
}

int RootNodeAddCamera(root_node_t *root, node_t *camera)
{
    ListAppendPointer(root->list_cameras, (void *) camera);
    return 1;
}

int RootNodeRemoveCamera(root_node_t *root, node_t *camera)
{
    list_data_t data;
    data.data_ptr = (void *) camera;
    ListRemoveValue(root->list_cameras, data, 0);
    return 1;
}

int RootNodeRender(root_node_t *root)
{
    list_iterator_t it;
    for(it = ListIterator(root->list_cameras);
        !ListIsEnd(&it);
        ListNext(&it))
    {
        camera_node_t *cam = (camera_node_t *) ListIteratorGet(&it).data_ptr;
        CameraNodeClear(cam);
    }
    return 1;
}

