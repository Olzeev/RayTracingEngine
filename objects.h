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

    float2 intersection(float3 ray_pos, float3 ray_dir, float EPS, float3 &normal)
    {
        
        // Проверяем параллельность
        float denom = dot(ray_dir, dir);
        if(fabs(denom) < EPS) 
            return float2(-1.0, -1.0); // Нет пересечения
        
        // Вычисляем расстояние
        float t = -(dot(ray_pos, dir) + h) / denom;
        
        // Проверяем, что пересечение впереди
        if(t < EPS) 
            return float2(-1.0, -1.0);

        normal = dir;
        if (dot(ray_dir, normal) > 0) {
            normal = -normal;
        }
        
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


struct Fractal {
    int type;
    float3 pos;
    int iterations;
    int power;
    float scale;
    Material mat;

    float sdf(float3 point) {
        if (type == 1) {
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
        } else {
            // Вершины тетраэдра (единичного)
            const float3 verts[4] = {
                float3( -0.5f, 0.0f,  -1.0 / 2 / sqrt(3)),  // основание
                float3(0.5f, 0.0f,  -1.0 / 2 / sqrt(3)),  // основание
                float3( 0.0f, 0.0f, sqrt(3) / 2),  // основание
                float3( 0.0f, sqrt(2.0 / 3.0),  0.0f)   // вершина сверху
            };
            float3 p = point - pos;
            float3 q = p;
            float scale = 1.0f;
            float thickness = 0.001f;

            for (int i = 0; i < iterations; i++)
            {
                // Находим ближайшую вершину
                float d0 = length(q - verts[0]);
                float d1 = length(q - verts[1]);
                float d2 = length(q - verts[2]);
                float d3 = length(q - verts[3]);

                int id = 0;
                float dmin = d0;
                if (d1 < dmin) { dmin = d1; id = 1; }
                if (d2 < dmin) { dmin = d2; id = 2; }
                if (d3 < dmin) { dmin = d3; id = 3; }

                // Инвертируем и масштабируем пространство
                q = q * 2.0f - verts[id];
                scale *= 2.0f;
            }

            // Расстояние до тетраэдра (аппроксимация)
            float dist = (length(q) - 1.0f) / scale;
            return dist;
            
        }
        
    }

    float3 get_normal(float3 point, float EPS) {
        float d = sdf(point);
        float dx = sdf(point + float3(EPS, 0, 0)) - d;
        float dy = sdf(point + float3(0, EPS, 0)) - d;
        float dz = sdf(point + float3(0, 0, EPS)) - d;
        return normalize(float3(dx, dy, dz) / 2.0 / EPS);
    }
};
