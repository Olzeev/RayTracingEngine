#include "objects.h"

struct Scene {
    Box *boxes;
    int boxes_size;

    Sphere *spheres;
    int spheres_size;

    Plane *planes;
    int planes_size;

    LightSource *light_sources;
    int light_sources_count;
};