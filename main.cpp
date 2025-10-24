#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "LiteMath.h"

#include <cstdio>
#include <cstring>
#include <string>
#include "custom_utils.h"
#include "input.h"
#include "public_image.h"



using LiteMath::float3, LiteMath::normalize;

// GENERAL
float FOV = PI / 2;

// RAYMARCHING
int ITER_MAX = 50;
float DIST_MIN = 0.001f;
float MAX_DIST = 10e5;
float EPS = 0.0001f;

// RAYTRACING
int REFLECT_ITERATIONS = 2;
int DIFFUSE_RAYS_COUNT = 2;
float3 BACKGROUND_COLOR = float3(0.0);




int ray_marching(
  Scene scene, float3 cur_pos, float3 cur_dir, 
  float3 &intersect_point, 
  float3 &intersect_normal, 
  Material &mat
) {
  
  float3 cur_intersection_point, cur_intersection_normal;
    for (int iter = 0; iter < ITER_MAX; ++iter) {
        float cur_min_dist = MAX_DIST;
        Mundelbulb intersect_object;
        for (int i = 0; i < scene.fractals_size; ++i) {
          float dist = scene.fractals[i].sdf(cur_pos);
          if (dist < cur_min_dist) {
            cur_min_dist = dist;
            intersect_object = scene.fractals[i];
          }
        }
        if (cur_min_dist <= DIST_MIN) {
          intersect_point = cur_pos;
          intersect_normal = intersect_object.get_normal(cur_pos, EPS);
          mat = intersect_object.mat;
          return 0;
        } else if (cur_min_dist >= MAX_DIST) {
          return -1;
        }
        cur_pos = cur_pos + cur_dir * cur_min_dist;
    }
    return -1;
}



int ray_tracing( 
  Scene scene, float3 cur_pos, float3 cur_dir, 
  float3 &intersect_point, 
  float3 &intersect_normal, 
  Material &mat
) {
  float2 cur_min_dist = float2(MAX_DIST);
  bool intersect = false;
  cur_pos += cur_dir * EPS;
  for (int i = 0; i < scene.boxes_size; ++i) {
    float3 cur_normal;
    float2 dist = scene.boxes[i].intersection(cur_pos, cur_dir, cur_normal);
    if (dist.x >= 0 && dist.x < cur_min_dist.x) {
      cur_min_dist = dist;
      intersect_normal = cur_normal;
      intersect = true;
      mat = scene.boxes[i].mat;
    }
  }
  for (int i = 0; i < scene.spheres_size; ++i) {
    float3 cur_normal;
    float2 dist = scene.spheres[i].intersection(cur_pos, cur_dir, cur_normal);
    if (dist.x >= 0 && dist.x < cur_min_dist.x) {
      cur_min_dist = dist;
      intersect_normal = cur_normal;
      intersect = true;
      mat = scene.spheres[i].mat;
    }
  }
  for (int i = 0; i < scene.planes_size; ++i) {
    float3 cur_normal;
    float2 dist = scene.planes[i].intersection(cur_pos, cur_dir, 1e-6);
    if (dist.x >= 0 && dist.x < cur_min_dist.x) {
      cur_min_dist = dist;
      intersect_normal = scene.planes[i].dir;
      intersect = true;
      mat = scene.planes[i].mat;
    }
  }
  if (intersect) {
    intersect_point = cur_pos + cur_min_dist.x * cur_dir;
    return 0;
  }
  return -1;
}


float ambient_occlusion(Scene scene, float3 point, float3 normal) {
  int cnt = 0;
  float radius = 5.0f;
  for (int i = 0; i < DIFFUSE_RAYS_COUNT; ++i) {
    float3 cur_dir = random_on_hemisphere(normal);
    float3 p1, p2, p3;
    Material mat;
    if (ray_tracing(scene, point, cur_dir, p1, p3, mat) == 0 || ray_marching(scene, point, cur_dir, p2, p3, mat) == 0) {
      float dist = min(length(p1 - point), length(p2 - point));
      if (dist <= radius) {
        cnt++;
      }
    }
  }
  //std::cout << 1.0f - float(cnt) / DIFFUSE_RAYS_COUNT << '\n';
  return 1.0f - float(cnt) / DIFFUSE_RAYS_COUNT;
}

