#include "RGL.h"

#include <version.h>

/*
 *
 * This uses SDL 2.x, x is 0 currently
 *
 */

// namespace RGL
// {

glm::vec4 __center; // DD parameters
glm::mat4 __base;
glm::mat4 __scale;


glm::vec4 _workstation(glm::vec4 a)
{
  // converts DI coordinates to screen coordinates. This needs to be modifed to work with variable position camera.
  // assume input range is -1.0 to 1.0
  glm::vec4 x, y;
  x = (a * __scale);
  y = x + __center;
//    y[2] = a[2]; // fix for DDC, and fixed camera
  y[3] = 1.;
  return y;
}

void SetUp()
{
  /*
     These need integrating and modifying for a variable camera position
   */
  __center = glm::vec4 ((float) WIDTH / 2., (float) HEIGHT / 2., 0.0, 0.0);   // DD parameters
  __base = glm::mat4 (1.);
  float fscale = ((float) HEIGHT) / 4.;   // needed because of poor macro expansion.
  __scale = glm::mat4 (fscale);
  __scale[2][2] = 1.f;
  __scale[1][1] = -__scale[1][1];
}


SDL_Window *screen; /* global variable for the screen, bad programming practice */
SDL_Renderer *renderer = NULL;

Values World;

/** \brief
 *
 * \return void
 *
 * This is the exit function called when SDL is forced to quit.
 */
void exitfunc()
{
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(screen);
  SDL_Quit();
  HELP::print("SDL - Quit");
}

/** \brief
 *
 * \param void
 * \return void
 *
 * Function called to set the default values in the World structure. If you add to the structure, add an entry here.
 */
void Default(void)
{
  World.DefaultColour = Colour(0, 0, 0);
  World.BackFaceCulling = false;
  World.ZBuffer = false;
  World.ShadeType = Flat;
  World.LightCount = 0;   // No lights, use material directly.
  World.lod = 0.05;
  World.CameraVector = glm::vec4(0., 0., 1., 0.);
}

/** \brief
 *
 * \param x int
 * \param y int
 * \param c Uint32
 * \return void
 *
 * Internal function for fast write to 16 bit screen
 */
/** \brief
 *
 * \param x int
 * \param y int
 * \param c Uint32
 * \return void
 *
 * Writes to screen, makimg the choice as specified by #define RASPBERRY_PI
 */

/** \brief
 *
 * \param x int
 * \param y int
 * \param r int
 * \param g int
 * \param b int
 * \return void
 *
 * Generic write RGB value to screen.
 */
void SetPixelRGB(int x, int y, int r, int g, int b)
{
  SetPixel(x, y, Colour(r, g, b));
}

/** \brief
 *
 * \param x int
 * \param y int
 * \param pixel Uint32
 * \return void
 *
 * Write colour value to screen, checking format.
 */

void SetPixel(int x, int y, Uint32 pixel)   // Clumsy to retain compatibility with SDL 1.2
{
  int b = pixel & 0xFF;
  int g = (pixel >> 8) & 0xFF;
  int r = (pixel >> 16) & 0xFF;
  SDL_SetRenderDrawColor(renderer, r, g, b, 255);
  SDL_RenderDrawPoint(renderer, x, y);
}
/** \brief
 *
 * \param r int
 * \param g int
 * \param b int
 * \return Uint32
 *
 * convert RGC to internal colour format
 */
Uint32  Colour(int r, int g, int b)   /* generate a colour value */
{
  //    return SDL_MapRGB(screen->format, (Uint8) r, (Uint8) g, (Uint8) b);
  // ignore internal format, this will go away and use a colour class.
  return (r << 16) | (g << 8) | b;
}
/*
 * convert RGC to internal colour format
 */
Uint32  Colour(float fr, float fg, float fb)   /* generate a colour value */
{
  //    return SDL_MapRGB(screen->format, (Uint8) r, (Uint8) g, (Uint8) b);
  // ignore internal format, this will go away and use a colour class.
  int r = (int)(255. * fr);
  int g = (int)(255. * fg);
  int b = (int)(255. * fb);

  return (r << 16) | (g << 8) | b;
}

/** \brief
 *
 * \param cl glm::vec4
 * \return Uint32
 *
 * Convert RGBA vector to internal colour format
 */
Uint32  Colour(glm::vec4 cl)   /* generate a colour value */
{
  float r = 255. * cl[0];
  float g = 255. * cl[1];
  float b = 255. * cl[2];
//    return SDL_MapRGB(screen->format, (Uint8) r, (Uint8) g, (Uint8) b);
  return ((int) r << 16) | ((int) g << 8) | (int) b;

}
Uint32  Colour(glm::vec3 cl)   /* generate a colour value */
{
  float r = 255. * cl[0];
  float g = 255. * cl[1];
  float b = 255. * cl[2];
//    return SDL_MapRGB(screen->format, (Uint8) r, (Uint8) g, (Uint8) b);
  return ((int) r << 16) | ((int) g << 8) | (int) b;

}


/** \brief
 *
 * \param void
 * \return void
 *
 * Update the screen
 */

