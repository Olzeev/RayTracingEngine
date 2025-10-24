#include "objects.h"

struct Scene {
    Box *boxes;
    int boxes_size;

    Sphere *spheres;
    int spheres_size;

    Plane *planes;
    int planes_size;

    Mundelbulb *fractals;
    int fractals_size;

    LightSource *light_sources;
    int light_sources_count;
};


struct Camera {
    float3 pos;
    float3 dir;
};