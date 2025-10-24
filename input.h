#include <iostream>
#include "scene.h"


void input_material(Material &mat) {
  std::cout << "Material color <r[0-1] g[0-1] b[0-1]>:\n";
  std::cin >> mat.color.x >> mat.color.y >> mat.color.z;
  std::cout << "Material ambient [0-1]:\n";
  std::cin >> mat.ambient;
  std::cout << "Material reflection [0-1]:\n";
  std::cin >> mat.reflection;
  std::cout << "Material specular [0-1]:\n";
  std::cin >> mat.specular;
  std::cout << "Material reflection power (2, 4, 16, ...):\n";
  std::cin >> mat.reflect_n;
}


void input_objects(Sphere *spheres, int &spheres_count, 
    Box *boxes, int &boxes_count, 
    Plane *planes, int &planes_count, 
    Mundelbulb *fractals, int &fractals_count, 
    LightSource *lights, int &lights_count,
    Camera &camera, 
    Scene &scene
) {

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
      Material mat;
      input_material(mat);
      spheres[i].mat = mat;
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
      Material mat;
      input_material(mat);
      boxes[i].mat = mat;
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
      Material mat;
      input_material(mat);
      planes[i].mat = mat;
    }

    std::cout << "Mundelbulbs count:\n";
    std::cin >> fractals_count;
    fractals = new Mundelbulb[fractals_count];
    
    for (int i = 0; i < fractals_count; ++i) {
      float x, y, z, bailout;
      int iter, power;

      std::cout << "Fractal " << i + 1 << " position <x y z>:\n";
      std::cin >> x >> y >> z;
      std::cout << "Fractal " << i + 1 << " render iterations: \n";
      std::cin >> iter;
      std::cout << "Fractal " << i + 1 << " power:\n";
      std::cin >> power;

      fractals[i].pos = float3(x, y, z);
      fractals[i].iterations = iter;
      fractals[i].power = power;
      Material mat;
      input_material(mat);
      fractals[i].mat = mat;
    }

    std::cout << "Light sources count:\n";
    std::cin >> lights_count;
    lights = new LightSource[lights_count];

    for (int i = 0; i < lights_count; ++i) {
      float x, y, z, p;
      float r, g, b;
      float3 color;
      std::cout << "Light source " << i + 1 << " position <x y z>:\n";
      std::cin >> x >> y >> z;
      lights[i].pos = float3(x, y, z);
      std::cout << "Light source " << i + 1 << "power:\n";
      std::cin >> p;
      lights[i].power = p;
      std::cout << "Light source " << i + 1 << "color <r[0-1] g[0-1] b[0-1]>:\n";
      std::cin >> r >> g >> b;
      lights[i].color = float3(r, g, b);
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
      fractals, fractals_count,
      lights, lights_count
    };
}

void input_const(
  float &DIST_MIN, 
  int &ITER_MAX, 
  float &FOV,
  float &MAX_DIST, 
  float &EPS, 
  int &REFLECT_ITERATIONS, 
  int &DIFFUSE_RAYS_COUNT, 
  float3 &BACKGROUND_COLOR
) {
  std::cout << "Min distance:\n";
  std::cin >> DIST_MIN;
  std::cout << "Max distance:\n";
  std::cin >> MAX_DIST;
  std::cout << "Max raymarching iiterations:\n";
  std::cin >> ITER_MAX;
  std::cout << "FOV (Pi * ...):\n";
  float c;
  std::cin >> c;
  FOV = 3.14159265359 * c;
  std::cout << "Epsilon:\n";
  std::cin >> EPS;
  std::cout << "Raytracing iterations:\n";
  std::cin >> REFLECT_ITERATIONS;
  std::cout << "Diffuse rays count:\n";
  std::cin >> DIFFUSE_RAYS_COUNT;
  std::cout << "Backgorund color <r[0-1] g[0-1] b[0-1]>:\n";
  std::cin >> BACKGROUND_COLOR.x >> BACKGROUND_COLOR.y >> BACKGROUND_COLOR.z;
}