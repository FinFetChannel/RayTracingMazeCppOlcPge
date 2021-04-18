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

Several variables have to be initialized related to: maps, player's position and orientation, light position and additional support variables.

<details>
  <summary>Imports, map and initialization:</summary>

```c++

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

int Wsize; int level; // map size, current level
int Wmap[100][100]; int Rmap[100][100]; float Hmap[100][100]; int Tmap[100][100]; int Smap[100][100];
float Rc[100][100]; float Gc[100][100]; float Bc[100][100]; // RGB maps

float playerx = 1.5; float playery = 1.5;
int exitx = 1; int exity = 1;
float lx; float ly;
float playerH = 1.5; float playerV = -.1; // player orientation
float nx; float ny; float nz; float dot; // normal vector

float mousex; float mousey; // mouse position
const int screenwidth = 224;
int Spixel = 4; // pixel scaling on screen
const float mod = screenwidth/60; // pixel scaler in field of view (60°)
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


### Main() and levels

The main function is responsible for the level management, with a for loop that increases the size of the map at each turn. The player can only advance to the next level if its last position is equal to the last exit position. The light position is always set to the middle of the level and the player's position is reset.

<details>
  <summary>Main() and levels</summary>

```c++
int main()
{
	for (int x = 0; x < 10; x++)
    {
        if (int(playerx) == exitx & int(playery) == exity)
        {
            level = x + 1;
            Wsize = level*10;
            playerx = 1.5; playery = 1.5;
            lx = Wsize/2; ly = Wsize/2;
            Example demo;
            if (demo.Construct(screenwidth, int(screenwidth*0.75), Spixel, Spixel))
                demo.Start();
        }

    }
	return 0;
}
```

</details>

### Setting up a level

The setup of a level is the process of generating a map of a given size. Firstly, a map is generated with blocks in random locations and random features, except on the edges of the map, where there are always full-height prismatic walls. After that a random walker tries to reach the opposite side of the map, in the process some blocks are removed to give way.

<details>
  <summary>Imports, map and initialization:</summary>

```c++
bool OnUserCreate() override
	{
	srand (time(NULL));
	for (int x = 0; x < Wsize; x++)
		for (int y = 0; y < Wsize; y++)
           	{
                	Rmap[x][y] = int(((float) rand()) / (float) RAND_MAX + 0.2); // Reflective?
			
                	if (int(((float) rand()) / (float) RAND_MAX + 0.2)) // Textured?
                   		Tmap[x][y] = rand()%2 + 1;
                	else
                    		Tmap[x][y] = 0;
				
               		Rc[x][y] = rand()%255; Gc[x][y] = rand()%255; Bc[x][y] = rand()%255; // RGB
			
                	if(x == 0 || y == 0 || x == Wsize-1 || y == Wsize-1){
                    		Wmap[x][y] = 1; Hmap[x][y] = 1; Smap[x][y] == 0;}
                	else
			{
                    		Wmap[x][y] = int(((float) rand()) / (float) RAND_MAX + 0.5);
                    		Hmap[x][y] = 0.2 + 0.6*(((float) rand()) / (float) RAND_MAX );
                    		Smap[x][y] = int(((float) rand()) / (float) RAND_MAX + 0.2);
                	}
            	}
	
        Wmap[int(playerx)][int(playery)] = 0; // Remove wall fron starting position
	
        int x = int(playerx); int y = int(playery);  int cont = 0;
	
        while (1){
            int testx = x; int testy = y;
	    
            if (((float) rand()) / (float) RAND_MAX > 0.5)
            	testx += (rand()%2)*2 - 1;
            else
            	testy += (rand()%2)*2 - 1;
		
            if (testx > 0 & testx < Wsize -1 & testy > 0 & testy < Wsize -1){
                if (Wmap[testx][testy] == 0 || cont > 5){ // move to new position if not wall or counter reached limit
                    cont = 0; x = testx; y = testy; Wmap[x][y] = 0;
                    if (x == Wsize-2){
                        exitx = x; exity = y; // set exit of the maze
                        break;
                    }
                }
                else
                    cont += 1; // increase counter if cannot move
            }
        }
        for (int x = 0; x < 6; x++)
			for (int y = 0; y < 6; y++)
                tr[x][y] = 0.5 + 0.4*(((float) rand()) / (float) RAND_MAX);
		return true;
	}
```

</details>



<details>
  <summary>Imports, map and initialization:</summary>

```c++

```

</details>



<details>
  <summary>Imports, map and initialization:</summary>

```c++

```

</details>
