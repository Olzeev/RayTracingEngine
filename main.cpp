#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION


#include "utils/public_camera.h"
#include "utils/public_image.h"

#include <cstdio>
#include <cstring>
#include <SDL_keycode.h>
#include <cstdint>
#include <iostream>
#include <fstream>
#include <SDL.h>
#include <chrono>
#include "utils/LiteMath.h"
#include "utils/object.h"



using LiteMath::float2;
using LiteMath::float3;
using LiteMath::float4;
using LiteMath::int2;
using LiteMath::int3;
using LiteMath::int4;
using LiteMath::uint2;
using LiteMath::uint3;
using LiteMath::uint4;

static constexpr int SCREEN_WIDTH  = 1280;
static constexpr int SCREEN_HEIGHT = 720;

float rad_to_deg(float rad) { return rad * 180.0f / M_PI; }

uint32_t float3_to_RGBA8(float3 c)
{
  uint8_t r = (uint8_t)(std::clamp(c.x,0.0f,1.0f)*255.0f);
  uint8_t g = (uint8_t)(std::clamp(c.y,0.0f,1.0f)*255.0f);
  uint8_t b = (uint8_t)(std::clamp(c.z,0.0f,1.0f)*255.0f);
  return 0xFF000000 | (r<<16) | (g<<8) | b;
}


void render(const Camera &camera, uint32_t *out_image, int W, int H, 
  Object models[], int models_count
)
{

  #pragma omp parallel for collapse(2)
  for (int y=0;y<H;y++)
  {
    for (int x=0;x<W;x++)
    {
      float3 cur_dir = screen_offset(camera.dir, x, y, W, H);
      float3 color = float3(0.0f);

      char intersection = 0;
      float min_dist = std::numeric_limits<float>::max();
      float3 normal;

      for (int i = 0; i < models_count; ++i) {
        float3 cur_normal;
        float cur_dist = bvh_traverse(models[i].bvh, camera.pos, cur_dir, cur_normal);
        if (cur_dist >= 0) {
          if (!intersection || cur_dist < min_dist) {
            min_dist = cur_dist;
            normal = cur_normal;
            intersection = 1;
          }
        }
      }
      if (intersection) {
        color = float3(0.2f + dot(normal, normalize(float3(-1, 1, 0.2))));
      }
      out_image[y*W + x] = float3_to_RGBA8(color);
    }
  }
}


