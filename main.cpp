#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "LiteMath.h"

#include <cstdio>
#include <cstring>
#include <string>
#include <iostream>
#include "scene.h"
#include "custom_utils.h"
#include "public_image.h"



using LiteMath::float3, LiteMath::normalize;

#define DIST_MIN 0.001f
#define ITER_MAX 50
#define PI 3.14159265359
#define FOV PI / 2
#define MAX_DIST 10e5
#define EPS 0.001f



struct Camera {
    float3 pos;
    float3 dir;
};


float3 render(Scene scene, Camera camera, float3 cur_pos, float3 cur_dir) {
    int iter_count = 0;
    bool intersect = false;
    float3 intersect_point;
    float3 intersect_normal;

    while (iter_count < ITER_MAX) {
        float cur_min_dist = MAX_DIST;
        cur_dir = normalize(cur_dir);
        

        for (int i = 0; i < scene.boxes_size; ++i) {
          float dist = scene.boxes[i].sdf(cur_pos);
          if (dist < cur_min_dist) {
            cur_min_dist = dist;
            intersect_normal = scene.boxes[i].get_normal(cur_pos, EPS);
          }
        }
        for (int i = 0; i < scene.spheres_size; ++i) {
          float dist = scene.spheres[i].sdf(cur_pos);
          if (dist < cur_min_dist) {
            cur_min_dist = dist;
            intersect_normal = scene.spheres[i].get_normal(cur_pos, EPS);
          }
        }
        for (int i = 0; i < scene.planes_size; ++i) {
          float dist = scene.planes[i].sdf(cur_pos);
          if (dist < cur_min_dist) {
            cur_min_dist = dist;
            intersect_normal = scene.planes[i].get_normal(cur_pos, EPS);
          }
        }
        if (cur_min_dist <= DIST_MIN) {
          intersect = true;
          intersect_point = cur_pos;

          break;
        } else if (cur_min_dist >= MAX_DIST) {
          break;
        }
        cur_pos = cur_pos + cur_dir * cur_min_dist;

        iter_count++;
    }
    if (intersect) {
      float c = 0;
      for (int i = 0; i < scene.light_sources_count; ++i) {
        c += scene.light_sources[i].lambert(intersect_point, intersect_normal);
      }
      return float3(c);
    } else 
      return float3(0.0f);

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
  bool sample = false;
  int sample_id = -1;

  int spheres_count = 0, boxes_count = 0, planes_count = 0, light_sources_count = 0;
  Sphere *spheres; 
  Box *boxes;
  Plane *planes;
  LightSource *light_sources;

  Camera camera;

  Scene scene;

  for (int i = 1; i < argc; ++i) {
    if (strcmp(argv[i], "-i") == 0) {
      input = true;
    } else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
      std::cout << "-i: input mode\n-s: sample mode, provide a sample id (0-2)\n";
      return 1;
    }
  }
  if (input) {
    std::cout << "Spheres count:\n";
    std::cin >> spheres_count;
    spheres = new Sphere[spheres_count];
    for (int i = 0; i < spheres_count; ++i) {
      float x, y, z, r;
      std::cout << "Sphere " << i + 1 << " position <x y z>:\n";
      std::cin >> x >> y >> z;
      spheres[i].pos = float3(x, y, z); 
      std::cout << "Sphere " << i + 1 << " radius:\n";
      std::cin >> r;
      spheres[i].r = r;
    }

    std::cout << "Boxes count:\n";
    std::cin >> boxes_count;
    boxes = new Box[boxes_count];

    for (int i = 0; i < boxes_count; ++i) {
      float x, y, z, a, b, c;
      std::cout << "Box " << i + 1 << " position <x y z>:\n";
      std::cin >> x >> y >> z;
      boxes[i].pos = float3(x, y, z); 
      std::cout << "Box " << i + 1 << " size <a b c>:\n";
      std::cin >> a >> b >> c;
      boxes[i].box_size = float3(a, b, c);
    }

    std::cout << "Planes count:\n";
    std::cin >> planes_count;
    planes = new Plane[planes_count];

    for (int i = 0; i < planes_count; ++i) {
      float x, y, z, h;
      std::cout << "Plane " << i + 1 << " direction <x y z>:\n";
      std::cin >> x >> y >> z;
      planes[i].dir = normalize(float3(x, y, z)); 
      std::cout << "Plane " << i + 1 << " height:\n";
      std::cin >> h;
      planes[i].h = h;
    }

    std::cout << "Light sources count:\n";
    std::cin >> light_sources_count;
    light_sources = new LightSource[light_sources_count];

    for (int i = 0; i < light_sources_count; ++i) {
      float x, y, z, p;
      std::cout << "Light source " << i + 1 << " position <x y z>:\n";
      std::cin >> x >> y >> z;
      light_sources[i].pos = float3(x, y, z);
      std::cout << "Light source " << i + 1 << " power:\n";
      std::cin >> p;
      light_sources[i].power = p;
    }

    float cx, cy, cz;
    std::cout << "Camera position <x y z>:\n";
    std::cin >> cx >> cy >> cz;
    camera.pos = float3(cx, cy, cz);
    float dx, dy, dz;
    std::cout << "Camera direction <x y z>:\n";
    std::cin >> dx >> dy >> dz;
    camera.dir = normalize(float3(dx, dy, dz));

    scene = Scene{
      boxes, boxes_count, 
      spheres, spheres_count, 
      planes, planes_count, 
      light_sources, light_sources_count
    };
  }
  std::cout << "\nRendering started...\n";
  
  for (int x = 0; x < W; ++x) {
    for (int y = 0; y < H; ++y) {
      float3 new_dir = screen_offset(camera.dir, x, y, W, H, FOV);
      //std::cout << "h\n";
      float3 col = render(scene, camera, camera.pos, new_dir);
      //std::cout << new_dir.x << ' ' << new_dir.y << ' ' << new_dir.z << '\n';
      image[get_image_index(x, y, 'r', W, H)] = col.x;
      image[get_image_index(x, y, 'g', W, H)] = col.y;
      image[get_image_index(x, y, 'b', W, H)] = col.z;
    }
  }
  std::cout << "Rendering finished!\n";

  write_image_rgb("out.png", image, W, H);

  std::cout << "Image created!\n";
  return 0;
}