float3 render(Scene scene, float3 cur_pos, float3 cur_dir, int iter) {
  if (iter >= REFLECT_ITERATIONS) {
    return float3(0.0);
  }
  float3 intersect_point1, intersect_normal1;
  Material mat1;
  int res1 = ray_marching(scene, cur_pos, cur_dir, intersect_point1, intersect_normal1, mat1);
  
  float3 intersect_point2, intersect_normal2;
  Material mat2;
  int res2 = ray_tracing(scene, cur_pos, cur_dir, intersect_point2, intersect_normal2, mat2);


  if (res1 != 0 && res2 != 0) {
    return BACKGROUND_COLOR;
  }
  if ((res1 == 0 && res2 == 0)) {
    float dist1 = length(intersect_point1 - cur_pos);
    float dist2 = length(intersect_point2 - cur_pos);
    if (dist2 < dist1) { 
      intersect_point1 = intersect_point2;
      mat1 = mat2;
      intersect_normal1 = intersect_normal2;
    }
  } else if (res2 == 0) {
    intersect_point1 = intersect_point2;
    mat1 = mat2;
    intersect_normal1 = intersect_normal2;
  }

  
  float3 reflect_dir = reflect(cur_dir, intersect_normal1);


  float AO = ambient_occlusion(scene, intersect_point1, intersect_normal1);
  float3 I_a = mat1.ambient * BACKGROUND_COLOR * AO;
  float3 I_d = float3(0.0);
  float3 I_s = float3(0.0);

  for (int i = 0; i < scene.light_sources_count; ++i) {
    float3 L = scene.light_sources[i].pos - intersect_point1;
    float3 Ln = normalize(L);
    float3 p1, p2, p3, p4;
    Material mat;
    if (ray_tracing(scene, intersect_point1, Ln, p1, p2, mat) == 0 || ray_marching(scene, intersect_point1 + Ln * DIST_MIN, Ln, p3, p4, mat) == 0) {
      float dist = min(length(p1 - intersect_point1), length(p3 - intersect_point1));
      if (dist < length(L)) 
        continue;
    }
    
    float3 R = reflect(-Ln, intersect_normal1);
    I_d += mat1.color * scene.light_sources[i].color * max(0.0f, dot(intersect_normal1, Ln));
    I_s += mat1.specular * scene.light_sources[i].color * scene.light_sources[i].power * pow(max(0.0f, dot(R, -cur_dir)), mat1.reflect_n);
  }
  float3 color = I_a + I_d + I_s;
  

  return color + render(scene, intersect_point1, reflect_dir, iter + 1) * mat1.reflection;

}

int get_image_index(int x, int y, char c, const int W, const int H) {
  if (c == 'r') {
    return (y * W + x) * 3;
  } else if (c == 'g') {
    return (y * W + x) * 3 + 1;
  } else if (c == 'b') {
    return (y * W + x) * 3 + 2;
  } 
  return -1;
}


int main(int argc, char **argv)
{
  constexpr int W = 1024;
  constexpr int H = 1024;
  std::vector<float> image(3*W*H, 0.0f);
  
  bool input = false;
  bool const_input_flag = false;

  int spheres_count = 0, boxes_count = 0, planes_count = 0;
  int fractals_count = 0;
  int lights_count = 0;

  Sphere *spheres; 
  Box *boxes;
  Plane *planes;
  Mundelbulb *fractals;
  LightSource *lights;


  Camera camera;

  Scene scene;

  for (int i = 1; i < argc; ++i) {
    if (strcmp(argv[i], "-i") == 0 || strcmp(argv[i], "-input") == 0) {
      input = true;
    } else if (strcmp(argv[i], "-c") == 0 || strcmp(argv[i], "--const") == 0) {
      const_input_flag = true;
    } else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
      std::cout << "-i: input mode\n-s: sample mode, provide a sample id (0-2)\n";
      return 1;
    }
  }
  if (input) {
    input_objects(spheres, spheres_count, 
      boxes, boxes_count, 
      planes, planes_count, 
      fractals, fractals_count, 
      lights, lights_count,
      camera, 
      scene
    );
  }
  if (const_input_flag) {
    input_const(DIST_MIN, ITER_MAX, FOV, MAX_DIST, EPS, REFLECT_ITERATIONS, DIFFUSE_RAYS_COUNT, BACKGROUND_COLOR);
  }
  std::cout << "\nRendering started...\n";
  
  for (int x = 0; x < W; ++x) {
    for (int y = 0; y < H; ++y) {
      float3 new_dir = screen_offset(camera.dir, x, y, W, H, FOV);
      float3 col = render(scene, camera.pos, new_dir, 0);
      image[get_image_index(x, y, 'r', W, H)] = col.x;
      image[get_image_index(x, y, 'g', W, H)] = col.y;
      image[get_image_index(x, y, 'b', W, H)] = col.z;
    }
    if (x % 50 == 0)
      std::cout << '\r' << float(x) / W * 100 << "%\n" << std::flush;
  }
  std::cout << "Rendering finished!\n";

  write_image_rgb("out.png", image, W, H);

  std::cout << "Image created!\n";
  return 0;
}