#include "LiteMath.h"
#include <random>

using LiteMath::float3, LiteMath::float2, LiteMath::cross, LiteMath::normalize;

double PI = 3.14159265358979323846;

float sum3(float3 vec) {
    return vec.x + vec.y + vec.z;
}

float2 normalize_screen_offset(int x, int y, const int W, const int H) {
    float dx = float(x - W / 2) / (W / 2);
    float dy = float(H / 2 - y) / (H / 2);
    return float2(dx, dy);
}


float3 screen_offset(float3 dir, int x, int y, const int W, const int H, float fov) {
    float2 dv = normalize_screen_offset(x, y, W, H);
    float aspect_ratio = (float)W / H;
    float2 offset = float2(dv.x * tan(fov / 2), dv.y * tan(fov / 2) / aspect_ratio);
    
    float3 world_up = float3(0, 1, 0);
    float3 right = normalize(cross(world_up, dir));
    float3 up = normalize(cross(dir, right));
    float3 new_direction = dir + offset.x * right + offset.y * up;
    return normalize(new_direction);
}


float random_float() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    return dist(gen);
}


float3 random_on_hemisphere(float3 normal) {
    float r1 = random_float();
    float r2 = random_float();
    float phi = 2 * PI * r1;
    float cosTheta = sqrt(1 - r2);
    float sinTheta = sqrt(r2);

    float3 localDir = float3(
        cos(phi) * sinTheta,
        sin(phi) * sinTheta,
        cosTheta
    );

    float3 tangent;
    if (abs(normal.x) > abs(normal.y)) {
        tangent = normalize(cross(normal, float3(0, 1, 0)));
    } else {
        tangent = normalize(cross(normal, float3(1, 0, 0)));
    }
    float3 bitangent = cross(normal, tangent);

    return normalize(
        tangent * localDir.x +
        bitangent * localDir.y +
        normal * localDir.z
    );
}

float3 perturb_direction(float3 perfect_reflect_dir, float glossiness) {
    // Чем выше glossiness, тем меньше отклонение
    float roughness = 1.0 / glossiness;
    
    // Случайное отклонение в пределах полусферы
    float3 random_vec = random_on_hemisphere(perfect_reflect_dir);
    
    // Интерполяция между идеальным направлением и случайным
    return normalize(mix(perfect_reflect_dir, random_vec, roughness));
}