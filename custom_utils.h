#include "LiteMath.h"

using LiteMath::float3, LiteMath::float2, LiteMath::cross, LiteMath::normalize;

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

