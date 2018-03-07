#ifndef RGL_H_INCLUDED
#define RGL_H_INCLUDED

/*!

   RGL, stands for R Graphics Library where R is Rendering Class.

 */

/*
    Include Files
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <vector>
#include <valarray>
#include <algorithm>

#define GLM_SWIZZLE
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#ifdef SDL12
#include <SDL/SDL.h>
#endif // SDL12

#ifdef SDL20
#include <SDL2/SDL.h>
#endif // SDL 2.x

#define WHITE (Colour(255,255,255))
#define BLACK (Colour(0,0,0))
#define RED (Colour(255,0,0))
#define GREEN (Colour(0,255,0))
#define BLUE (Colour(0,0,255))
#define MAGENTA (Colour(255,255,0))
#define CYAN (Colour(255,0,255))
#define YELLOW (Colour(0,255,255))

/*
 * rgba colours as 4 element float vector
 */

#define hWHITE (glm::vec4(1.0f,1.0f,1.0f,1.0f))
#define hBLACK (glm::vec4(0.0f,0.0f,0.0f,1.0f))
#define hRED (glm::vec4(1.0f,0.0f,0.0f,1.0f))
#define hGREEN (glm::vec4(0.0f,1.0f,0.0f,1.0f))
#define hBLUE (glm::vec4(0.0f,0.0f,1.0f,1.0f))
#define hMAGENTA (glm::vec4(1.0f,1.0f,0.0f,1.0f))
#define hCYAN (glm::vec4(1.0f,0.0f,1.0f,1.0f))
#define hYELLOW (glm::vec4(0.0f,1.0f,1.0f,1.0f))


/*
    Default Screen Size, should use const int.
 */

#ifndef HEIGHT
#define HEIGHT 480
#endif
#ifndef WIDTH
#define WIDTH 640
#endif

#define FALSE 0
#define TRUE 1
#define Nothing 0;

#include <parameters.h>

// namespace RGL //rgl2d interface to SDL and the windowing system.
// {
extern Values World;
glm::vec4 _workstation(glm::vec4 a);

#ifdef SDL12

extern SDL_Surface *screen;

void _SET32(int, int, Uint32);
void _SET16(int, int, Uint32);
void _Set(int, int, Uint32);

#endif // SDL12

#ifdef SDL20

extern SDL_Window *screen;     /* global variable for the screen, bad programming practice */
extern SDL_Renderer *renderer;

    #define _Set(x, y, z) SetPixel(x, y, z)
    #define _Set32(x, y, z) SetPixel(x, y, z)
    #define _Set16(x, y, z) SetPixel(x, y, z)

#endif // SDL 2.x

void StartGraphics(char *title);
void StopGraphics(int wait);
bool Animate(float * time);

void SetUp(void);
void Default(void);

void SetPixel(int, int, Uint32);
void SetPixelRGB(int, int, int, int, int);
void _Lock(void);
void _UnLock(void);
void Lock(void);
void UnLock(void);
void SetPixelRGB(int x, int y, int r, int g, int b);
void SetPixel(int x, int y, Uint32 pixel);
void ClearScreen(void);
void Flip(void);
void UpdateScreen(void);
void UpdateWindow(int x, int y, int wx, int wy);

bool Clip(glm::vec2 x);
int Clip(int wxl, int wxr, int wyb, int wyt, int x, int y);

void FilledBox(int wxl, int wxr, int wyb, int wyt, Uint32 colour);
void Box(int wxl, int wxr, int wyb, int wyt, Uint32 colour);
void Block(glm::vec2 p, glm::vec3 c);
void Line(int x0, int y0, int x1, int y1, Uint32 colour);   //draw a real line
void Line(glm::vec2 s, glm::vec2 e, glm::vec3 c);

glm::vec2 interpolatedPosition(glm::vec2 p0, glm::vec2 p1, glm::vec2 p2, glm::vec2 p3, float t);

bool BackFaceCulling(glm::vec4 a, glm::vec4 b, glm::vec4 c);


Uint32 Colour(int r, int g, int b);
Uint32 Colour(float r, float g, float b);
Uint32 Colour(glm:: vec4 cl);
Uint32 Colour(glm:: vec3 cl);

float GetTicks(void);

void dump(void);   //Dump current screen to bmp file
// } // End of 'RGL'



/*
    Helper routines
 */
void print(glm::vec2 x);
void print(glm::mat2 x);
void print(glm::vec3 x);
void print(glm::mat3 x);
void print(glm::vec4 x);
void print(glm::mat4 x);
void print(const char *x);
void print(float x);
void Print(const char *x);
void Print(float x);
void print(int x);


#endif // RGL_H_INCLUDED
