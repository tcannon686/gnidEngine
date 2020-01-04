
typedef struct root_node root_node_t;

node_t *RootNodeNew();

int RootNodeAddCamera(root_node_t *root, node_t *camera);
int RootNodeRemoveCamera(root_node_t *root, node_t *camera);
node_t *RootNodeGetMainCamera(root_node_t *root);

int RootNodeRender(root_node_t *root);

