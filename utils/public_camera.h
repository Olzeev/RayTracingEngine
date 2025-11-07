#pragma once
#include "LiteMath.h"
#include <cstring>
#include <cstdio>
#include <errno.h>

using LiteMath::float3, LiteMath::float2;

float2 normalize_screen_offset(int x, int y, const int W, const int H) {
    float dx = float(x - W / 2) / (W / 2);
    float dy = float(H / 2 - y) / (H / 2);
    return float2(dx, dy);
}


float3 screen_offset(float3 dir, int x, int y, const int W, const int H) {
    float2 dv = normalize_screen_offset(x, y, W, H);

    float2 offset = float2(
        dv.x * tan(LiteMath::M_PI / 2 * 0.5f),
        dv.y * tan(LiteMath::M_PI / 3 * 0.5f)
    );

    float3 world_up = float3(0, 1, 0);
    float3 right = normalize(cross(world_up, dir));
    float3 up = normalize(cross(dir, right));

    float3 new_dir = dir + offset.x * right + offset.y * up;
    return normalize(new_dir);
}


struct Camera
{
  float3 pos;         // camera position
  float3 dir;      // point, at which camera is looking
  float speed;
  float angle_x;
  float angle_y;
  float sensitivity;
  float z_near = 0.1f;                // distance to near plane
  float z_far = 100.0f;               // distance to far plane

  bool to_file(const char *filename)
  {
    FILE *f = fopen(filename, "w");
    if (!f)
    {
      fprintf(stderr, "failed to open/create file %s. Errno %d\n", filename, (int)errno);
      return false;
    }
    fprintf(f, "camera_position = %f, %f, %f\n", pos.x, pos.y, pos.z);
    fprintf(f, "dir = %f, %f, %f\n", dir.x, dir.y, dir.z);
    fprintf(f, "speed = %f\n", speed);
    fprintf(f, "z_near  = %f\n", z_near);
    fprintf(f, "z_far  = %f\n", z_far);

    int res = fclose(f);
    if (res != 0)
    {
      fprintf(stderr, "failed to close file %s. fclose returned %d\n", filename, res);
      return false;
    }
    return true;
  }

  bool from_file(const char *filename)
  {
    FILE *f = fopen(filename, "r");
    if (!f)
    {
      fprintf(stderr, "failed to open file %s. Errno %d\n", filename, (int)errno);
      return false;
    }
    fscanf(f, "camera_position = %f, %f, %f\n", &pos.x, &pos.y, &pos.z);
    fscanf(f, "dir = %f, %f, %f\n", &dir.x, &dir.y, &dir.z);
    fscanf(f, "speed = %f\n", &speed);
    fscanf(f, "z_near  = %f\n", &z_near);
    fscanf(f, "z_far  = %f\n", &z_far);

    int res = fclose(f);
    if (res != 0)
    {
      fprintf(stderr, "failed to close file %s. fclose returned %d\n", filename, res);
      return false;
    }
    return true;
  }
};

struct DirectedLight
{
  float3 dir; // direction TO light, i.e 0,1,0 if light is above
  float intensity = 1.0f;

  bool to_file(const char *filename)
  {
    FILE *f = fopen(filename, "w");
    if (!f)
    {
      fprintf(stderr, "failed to open/create file %s. Errno %d\n", filename, (int)errno);
      return false;
    }
    fprintf(f, "light direction = %f, %f, %f\n", dir.x, dir.y, dir.z);
    fprintf(f, "intensity = %f\n", intensity);

    int res = fclose(f);
    if (res != 0)
    {
      fprintf(stderr, "failed to close file %s. fclose returned %d\n", filename, res);
      return false;
    }
    return true;
  }

  bool from_file(const char *filename)
  {
    FILE *f = fopen(filename, "r");
    if (!f)
    {
      fprintf(stderr, "failed to open file %s. Errno %d\n", filename, (int)errno);
      return false;
    }
    fscanf(f, "light direction = %f, %f, %f\n", &dir.x, &dir.y, &dir.z);
    fscanf(f, "intensity = %f\n", &intensity);

    int res = fclose(f);
    if (res != 0)
    {
      fprintf(stderr, "failed to close file %s. fclose returned %d\n", filename, res);
      return false;
    }
    return true;
  }
};