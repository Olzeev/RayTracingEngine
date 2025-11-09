#include "LiteMath.h"

using LiteMath::float3;

struct Model {
    BVH_Node *bvh;
    std::vector <Triangle> tr;
};

struct Object {
    float3 pos;
    Model *model;
    int reflect;
};