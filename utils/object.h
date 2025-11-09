#include "LiteMath.h"

using LiteMath::float3;

struct Model {
    BVH_Node *bvh;
    std::vector <Triangle> tr;
    float3 size=float3(1.0f);
};

struct Object {
    float3 pos;
    Model *model;
};