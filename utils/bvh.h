#include "LiteMath.h"
#include "mesh.h"
#include <vector>
#include <algorithm>
#include <limits>
#include <math.h>


#define BVH_MAX_TRIANGLES 15

struct Triangle {
    float3 vert[3];
    float3 center_pos;
    int ind;
};

struct AABB {
    float3 pos;
    float3 size;
};

struct BVH_Node {
    AABB box;
    BVH_Node *left, *right;
    char type; // 0 - node, 1 - leaf
    std::vector <Triangle> triangles;
};

#include "object.h"

struct TLBVH_Node {
    AABB box;
    TLBVH_Node *left, *right;
    char type; //0 - box, 1 - leaf
    std::vector<Object> obj;
};


#include "custom_utils.h"

AABB generate_aabb(std::vector <Triangle> tr) {
    float3 min_coord = float3(tr[0].vert[0].x, tr[0].vert[0].y, tr[0].vert[0].z);
    float3 max_coord = min_coord;
    int n = tr.size();
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < 3; ++j) {
            min_coord = float3(LiteMath::min(min_coord.x, tr[i].vert[j].x), LiteMath::min(min_coord.y, tr[i].vert[j].y), LiteMath::min(min_coord.z, tr[i].vert[j].z));
            max_coord = float3(LiteMath::max(max_coord.x, tr[i].vert[j].x), LiteMath::max(max_coord.y, tr[i].vert[j].y), LiteMath::max(max_coord.z, tr[i].vert[j].z));
        }
    }
    AABB res;
    res.pos = (min_coord + max_coord) / 2.0f;
    res.size = (max_coord - min_coord) / 2.0f;
    return res;
}

bool cmpx(const Triangle &a, const Triangle &b) { 
    if (a.center_pos.x < b.center_pos.x) {
        return true;
    } else if (a.center_pos.x > b.center_pos.x) {
        return false;
    }
    if (a.center_pos.y < b.center_pos.y) {
        return true;
    } else if (a.center_pos.y > b.center_pos.y) {
        return false;
    }
    return a.center_pos.z < b.center_pos.z;
}
bool cmpy(const Triangle &a, const Triangle &b) { 
    if (a.center_pos.y < b.center_pos.y) {
        return true;
    } else if (a.center_pos.y > b.center_pos.y) {
        return false;
    }
    if (a.center_pos.x < b.center_pos.x) {
        return true;
    } else if (a.center_pos.x > b.center_pos.x) {
        return false;
    }
    return a.center_pos.z < b.center_pos.z;
}
bool cmpz(const Triangle &a, const Triangle &b) {
    if (a.center_pos.z < b.center_pos.z) {
        return true;
    } else if (a.center_pos.z > b.center_pos.z) {
        return false;
    }
    if (a.center_pos.x < b.center_pos.x) {
        return true;
    } else if (a.center_pos.x > b.center_pos.x) {
        return false;
    }
    return a.center_pos.y < b.center_pos.y;
}

BVH_Node *build_bvh(std::vector <Triangle> &tr, int depth) {
    int n = tr.size();
    BVH_Node *res = new BVH_Node;

    if (n <= BVH_MAX_TRIANGLES) {
        res->type = 1;
        res->triangles = tr;
        return res;
    }

    AABB box = generate_aabb(tr);
    res->type = 0;
    res->box = box;

    
    if (box.size.x >= box.size.y && box.size.x >= box.size.z) {
        sort(tr.begin(), tr.end(), cmpx);
    }
    else if (box.size.y >= box.size.x && box.size.y >= box.size.z) {
        sort(tr.begin(), tr.end(), cmpy);
    } else {
        sort(tr.begin(), tr.end(), cmpz);
    }
    std::vector <Triangle> tr1;
    std::vector <Triangle> tr2;
    for (int i = 0; i < n; ++i) {
        if (i < n / 2) {
            tr1.push_back(tr[i]);
        } else {
            tr2.push_back(tr[i]);
        }
    }
    res->left = build_bvh(tr1, depth + 1);
    res->right = build_bvh(tr2, depth + 1);
    return res;
}


