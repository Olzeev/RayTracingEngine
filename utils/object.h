#include <LiteMath.h>
#include "bvh.h"

using LiteMath::float3;

struct Object {
    float3 pos;
    BVH_Node *bvh;
    std::vector <Triangle> tr;
};