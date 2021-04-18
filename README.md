# RayTracingMazeCppOLC

Simple ray tracing game in C++, originally developed in [Python](https://github.com/FinFetChannel/pytracingMaze). As you may have guessed, things started to get a bit heavy for Python and my poor optimization skills, still, developing the proof of concept with Python was worth it.

## Intro

### Rays

The basic approach is to trace rays of light in the reverse direction, starting from the camera and interacting with the environment, with three basic types of rays:

* Vision rays - Initial ray that shoots from the camera and returns the coordinates where it has hit something
* Reflection rays - Secondary rays that start where the vision ray has hit something and goes in the direction of the light
* Reflection rays - Secondary rays cast when a vision ray hits a reflective surface, the new direction is the reflection of vision ray in relation to the normal vector of the surface, can have several bounces (reflections inside reflections)

### Maps

Maps are defined by grids, with different maps for different features: wall positions, colors, heights, reflectivities, textures and geometry (spheres or prisms). The maps are generated randomly, so each level and each game is a bit different. For that, a random walker algorithm is used, removing some walls from the map while traversing it, giving preference to pre-existing paths carved randomly.

### Game logic

The player starts on one side of the map and has the objective of finding a blue floor patch on the opposite side of the map. At each level the size of map gets bigger, with a total of.

### Inputs

Basic inputs are similar to FPS games, with WASD for movement and mouse for orientation (optionally QERF), the esc key is reserved for quitters.

### Engine

The game is built on top of the [PixelGameEngine](https://github.com/OneLoneCoder/olcPixelGameEngine) from [javidx9](https://www.youtube.com/channel/UC-yuWVUplUJZvieEligKBkA).

## Code

The code is a bit messy, following the basic structure:

* Variables initialization
* Level loop
   * Initialize level - OnUserCreate()
   * Game loop - OnUserUpdate()
      * Check inputs
      * Movement
      * Pixel loop
         * Initialize Vision ray
         * Vision Ray loop
            * Increment until reaching some surface
            * If hit reflective suface, reflect ray direction, else break out
         * Initialize Shadow ray
         * Shadow Ray loop
            * Increment until reaching light or blocked by something
         * Draw pixel
    * Check if reached end of level

Complexity arises from the different branches on block features wich nay be combined (reflectivity, geometry, texture).

### Variables initialization

Several variables have to be initialized related to: maps, player's position and orientation, light position, 

<details>
  <summary>Imports, map and initialization:</summary>

```c++

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

int Wsize; int level;
int Wmap[100][100]; int Rmap[100][100]; float Hmap[100][100]; int Tmap[100][100]; int Smap[100][100];
float Rc[100][100]; float Gc[100][100]; float Bc[100][100];

float playerx = 1.5; float playery = 1.5;
int exitx = 1; int exity = 1;
float lx; float ly;
float playerH = 1.5; float playerV = -.1;
float nx; float ny; float nz; float dot;

float mousex; float mousey;
const int screenwidth = 224;
int Spixel = 4;
const float mod = screenwidth/60;
int sx; int sy; float tr[6][6]; // random texture
float tb[6][4] = {{.95, .99, .97, .78}, // Brick texture
                  {.97, .95, .96, .81},
                  {.82, .81, .83, .78},
                  {.93, .83, .98, .96},
                  {.99, .78, .97, .95},
                  {.81, .78, .82, .82}
                };

```

</details>
