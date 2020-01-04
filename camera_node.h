
typedef struct camera_node camera_node_t;


node_t *CameraNodeNew();
void CameraNodeSetBackgroundColor(
        camera_node_t *node,
        vector_t *background_color);

void CameraNodeClear(camera_node_t *node);