void Draw_all(void)
{
  SDL_RenderPresent(renderer);
}

void  StopGraphics(int wait)
{
  SDL_Event test_event;
  SDL_RenderPresent(renderer);
  if(wait)     // if wait is false exit directly
  {
    while(wait)
    {
      while(SDL_PollEvent(&test_event))         /* wait for quit event */
      {
        switch(test_event.type)
        {
        case SDL_KEYDOWN:
          //~ print("Key event");
          //~ print((int) test_event.key.keysym.sym);
          if(test_event.key.keysym.sym == SDLK_q)
          {
            wait = false;
          }
          if(test_event.key.keysym.sym == SDLK_ESCAPE)
          {
            wait = false;
          }
//                    if(test_event.key.keysym.sym == SDLK_p) {
//                        Dump();
//                    }

          break;           // SDL_KEYDOWN
        case SDL_QUIT:
          wait = FALSE;
          break;           // SDL_QUIT
        default:
          break;
        }
      }
    }
  }
  else
    wait = true;
//    exitfunc();
  return;
}

bool  Animate(float *time)   //, int *key_back) {
{
  SDL_Event test_event;
  bool wait;
  Draw_all();
  wait = true;
  *time = ((float) SDL_GetTicks()) / 1000.;   // return time in seconds.
  if(!SDL_PollEvent(&test_event))   // check for event
    return true;
  wait = true;
  switch(test_event.type)
  {
  case SDL_KEYDOWN:
//        *key_back = test_event.key.keysym.sym;
    if(test_event.key.keysym.sym == SDLK_q || test_event.key.keysym.sym == SDLK_ESCAPE)
    {
      wait = false;
    }
//        if(test_event.key.keysym.sym == SDLK_p) {
//            Dump();
//            return true;
//        }

    break;     // SDL_KEYDOWN
  case SDL_QUIT:   // should be a less brutal exit i.e. esc, q, Q or END
    wait = false;
    break;     // SDL_QUIT
  default:
    return true;
  }
  return wait;
}



void StartGraphics(char *title)   //Start up SDL and other stuff
{
  int w = WIDTH;
  int h = HEIGHT;
  int desired_bpp = 0;
  HELP::print("SDL - Startup");
  HELP::print(version_string);
  atexit(exitfunc);
  if ( SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_JOYSTICK) < 0 )
  {
    fprintf(stderr,
            "Couldn't initialize SDL: %s\n", SDL_GetError());
    exit(1);
  }
  SDL_JoystickEventState(SDL_ENABLE);


  /* Initialize the display */
  screen = SDL_CreateWindow(title,
                            SDL_WINDOWPOS_UNDEFINED,
                            SDL_WINDOWPOS_UNDEFINED,
                            WIDTH, HEIGHT,
                            (Uint32) 0);


  if ( screen == NULL )
  {
    fprintf(stderr, "Couldn't set Window: %s\n", SDL_GetError());
    exitfunc();
  }

  renderer = SDL_CreateRenderer(screen, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

  if ( renderer == NULL )
  {
    fprintf(stderr, "Couldn't set renderer %s\n", SDL_GetError());
    exitfunc();
  }

  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
  Default();
  World.DefaultColour = Colour(255, 255, 255);
  HELP::print("SDL - Started");
  SetUp();
}

//~ void  Title(char * text) { //not working.
//~ SDL_SetWindowTitle(screen, text);
//~ }

void  ClearScreen()
{
  int pixel = World.DefaultColour;
  int b = pixel & 0xFF;
  int g = (pixel >> 8) & 0xFF;
  int r = (pixel >> 16) & 0xFF;
  int e1 = SDL_SetRenderDrawColor(renderer, r, g, b, 255);
  int e2 = SDL_RenderClear(renderer);
//    if(World.ZBuffer) {
//        for(int x = 0; x<WIDTH; x++)
//            for(int y = 0; y < HEIGHT; y++)
//                ZBuffer[x][y] = 1000000.0;
//    }
}

void Dump(void)
{
  SDL_Surface *sshot = SDL_CreateRGBSurface(0, WIDTH, HEIGHT, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
  int ret = SDL_RenderReadPixels(renderer, NULL, SDL_PIXELFORMAT_ARGB8888, sshot->pixels, sshot->pitch);
  if(ret)
    SDL_GetError();
  SDL_SaveBMP(sshot, "screenshot.bmp");
  SDL_FreeSurface(sshot);
}

void Dump(const char *fn)
{
  SDL_Surface *sshot = SDL_CreateRGBSurface(0, WIDTH, HEIGHT, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
  int ret = SDL_RenderReadPixels(renderer, NULL, SDL_PIXELFORMAT_ARGB8888, sshot->pixels, sshot->pitch);
  if(ret)
    SDL_GetError();
  SDL_SaveBMP(sshot, fn);
  SDL_FreeSurface(sshot);
}
void  Lock()
{
  return;
}
void  UnLock()
{
  return;
}
void  Flip()
{
  return;
}

float GetTicks(void)
{
  return (float) SDL_GetTicks();
}


// } // End of 'RGL'
