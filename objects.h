#include "LiteMath.h"

using LiteMath::float3, LiteMath::float2, LiteMath::length, LiteMath::max, LiteMath::min, 
    LiteMath::abs;

struct Box {
    float3 pos;
    float3 box_size;

    float sdf(float3 p) {
        float3 q = abs(p - pos) - box_size;
        return length(max(q, float3(0.0f))) + min(max(q.x, max(q.y, q.z)), 0.0f);
    }

    float3 get_normal(float3 point, float EPS) {
        float dx = (sdf(point + float3(EPS, 0, 0)) - sdf(point - float3(EPS, 0, 0)));
        float dy = (sdf(point + float3(0, EPS, 0)) - sdf(point - float3(0, EPS, 0)));
        float dz = (sdf(point + float3(0, 0, EPS)) - sdf(point - float3(0, 0, EPS)));
        return normalize(float3(dx, dy, dz) / 2.0 / EPS);
    }
};

struct Sphere {
    float3 pos;
    float r;

    float sdf(float3 p) {
        return length(p - pos) - r;
    }
    float3 get_normal(float3 point, float EPS) {
        float dx = (sdf(point + float3(EPS, 0, 0)) - sdf(point - float3(EPS, 0, 0)));
        float dy = (sdf(point + float3(0, EPS, 0)) - sdf(point - float3(0, EPS, 0)));
        float dz = (sdf(point + float3(0, 0, EPS)) - sdf(point - float3(0, 0, EPS)));
        return normalize(float3(dx, dy, dz) / 2.0 / EPS);
    }
};

struct Plane {
    float h;
    float3 dir;

    float sdf(float3 p)
    {
        return dot(p, dir) + h;
    }
    float3 get_normal(float3 point, float EPS) {
        float dx = (sdf(point + float3(EPS, 0, 0)) - sdf(point - float3(EPS, 0, 0)));
        float dy = (sdf(point + float3(0, EPS, 0)) - sdf(point - float3(0, EPS, 0)));
        float dz = (sdf(point + float3(0, 0, EPS)) - sdf(point - float3(0, 0, EPS)));
        return normalize(float3(dx, dy, dz) / 2.0 / EPS);
    }
};

struct LightSource {
    float3 pos;
    float power;

    float lambert(float3 point, float3 normal) {
        return min(max(0.05f, dot(normal, normalize(pos - point))) * power, 1.0f);
    }
};


struct Mundelbulb {
    float3 pos;
    int iterations;
    float bailout;
    int power;

    float sdf(float3 point) {
        float3 z = point - pos;
        float dr = 1.0;
        float r = 0.0;

        for (int i = 0; i < iterations ; i++) {
            r = length(z);
            if (r > bailout) break;
            
            // convert to polar coordinates
            float theta = acos(z.z / r);
            float phi = atan2(z.y, z.x);
            dr = pow(r, power - 1.0) * power * dr + 1.0;
            
            // scale and rotate the point
            float zr = pow(r, power);
            theta = theta * power;
            phi = phi * power;
            
            // convert back to cartesian coordinates
            z = zr * float3(sin(theta) * cos(phi), sin(phi) * sin(theta), cos(theta));
            z += point - pos;
        }
        return 0.5 * log(r) * r / dr;
    }

    float3 get_normal(float3 point, float EPS) {
        //float d = sdf(point);
        float dx = (sdf(point + float3(EPS, 0, 0)) - sdf(point - float3(EPS, 0, 0)));
        float dy = (sdf(point + float3(0, EPS, 0)) - sdf(point - float3(0, EPS, 0)));
        float dz = (sdf(point + float3(0, 0, EPS)) - sdf(point - float3(0, 0, EPS)));
        return normalize(float3(dx, dy, dz) / 2.0 / EPS);
    }
};