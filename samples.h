#ifndef objects_h
#include "objects.h"
#endif
#ifndef scene_h
#include "scene.h"
#endif
#ifndef camera_h
#include "camera.h"
#endif
Box *boxes1 = new Box[3]{
    {float3(0, 0, 0), float3(1, 1, 1)},
    {float3(5, 2, 3), float3(2, 1, 1)},
    {float3(-5, -2, -3), float3(1, 1, 2)},
};
Sphere *spheres1 = new Sphere[0];
Plane *planes1 = new Plane[0];
Scene sample_scene1{boxes1, 3, spheres1, 0, planes1, 0};


Box *boxes2 = new Box[0];
Sphere *spheres2 = new Sphere[3]{
    {float3(0, 0, 0), 2},
    {float3(5, 2, 3), 1},
    {float3(-5, -2, -3), 4},
};
Plane *planes2 = new Plane[0];

Scene sample_scene2{boxes2, 0, spheres2, 3, planes2, 0};


Box *boxes3 = new Box[1]{{float3(0, 0, 0), float3(1, 1, 1)}};
Sphere *spheres3 = new Sphere[0];
Plane *planes3 = new Plane[1]{
    {-2, float3(0, 1, 0)}
};
Scene sample_scene3{boxes3, 1, spheres3, 0, planes3, 1};


Camera sample_camera{float3(0, 0, 0), float3(1, 0, 0)};