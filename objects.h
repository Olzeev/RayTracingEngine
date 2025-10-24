#include "LiteMath.h"

using LiteMath::float3, LiteMath::float2, LiteMath::length, LiteMath::max, LiteMath::min, 
    LiteMath::abs, LiteMath::lerp;


struct Material {
    float3 color = float3(1.0);
    float ambient;
    float reflection;
    float specular;
    int reflect_n;
};



struct Box {
    float3 pos;
    float3 box_size;
    Material mat;

    float sdf(float3 p) {
        float3 q = abs(p - pos) - box_size;
        return length(max(q, float3(0.0f))) + min(max(q.x, max(q.y, q.z)), 0.0f);
    }

    float2 intersection(float3 ray_pos, float3 ray_dir, float3 &normal) 
    {
        float3 local_pos = ray_pos - pos;
        float3 half_size = box_size * 0.5f;
        
        float3 inv_dir = 1.0f / ray_dir;
        float3 t0 = (-half_size - local_pos) * inv_dir;
        float3 t1 = (half_size - local_pos) * inv_dir;
        
        float3 tmin = min(t0, t1);
        float3 tmax = max(t0, t1);
        
        float tN = max(max(tmin.x, tmin.y), tmin.z);
        float tF = min(min(tmax.x, tmax.y), tmax.z);
        
        if (tN > tF || tF < 0.0) return float2(-1.0);
        
        // Определяем, какая грань была пересечена первой
        normal = float3(0.0f);
        if (tN == tmin.x) normal = float3(-1.0f, 0.0f, 0.0f);
        else if (tN == tmin.y) normal = float3(0.0f, -1.0f, 0.0f);
        else if (tN == tmin.z) normal = float3(0.0f, 0.0f, -1.0f);
        
        // Корректировка нормали для обратных граней
        if (dot(normal, ray_dir) > 0.0f) normal = -normal;
        
        return float2(tN, tF);
    }

    float3 get_normal(float3 point, float EPS) {
        float dx = (sdf(point + float3(EPS, 0, 0)) - sdf(point - float3(EPS, 0, 0)));
        float dy = (sdf(point + float3(0, EPS, 0)) - sdf(point - float3(0, EPS, 0)));
        float dz = (sdf(point + float3(0, 0, EPS)) - sdf(point - float3(0, 0, EPS)));
        return normalize(float3(dx, dy, dz) / 2.0 / EPS);
    }

    private: 
    float3 step_alt(float3 edge, float3 x) {
        return float3(
            x.x >= edge.x ? 1.0f : 0.0f,
            x.y >= edge.y ? 1.0f : 0.0f,
            x.z >= edge.z ? 1.0f : 0.0f
        );
    }
    float3 sign_alt(float3 x) {
        return float3(
            x.x > 0.0f ? 1.0f : (x.x < 0.0f ? -1.0f : 0.0f),
            x.y > 0.0f ? 1.0f : (x.y < 0.0f ? -1.0f : 0.0f),
            x.z > 0.0f ? 1.0f : (x.z < 0.0f ? -1.0f : 0.0f)
        );
    }
};

struct Sphere {
    float3 pos;
    float r;
    Material mat;

    float sdf(float3 p) {
        return length(p - pos) - r;
    }
    float2 intersection(float3 ray_pos, float3 ray_dir, float3 &normal) {
        float3 oc = ray_pos - pos;
        float b = dot( oc, ray_dir );
        float c = dot( oc, oc ) - r * r;
        float h = b * b - c;
        if(h < 0.0) return float2(-1.0); // no intersection
        h = sqrt( h );
        normal = normalize(ray_pos + ray_dir * (-b - h) - pos);
        return float2( -b-h, -b+h );
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
    Material mat;

    float sdf(float3 p)
    {
        return dot(p, dir) + h;
    }

    float2 intersection(float3 ray_pos, float3 ray_dir, float EPS)
    {
        
        // Проверяем параллельность
        float denom = dot(ray_dir, dir);
        if(abs(denom) < EPS) 
            return float2(-1.0, -1.0); // Нет пересечения
        
        // Вычисляем расстояние
        float t = -(dot(ray_pos, dir) + h) / denom;
        
        // Проверяем, что пересечение впереди
        if(t < EPS) 
            return float2(-1.0, -1.0);
        return float2(t, t); // или float2(t, 0.0) в зависимости от контекста
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
    float3 color;

    float lambert(float3 point, float3 normal) {
        return min(max(0.05f, dot(normal, normalize(pos - point))) * power, 1.0f);
    }
};


struct Mundelbulb {
    float3 pos;
    int iterations;
    int power;
    Material mat;

    float sdf(float3 point) {
        float3 z = point - pos;
        float dr = 1.0;
        float r = 0.0;

        for (int i = 0; i < iterations ; i++) {
            r = length(z);
            if (r > 2.0) break;
            
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
        float d = sdf(point);
        float dx = sdf(point + float3(EPS, 0, 0)) - d;
        float dy = sdf(point + float3(0, EPS, 0)) - d;
        float dz = sdf(point + float3(0, 0, EPS)) - d;
        return normalize(float3(dx, dy, dz) / 2.0 / EPS);
    }
};