// You must include the command line parameters for your main function to be recognized by SDL
int main(int argc, char **args)
{
  // Pixel buffer (RGBA format)
  std::vector<uint32_t> pixels(SCREEN_WIDTH * SCREEN_HEIGHT, 0xFFFFFFFF); // Initialize with white pixels
  
  //cmesh4::SimpleMesh mesh = cmesh4::LoadMeshFromObj("models/stanford-bunny.obj", true);
  cmesh4::SimpleMesh bunny = cmesh4::LoadMeshFromObj("models/stanford-bunny.obj", true);
  Object objects[1];
  
  size_t m_tr_num = bunny.IndicesNum() / 3;      
  for (size_t j = 0; j < m_tr_num; ++j) {
    unsigned int i0 = bunny.indices[3*j + 0];
    unsigned int i1 = bunny.indices[3*j + 1];
    unsigned int i2 = bunny.indices[3*j + 2];

    float4 v0 = bunny.vPos4f[i0];
    float4 v1 = bunny.vPos4f[i1];
    float4 v2 = bunny.vPos4f[i2];
    Triangle tr;
    tr.vert[0] = float3(v0.x, v0.y, v0.z);
    tr.vert[1] = float3(v1.x, v1.y, v1.z);
    tr.vert[2] = float3(v2.x, v2.y, v2.z);
    tr.center_pos = (tr.vert[0] + tr.vert[1] + tr.vert[2]) / 3;
    tr.ind = j;
    objects[0].tr.push_back(tr);
  }
  objects[0].pos = float3(0.0f);
  std::cout << "Building BVH...\n";
  objects[0].bvh = build_bvh(objects[0].tr, 0);
  std::cout << "BVH built!\n";

  // Initialize SDL. SDL_Init will return -1 if it fails.
  if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
  {
    std::cerr << "Error initializing SDL: " << SDL_GetError() << std::endl;
    return 1;
  }

  // Create our window
  SDL_Window *window = SDL_CreateWindow("SDF Viewer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                        SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

  // Make sure creating the window succeeded
  if (!window)
  {
    std::cerr << "Error creating window: " << SDL_GetError() << std::endl;
    return 1;
  }

  // Create a renderer
  SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (!renderer)
  {
    std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 1;
  }

  // Create a texture
  SDL_Texture *texture = SDL_CreateTexture(
      renderer,
      SDL_PIXELFORMAT_ARGB8888,    // 32-bit RGBA format
      SDL_TEXTUREACCESS_STREAMING, // Allows us to update the texture
      SCREEN_WIDTH,
      SCREEN_HEIGHT);

  if (!texture)
  {
    std::cerr << "Texture could not be created! SDL_Error: " << SDL_GetError() << std::endl;
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 1;
  }
  SDL_ShowCursor(SDL_DISABLE);
  SDL_SetRelativeMouseMode(SDL_TRUE);

  SDL_Event ev;
  bool running = true;

  Camera camera;
  camera.pos = float3(-3, 0, 0);
  camera.dir = float3(1, 0, 0);
  camera.angle_x = 0.0f;
  camera.angle_y = 0.0f;
  camera.speed = 2.0f;
  camera.sensitivity = 0.05f;

  auto time = std::chrono::high_resolution_clock::now();
  auto prev_time = time;
  float time_from_start = 0;
  uint32_t frameNum = 0;

  const Uint8* keys = SDL_GetKeyboardState(NULL);

  // Main loop
  while (running)
  {
    //update camera or scene
    prev_time = time;
    time = std::chrono::high_resolution_clock::now();

    //get delta time in seconds
    float dt = std::chrono::duration<float, std::milli>(time - prev_time).count() / 1000.0f;
    time_from_start += dt;
    frameNum++;

    if (frameNum % 10 == 0)
      printf("Render time: %f ms\n", 1000.0f*dt);
    // Process keyboard input
    while (SDL_PollEvent(&ev) != 0)
    {
      // check event type
      switch (ev.type)
      {
      case SDL_QUIT:
        // shut down
        running = false;
        break;
      case SDL_KEYDOWN:
        // test keycode
        switch (ev.key.keysym.sym)
        {
          //ESC to exit 
          case SDLK_ESCAPE:
            running = false;
            break;
        }
        break;
      case SDL_MOUSEMOTION:
        {
            int dx = -ev.motion.xrel;
            int dy = ev.motion.yrel;

            camera.angle_x += dx * camera.sensitivity * dt;
            camera.angle_y -= dy * camera.sensitivity * dt;

            if(camera.angle_y > M_PI / 2.001) camera.angle_y = M_PI / 2.001;
            if(camera.angle_y < -M_PI / 2.001) camera.angle_y = -M_PI / 2.001;

            camera.dir.x = cos(camera.angle_y) * cos(camera.angle_x);
            camera.dir.y = sin(camera.angle_y);
            camera.dir.z = cos(camera.angle_y) * sin(camera.angle_x);
            camera.dir = normalize(camera.dir);
        }
        break;
      }
    }

    

    float3 forward = normalize(float3(camera.dir.x, 0, camera.dir.z));
    float3 right = normalize(cross(forward, float3(0, 1, 0)));

    if (keys[SDL_SCANCODE_W]) camera.pos += camera.speed * forward * dt;
    if (keys[SDL_SCANCODE_S]) camera.pos -= camera.speed * forward * dt;
    if (keys[SDL_SCANCODE_D]) camera.pos -= camera.speed * right * dt;
    if (keys[SDL_SCANCODE_A]) camera.pos += camera.speed * right * dt;
    if (keys[SDL_SCANCODE_SPACE]) camera.pos += float3(0, camera.speed, 0) * dt;
    if (keys[SDL_SCANCODE_LSHIFT]) camera.pos -= float3(0, camera.speed, 0) * dt;
    

    // Render the scene
    render(camera, pixels.data(), SCREEN_WIDTH, SCREEN_HEIGHT, objects, 1);

    // Update the texture with the pixel buffer
    SDL_UpdateTexture(texture, nullptr, pixels.data(), SCREEN_WIDTH * sizeof(uint32_t));

    // Clear the renderer
    SDL_RenderClear(renderer);

    // Copy the texture to the renderer
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);

    // Update the screen
    SDL_RenderPresent(renderer);
  }

  // Destroy the window. This will also destroy the surface
  SDL_DestroyWindow(window);

  // Quit SDL
  SDL_Quit();

  // End the program
  return 0;
}