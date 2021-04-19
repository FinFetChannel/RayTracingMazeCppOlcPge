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

class Example : public olc::PixelGameEngine
{
public:
	Example()
	{
		sAppName = "DemoRayTracingMaze";
	}

public:
	bool OnUserCreate() override
	{
		srand (time(NULL));
		for (int x = 0; x < Wsize; x++)
			for (int y = 0; y < Wsize; y++)
            {
                Rmap[x][y] = int(((float) rand()) / (float) RAND_MAX + 0.2);
                if (int(((float) rand()) / (float) RAND_MAX + 0.2))
                    Tmap[x][y] = rand()%2 + 1;
                else
                    Tmap[x][y] = 0;
                Rc[x][y] = rand()%255;
                Gc[x][y] = rand()%255;
                Bc[x][y] = rand()%255;
                if(x == 0 || y == 0 || x == Wsize-1 || y == Wsize-1){
                    Wmap[x][y] = 1; Hmap[x][y] = 1; Smap[x][y] == 0;}
                else{
                    Wmap[x][y] = int(((float) rand()) / (float) RAND_MAX + 0.5);
                    Hmap[x][y] = 0.2 + 0.6*(((float) rand()) / (float) RAND_MAX );
                    Smap[x][y] = int(((float) rand()) / (float) RAND_MAX + 0.2);
                }
            }
        Wmap[int(playerx)][int(playery)] = 0;
        int x = int(playerx); int y = int(playery);
        int cont = 0;
        while (1){
            int testx = x; int testy = y;
            if (((float) rand()) / (float) RAND_MAX > 0.5)
            testx += (rand()%2)*2 - 1;
            else
            testy += (rand()%2)*2 - 1;
            if (testx > 0 & testx < Wsize -1 & testy > 0 & testy < Wsize -1){
                if (Wmap[testx][testy] == 0 || cont > 5){
                    cont = 0; x = testx; y = testy; Wmap[x][y] = 0;
                    if (x == Wsize-2){
                        exitx = x; exity = y;
                        break;
                    }
                }
                else
                    cont += 1;
            }
        }
        for (int x = 0; x < 6; x++)
			for (int y = 0; y < 6; y++)
                tr[x][y] = 0.5 + 0.4*(((float) rand()) / (float) RAND_MAX);
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
	    // user inputs
        if (int(mousex) != float(GetMouseX()))
            playerH += 12*(float(GetMouseX()) - mousex)/ScreenWidth();

        if (int(mousey) != float(GetMouseY()))
            playerV += 3*(float(GetMouseY()) - mousey)/ScreenHeight();

        if (playerV > 0.5)playerV = 0.5;
        if (playerV < -0.5)playerV = -0.5;

        mousex = float(GetMouseX()); mousey = float(GetMouseY());

        if (GetKey(olc::Key::Q).bHeld) // turn left
            playerH += -1* fElapsedTime;

        if (GetKey(olc::Key::E).bHeld) // turn right
            playerH += 1* fElapsedTime;

        if (GetKey(olc::Key::R).bHeld) // turn up
            playerV += 1* fElapsedTime;

        if (GetKey(olc::Key::F).bHeld) // turn down
            playerV += -1* fElapsedTime;

        if (GetKey(olc::Key::ESCAPE).bHeld) // quit
            return 0;

        float px = playerx; float py = playery;
        if (GetKey(olc::Key::W).bHeld) // Forwards
        {
            px += cos(playerH)*2.f * fElapsedTime; py += sin(playerH)*2.f * fElapsedTime;
        }
        if (GetKey(olc::Key::S).bHeld) // Backwards
        {
            px += -cos(playerH)*2.f * fElapsedTime; py += -sin(playerH)*2.f * fElapsedTime;
        }
        if (GetKey(olc::Key::A).bHeld) // Leftwards
        {
            px += sin(playerH)*2.f * fElapsedTime; py += -cos(playerH)*2.f * fElapsedTime;
        }
        if (GetKey(olc::Key::D).bHeld) // Rightwards
        {
            px += -sin(playerH)*2.f * fElapsedTime; py += cos(playerH)*2.f * fElapsedTime;
        }
        if (!Wmap[int(px)][int(py)]){ // only moves if not wall
            playerx = px; playery = py;
        }

        // draw pixel after pixel
		for (int x = 0; x < ScreenWidth(); x++)
			for (int y = 0; y < ScreenHeight(); y++)
            {
                float xx = playerx; int xint;
                float yy = playery; int yint;
                float zz = 0.5;
                float Hangle = playerH + x*0.017453/mod - 0.523598;
                float Vangle = playerV + y*0.017453/mod - 0.393699;
                float dx = cos(Hangle)*0.04/mod;
                float dy = sin(Hangle)*0.04/mod;
                float dz = sin(Vangle)*0.04/mod;
                float shade = 1;
                int r = 255; int g = 255; int b = 255;
                float rr; float rg; float rb;

                while(1)
                {
                    xx += dx;
                    yy += dy;
                    zz += dz;

                    if (zz < 0) // ceiling
                    {
                        if (pow((xx-lx),2) + pow((yy-ly),2) < 0.1){
                            r = 255; g = 255; b = 255;
                            break;
                            }
                        else{
                            float shade2 = 0.25 * (abs(sin(yy+ly)+ sin(xx+lx))+2);
                            r = 255*shade2; g = 255*shade2; b = 255;
                            break;
                        }
                    }
                    if (zz > 1) // floor
                    {
                        if (int(2*xx)%2 == int(2*yy)%2){
                            if (int(xx) == exitx & int(yy) == exity){
                                r = 0; g = 0; b = 255;}
                            else{
                                r = 10; g = 10; b = 10;}
                        }
                        else{
                            r = 200; g = 230; b = 210;}
                        break;
                    }
                    if (Wmap[int(xx)][int(yy)]) // walls
                    {
                        if (Hmap[int(xx)][int(yy)] >= 1-zz)
                        {
                            if (Smap[int(xx)][int(yy)])// Spheres
                            {
                                if (pow(xx-int(xx)-0.5,2)+pow(yy-int(yy)-0.5,2)+pow(zz-int(zz)-0.5,2) < 0.25)
                                {
                                    if (Rmap[int(xx)][int(yy)]) // spherical mirrors
                                    {
                                        if (shade == 1){
                                            rr = Rc[int(xx)][int(yy)]; rg = Gc[int(xx)][int(yy)]; rb = Bc[int(xx)][int(yy)];} // tinted mirrors
                                        else{
                                            rr = 0.5*(rr + Rc[int(xx)][int(yy)]); rg = 0.5*(rg + Gc[int(xx)][int(yy)]); rb = 0.5*(rb + Bc[int(xx)][int(yy)]);}
                                        shade = shade*0.7;
                                        if (shade < 0.1){
                                            r = 0; g = 0; b = 0;
                                            break;
                                        }
                                        if (abs(Hmap[int(xx)][int(yy)] - 1+zz) <= abs(dz)) // horizontal surface
                                            dz = -dz;
                                        else{
                                            nx = (xx-int(xx)-0.5)/0.5; ny = (yy-int(yy)-0.5)/0.5; nz =(zz-0.5)/0.5;
                                            dot = 2*(dx*nx + dy*ny + dz*nz); // dR = -dI + 2*n*(dI·n)
                                            dx = (dx - nx*dot)*1.2; dy = (dy - ny*dot)*1.2; dz = (dz - nz*dot)*1.2;
                                        }
                                    }
                                    else
                                    {
                                        r = Rc[int(xx)][int(yy)]; g = Gc[int(xx)][int(yy)]; b = Bc[int(xx)][int(yy)];
                                        if (Tmap[int(xx)][int(yy)] != 0) // textures on spheres (a bit wonky)
                                        {
                                            if (yy - int(yy) < 0.05 || yy - int(yy) > 0.95)
                                                sx = int((xx*3 - int(3*xx))*4);
                                            else
                                                sx = int((yy*3 - int(3*yy))*4);
                                            if (xx - int(xx) < 0.95 & xx - int(xx) > 0.05 & yy - int(yy) < 0.95 & yy - int(yy) > 0.05)
                                                sy = int((xx*5 - int(5*xx))*6);
                                            else
                                                sy = int((zz*5 - int(5*zz))*6);
                                            if (Tmap[int(xx)][int(yy)] == 2){
                                                r = r*tr[sy][sx]; g = g*tr[sy][sx]; b = b*tr[sy][sx];
                                            }
                                            else{
                                                r = r*tb[sy][sx]; g = g*tb[sy][sx]; b = b*tb[sy][sx];
                                            }
                                        }
                                        break;
                                    }
                                }
                            }
                            else
                            {
                                if (Rmap[int(xx)][int(yy)]) // reflections
                                {
                                    if (shade == 1){
                                        rr = Rc[int(xx)][int(yy)]; rg = Gc[int(xx)][int(yy)]; rb = Bc[int(xx)][int(yy)];} // tinted mirrors
                                    else{
                                        rr = 0.5*(rr + Rc[int(xx)][int(yy)]); rg = 0.5*(rg + Gc[int(xx)][int(yy)]); rb = 0.5*(rb + Bc[int(xx)][int(yy)]);}
                                    shade = shade*0.7;
                                    if (shade < 0.1){
                                        r = 0; g = 0; b = 0;
                                        break;
                                    }
                                    if (abs(Hmap[int(xx)][int(yy)] - 1+zz) <= abs(dz)) // horizontal surface
                                        dz = -dz;
                                    else{
                                        if (Hmap[int(xx+dx)][int(yy-dy)] == Hmap[int(xx)][int(yy)])
                                            dx = -dx; // y surface
                                        else
                                            dy = -dy; // x surface
                                    }
                                }
                                else
                                {
                                    r = Rc[int(xx)][int(yy)]; g = Gc[int(xx)][int(yy)]; b = Bc[int(xx)][int(yy)]; // regular surface
                                    if (Tmap[int(xx)][int(yy)] != 0)
                                    {
                                        if (yy - int(yy) < 0.05 || yy - int(yy) > 0.95)
                                            sx = int((xx*3 - int(3*xx))*4);
                                        else
                                            sx = int((yy*3 - int(3*yy))*4);
                                        if (xx - int(xx) < 0.95 & xx - int(xx) > 0.05 & yy - int(yy) < 0.95 & yy - int(yy) > 0.05)
                                            sy = int((xx*5 - int(5*xx))*6);
                                        else
                                            sy = int((zz*5 - int(5*zz))*6);
                                        if (Tmap[int(xx)][int(yy)] == 2){
                                            r = r*tr[sy][sx]; g = g*tr[sy][sx]; b = b*tr[sy][sx];
                                        }
                                        else{
                                            r = r*tb[sy][sx]; g = g*tb[sy][sx]; b = b*tb[sy][sx];
                                        }
                                    }
                                break;
                                }
                            }
                        }

                    }
                }

                float dl = sqrt(pow ((xx-lx),2) + pow((yy-ly),2) + pow((0-zz),2) );
                if (shade < 1){ // tinted mirrors application
                    //r = 0.5*(rr + r); rg = 0.5*(rg + g); rb = 0.5*(rb + b); // colorful black tiles
                    r = sqrt(rr * r); rg = sqrt(rg * g); rb = sqrt(rb * b);
                }
                if (zz>0) // shade ray for everything thats under the ceiling level
                {
                    dx = 0.04*(lx-xx)/dl; dy = 0.04*(ly-yy)/dl; dz = 0.04*(0-zz)/dl; // light direction
                    while(1)
                    {
                        xx += dx; yy += dy; zz += dz;
                        if (Wmap[int(xx)][int(yy)] & Hmap[int(xx)][int(yy)] >= 1-zz)
                            if (!Smap[int(xx)][int(yy)] || (Smap[int(xx)][int(yy)] & (pow(xx-int(xx)-0.5,2)+pow(yy-int(yy)-0.5,2)+pow(zz-int(zz)-0.5,2) < 0.25)))
                                shade = shade*0.9;
                        if (zz<0 || shade<0.4)
                            break;
                    }
                }
                shade = sqrt(shade*(0.4 + 0.6)/(dl/2+0.1));
                //shade = shade* (1+0.2/sqrt(pow(playerx-xx,2)+pow(playery-yy,2)+pow(0.5-zz,2)));
                if (shade > 1)
                    shade  = 1;
                Draw(x, y, olc::Pixel(int(shade*r),int(shade*g), int(shade*b)));
            }
        if (int(playerx) == exitx & int(playery) == exity)
            return false;
        DrawString({ 10,10 }, std::to_string(level), olc::YELLOW);
		return true;
	}
};

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
