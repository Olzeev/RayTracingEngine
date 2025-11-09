#include "LiteMath.h"

using LiteMath::float3;

bool ray_triangle_intersect(float3 orig, float3 dir,
                            float3 v0, float3 v1, float3 v2,
                            float *t, float3 *normal)
{
    const float EPS = 1e-8f;

    

    float3 edge1 = v1 - v0;
    float3 edge2 = v2 - v0;

    float3 pvec = cross(dir, edge2);
    float det = dot(edge1, pvec);

    if (fabsf(det) < EPS)
        return false;

    float invDet = 1.0f / det;

    float3 tvec = orig - v0;
    float u = dot(tvec, pvec) * invDet;
    if (u < 0.0f || u > 1.0f)
        return false;

    float3 qvec = cross(tvec, edge1);
    float v = dot(dir, qvec) * invDet;
    if (v < 0.0f || u + v > 1.0f)
        return false;

    *normal = normalize(cross(v1 - v0, v2 - v0));
    if (dot(*normal, dir) > 0.0f)
        *normal = -(*normal);
    *t = dot(edge2, qvec) * invDet;

    return *t > EPS;
}

bool ray_box_intersect(AABB& box, float3 pos, const float3& ro, const float3& rd)
{
    // избегаем деления на 0
    float3 invDir = float3(
        (fabsf(rd.x) > 1e-8f) ? 1.0f / rd.x : 1e8f,
        (fabsf(rd.y) > 1e-8f) ? 1.0f / rd.y : 1e8f,
        (fabsf(rd.z) > 1e-8f) ? 1.0f / rd.z : 1e8f
    );

    const float3 minB = box.pos + pos - box.size;
    const float3 maxB = box.pos + pos + box.size;

    const float3 t1 = (minB - ro) * invDir;
    const float3 t2 = (maxB - ro) * invDir;

    const float3 tmin = min(t1, t2);
    const float3 tmax = max(t1, t2);

    const float tNear = std::max(std::max(tmin.x, tmin.y), tmin.z);
    const float tFar  = std::min(std::min(tmax.x, tmax.y), tmax.z);

    // true, если интервалы пересекаются и коробка не за лучом
    return (tNear <= tFar) && (tFar >= 0.0f);
}
