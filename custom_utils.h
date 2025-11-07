#include "utils/LiteMath.h"

using LiteMath::float3;

bool ray_triangle_intersect(float3 orig, float3 dir,
                            float3 v0, float3 v1, float3 v2,
                            float *t, float3 *normal)
{
    const float EPS = 1e-6f;

    *normal = normalize(cross(v1 - v0, v2 - v0));
    if (dot(*normal, dir) > 0.0f)
        *normal = -(*normal);

    float3 edge1 = v1 - v0;
    float3 edge2 = v2 - v0;

    float3 pvec = cross(dir, edge2);
    float det = dot(edge1, pvec);

    // Если детерминант ≈ 0 → луч параллелен треугольнику
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

    *t = dot(edge2, qvec) * invDet;

    return *t > EPS; // пересечение впереди луча
}