float bvh_traverse(BVH_Node *cur, float3 pos, float3 ray_pos, float3 ray_dir, float3 &normal) {
    if (cur->type == 1) {
        float min_dist = std::numeric_limits<float>::max();
        int intersect = 0;
        float3 cur_normal;
        int n = cur->triangles.size();
        for (int i = 0; i < n; ++i) {
            float cur_dist;
            float3 cur_normal;
            if (ray_triangle_intersect(
                ray_pos, ray_dir, 
                cur->triangles[i].vert[0] + pos, cur->triangles[i].vert[1] + pos, cur->triangles[i].vert[2] + pos, 
                &cur_dist, &cur_normal
            )) {
                if (!intersect || cur_dist < min_dist) {
                    min_dist = cur_dist;
                    normal = cur_normal;
                }
                intersect = 1;
            }
        }
        if (intersect)
            return min_dist;
        return -1.0f;
    }

    if (ray_box_intersect(cur->box, pos, ray_pos, ray_dir)) {
        float3 normal1;
        float res1 = bvh_traverse(cur->left, pos, ray_pos, ray_dir, normal1);
        float3 normal2;
        float res2 = bvh_traverse(cur->right, pos, ray_pos, ray_dir, normal2);
        if (res1 >= 0 && res2 >= 0) {
            if (res1 <= res2) {
                normal = normal1;
                return res1;
            } else {
                normal = normal2;
                return res2;
            }
        } else if (res1 >= 0) {
            normal = normal1;
            return res1;
        } else if (res2 >= 0) {
            normal = normal2;
            return res2;
        }
        return -1.0f;
    }
    return -1.0f;
}

AABB generate_tl_aabb(std::vector <Object> objects, int obj_count) {
    float3 min_coord = objects[0].pos;
    float3 max_coord = min_coord;
    float3 max_size = objects[0].model->size;
    for (int i = 1; i < obj_count; ++i) {
        min_coord = LiteMath::min(objects[i].pos, min_coord);
        max_coord = LiteMath::max(objects[i].pos, max_coord);
        max_size = LiteMath::max(objects[i].model->size, max_size);
    }
    AABB res;
    res.pos = (min_coord + max_coord) / 2.0f;
    res.size = (max_coord - min_coord + max_size) / 2.0f;
    return res;
}


bool cmp_tl_x(Object &a, Object &b) { return a.pos.x < b.pos.x; }
bool cmp_tl_y(Object &a, Object &b) { return a.pos.y < b.pos.y; }
bool cmp_tl_z(Object &a, Object &b) { return a.pos.z < b.pos.z; }

TLBVH_Node * build_tl_bvh(std::vector <Object> objects) {
    int obj_count = objects.size();

    if (obj_count <= 1) {
        TLBVH_Node *res = new TLBVH_Node;
        res->type = 1;
        res->obj = objects;
        return res;
    }
    TLBVH_Node *res = new TLBVH_Node;

    AABB box = generate_tl_aabb(objects, obj_count);
    res->type = 0;
    res->box = box;

    
    if (box.size.x >= box.size.y && box.size.x >= box.size.z) {
        sort(objects.begin(), objects.end(), cmp_tl_x);
    }
    else if (box.size.y >= box.size.x && box.size.y >= box.size.z) {
        sort(objects.begin(), objects.end(), cmp_tl_y);
    } else {
        sort(objects.begin(), objects.end(), cmp_tl_z);
    }
    std::vector <Object> obj1;
    std::vector <Object> obj2;
    for (int i = 0; i < obj_count; ++i) {
        if (i < obj_count / 2) {
            obj1.push_back(objects[i]);
        } else {
            obj2.push_back(objects[i]);
        }
    }
    res->left = build_tl_bvh(obj1);
    res->right = build_tl_bvh(obj2);
    return res;
}


float tl_bvh_traverse(TLBVH_Node *cur, float3 ray_pos, float3 ray_dir, float3 &normal) {
    if (cur == NULL) {
        return -1;
    }
    if (cur->type == 1) {
        if (cur->obj.size() == 0) {
            return -1;
        }
        float3 cur_normal;
        float dist = bvh_traverse(cur->obj[0].model->bvh, cur->obj[0].pos, ray_pos, ray_dir, cur_normal);
        if (dist >= 0) {
            normal = cur_normal;
            return dist;
        }
        return -1;
    }
    if (ray_box_intersect(cur->box, float3(0.0f), ray_pos, ray_dir)) {
        float3 normal1;
        float res1 = tl_bvh_traverse(cur->left, ray_pos, ray_dir, normal1);
        float3 normal2;
        float res2 = tl_bvh_traverse(cur->right, ray_pos, ray_dir, normal2);

        if (res1 >= 0 && res2 >= 0) {
            if (res1 <= res2) {
                normal = normal1;
                return res1;
            } else {
                normal = normal2;
                return res2;
            }
        } else if (res1 >= 0) {
            normal = normal1;
            return res1;
        } else if (res2 >= 0) {
            normal = normal2;
            return res2;
        }
        return -1.0f;
    }
    return -1.0f;
}