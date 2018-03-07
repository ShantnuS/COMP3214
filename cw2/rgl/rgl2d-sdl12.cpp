#include "RGL.h"

/*
 *
 * Uses SDL 1.2
 *
 */
// namespace RGL {

glm::vec4 __center; // DD parameters
glm::mat4 __base;
glm::mat4 __scale;


glm::vec4 _workstation(glm::vec4 a) {
  // converts DI coordinates to screen coordinates. This needs to be modifed to work with variable position camera.
  // assume input range is -1.0 to 1.0
  glm::vec4 x, y;
  x = (a * __scale);
  y = x + __center;
//    y[2] = a[2]; // fix for DDC, and fixed camera
  y[3] = 1.;
  return y;
}

void SetUp(){
  /*
     These need integrating and modifying for a variable camera position
   */
  __center = glm::vec4 ((float) WIDTH / 2., (float) HEIGHT / 2., 0.0, 0.0);   // DD parameters
  __base = glm::mat4 (1.);
  float fscale = ((float) HEIGHT) / 4.;   // needed because of poor macro expansion.
  __scale = glm::mat4 (fscale);
  __scale[2][2] = 1.f;
  __scale[1][1] = -__scale[1][1];
  World.zBuffer = (float *) calloc(WIDTH * HEIGHT, sizeof(float));   // allocate abd zero
  printf("Z-Buffer Created %d\n", World.Size);
}


SDL_Surface *screen;   /* global variable for the screen, bad programing practice */
//int buffer[HEIGHT / 8][WIDTH / 8];
int dumpit = 0;
Values World;
/** \brief
 *
 * \return void
 *
 * This is the exit function called when SDL is forced to quit.
 */
void exitfunc() {
  free(World.zBuffer);
  SDL_Quit();
  print("SDL - Quit");
}

/** \brief
 *
 * \param void
 * \return void
 *
 * Function called to set the default values in the World structure. If you add to the structure, add an entry here.
 */
void Default(void) {
  World.DefaultColour = Colour(0, 0, 0);
  World.CameraVector = glm::vec4(0., 0., 1., 0.);
  World.BackFaceCulling = false;
  World.ZBuffer =  false;
  World.Size = WIDTH * HEIGHT;
  World.MaxRange = -1.0e9;   // compromise on range.
  World.ShadeType = Flat;
  World.LightCount = 0;   // No lights, use material directly.
}

/** \brief
 *
 * \return void
 *
 * Internal function to lock the SDL screen.
 */
void _Lock() {   /* Lock the screen for direct access to the pixels */

  if ( SDL_MUSTLOCK(screen) ) {
    if ( SDL_LockSurface(screen) < 0 ) {
      fprintf(stderr, "Can't lock screen: %s\n", SDL_GetError());
      return;
    }
  }
}
/** \brief
 *
 * \return void
 *
 * Internal function to unlock the screen
 */
void _UnLock() {   /* Unlock and update the whole screen */
  if ( SDL_MUSTLOCK(screen) ) {
    SDL_UnlockSurface(screen);
  }
  /* Update just the part of the display that we've changed */
  SDL_UpdateRect(screen, 0, 0, 0, 0);
}

/** \brief
 *
 * \param void
 * \return void
 *
 * Lock the screen
 */
void Lock(void) {
  _Lock();
}
/** \brief
 *
 * \param void
 * \return void
 *
 * Unlock the screen
 */
void UnLock(void) {
  _UnLock();
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
void _SET16(int x, int y, Uint32 c) {   /* sets pixel if 16-bit display */
  Uint8 *p = (Uint8 *)screen->pixels + y * screen->pitch + x * 2;
  *(Uint16 *)p = c;
}

/** \brief
 *
 * \param x int
 * \param y int
 * \param c Uint32
 * \return void
 *
 * iNTERNAL FUNCTION TO WRITE 32 BIT SCREEN
 */
void _SET32(int x, int y, Uint32 c) {   /* sets pixel if 32-bit display */
  Uint8 *p = (Uint8 *)screen->pixels + y * screen->pitch + x * 4;
  *(Uint32 *)p = c;
}
/** \brief
 *
 * \param x int
 * \param y int
 * \param c Uint32
 * \return void
 *
 * Writes to screen, makimg the choice as specified by #define RASPBERRY_PI
 */
void _Set(int x, int y, Uint32 c) {   /* sets pixel if 32-bit display or 16-bit */
#ifdef RASPBERRY_PI
  _SET16(x, y, c);
#else
  _SET32(x, y, c);
#endif
}

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
void SetPixelRGB(int x, int y, int r, int g, int b)    {
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
void SetPixel(int x, int y, Uint32 pixel) {   /* set a single pixel */
  /* int bpp = screen->format->BytesPerPixel; */
  /* Here p is the address to the pixel we want to set */
  //Uint16 *p = (Uint16 *) ((Uint8 *)screen->pixels + y * screen->pitch + x * 2);
  //*p = pixel;
  //return;
  int bpp = screen->format->BytesPerPixel;
  /* Here p is the address to the pixel we want to set */
  Uint8 *p = (Uint8 *)screen->pixels + y * screen->pitch + x * bpp;
  switch(bpp) {
  case 1:
    *p = pixel;
    break;

  case 2:
    *(Uint16 *)p = pixel;
    break;
  case 3:
    if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
      p[0] = (pixel >> 16) & 0xff;
      p[1] = (pixel >> 8) & 0xff;
      p[2] = pixel & 0xff;
    }
    else {
      p[0] = pixel & 0xff;
      p[1] = (pixel >> 8) & 0xff;
      p[2] = (pixel >> 16) & 0xff;
    }
    break;

  case 4:
    *(Uint32 *)p = pixel;
    break;
  }
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
Uint32 Colour(int r, int g, int b) {   /* generate a colour value, input 0 -> 255 */
  return SDL_MapRGB(screen->format, (Uint8) r, (Uint8) g, (Uint8) b);
}

/** \brief
 *
 * \param r float
 * \param g float
 * \param b float
 * \return Uint32
 *
 * convert RGC to internal colour format
 */
Uint32 Colour(float r, float g, float b) {   /* generate a colour value, input 0 -> 1.0 */
  Uint8 ur, ug, ub;
  ur = (Uint8) (r * 255.f);
  ug = (Uint8) (g * 255.f);
  ub = (Uint8) (b * 255.f);
//    printf("%d %d %d\n", ur, ug, ub);
  return SDL_MapRGB(screen->format, ur, ug, ub);
}

/** \brief
 *
 * \param cl glm::vec4
 * \return Uint32
 *
 * Convert RGBA vector to internal colour format
 */
Uint32 Colour(glm::vec4 cl) {   /* generate a colour value */
  float r = 255. * cl[0];
  float g = 255. * cl[1];
  float b = 255. * cl[2];
  return SDL_MapRGB(screen->format, (Uint8) r, (Uint8) g, (Uint8) b);
}
Uint32 Colour(glm::vec3 cl) {   /* generate a colour value */
  float r = 255. * cl[0];
  float g = 255. * cl[1];
  float b = 255. * cl[2];
  return SDL_MapRGB(screen->format, (Uint8) r, (Uint8) g, (Uint8) b);
}


/** \brief
 *
 * \param void
 * \return void
 *
 * Update the screen
 */
void UpdateScreen(void) {
  SDL_UpdateRect(screen, 0, 0, 0, 0);   // Update whole screen
}

void UpdateWindow(int x, int y, int wx, int wy) {
  SDL_UpdateRect(screen, 0, 0, 0, 0);   // Update whole screen
}

void StopGraphics(int wait) {
  UnLock();
  SDL_Event test_event;
  if(wait) {   // if wait is false exit directly
    while(wait) {
      while(SDL_PollEvent(&test_event)) {               /* wait for quit event */
        switch(test_event.type) {
        case SDL_KEYDOWN:
          //~ print("Key event");
          //~ print((int) test_event.key.keysym.sym);
#ifndef RASPBERRY_PI
          if(test_event.key.keysym.sym == SDLK_q) {
            wait = false;
          }
          if(test_event.key.keysym.sym == SDLK_ESCAPE) {
            wait = false;
          }
          if(test_event.key.keysym.sym == SDLK_p) {
            dump();
          }

#endif
#ifdef RASPBERRY_PI
          if(test_event.key.keysym.sym == SDLK_BACKSPACE) {
            wait = false;
          }
          if(test_event.key.keysym.sym == SDLK_KP_MULTIPLY) {
            dump();
          }

#endif
          break;                               // SDL_KEYDOWN
        case SDL_QUIT:
          wait = FALSE;
          break;                               // SDL_QUIT
        default:
          break;
        }
      }
    }
  }
  else
    wait = true;
  SDL_Quit();
  return;
}

float GetTime(void){
  return ((float) SDL_GetTicks()) / 1000.;
}

bool Animate(float *time) {
  /*
   * Lock stratagey needed for SDL12 on PC etc.
   */
  SDL_Event test_event;
  bool wait;
  wait = true;
  *time = ((float) SDL_GetTicks()) / 1000.;   // return time in seconds.
  if(!SDL_PollEvent(&test_event))   // check for event
    return true;
  wait = true;
  switch(test_event.type) {
  case SDL_KEYDOWN:
#ifndef RASPBERRY_PI
    if(test_event.key.keysym.sym == SDLK_q || test_event.key.keysym.sym == SDLK_ESCAPE) {
      wait = false;
    }
    if(test_event.key.keysym.sym == SDLK_p) {
      dump();
      return true;
    }
#endif

#ifdef RASPBERRY_PI
    if(test_event.key.keysym.sym == SDLK_BACKSPACE) {
      wait = false;
      break;
    }
    else if(test_event.key.keysym.sym == SDLK_KP_MULTIPLY) {
      dump();
      return true;
    }
    else {
      return true;
    }
#endif
    break;             // SDL_KEYDOWN
  case SDL_QUIT:           // should be a less brutal exit i.e. esc, q, Q or END
    wait = false;
    break;             // SDL_QUIT
  default:
    return true;
  }
  return wait;
}



//unsigned int my_callbackfunc(unsigned int interval, void *param) {
//    SDL_Event event;
//    SDL_UserEvent userevent;
//
//    /* In this example, our callback pushes an SDL_USEREVENT event
//    into the queue, and causes ourself to be called again at the
//    same interval: */
//    userevent.type = SDL_USEREVENT;
//    userevent.code = 0;
//    userevent.data1 = NULL;
//    userevent.data2 = NULL;
//
//    event.type = SDL_USEREVENT;
//    event.user = userevent;
//
//    SDL_PushEvent(&event);
//    return(interval);
//    }

void StartGraphics(char *title) {   //Start up SDL and other stuff
  int w = WIDTH;
  int h = HEIGHT;
  int desired_bpp = 0;
  print("SDL - Startup");
  atexit(exitfunc);
  Uint32 video_flags = SDL_DOUBLEBUF | SDL_HWSURFACE | SDL_FULLSCREEN;
  if ( SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_JOYSTICK) < 0 ) {
    fprintf(stderr,
            "Couldn't initialize SDL: %s\n", SDL_GetError());
    exit(1);
  }
  SDL_JoystickEventState(SDL_ENABLE);
//    joystick = SDL_JoystickOpen(0);

  atexit(SDL_Quit);             /* Clean up on exit */

  /* Initialize the display */
  screen = SDL_SetVideoMode(w, h, desired_bpp, 0);
  if ( screen == NULL ) {
    fprintf(stderr, "Couldn't set %dx%dx%d video mode: %s\n",
            w, h, desired_bpp, SDL_GetError());
    exit(1);
  }
  /* Set the window manager title bar */
  SDL_WM_SetCaption(title, "testwin");
  Default();
  World.DefaultColour = Colour(255, 255, 255);
//    my_timer_id = SDL_AddTimer(20, my_callbackfunc, NULL);
//    printf("Timer started %d\n", (int) my_timer_id);
  print("SDL - Started");
  SetUp();
  Lock();
}


void ClearScreen() {
//    _Lock();
  SDL_FillRect(screen, 0, World.DefaultColour);
  //~ memset(World.zBuffer, 0, WIDTH * HEIGHT * sizeof(float) / 4.; // sets zbuffer tozero, no required
  for(int i = 0; i < World.Size; i++)
    World.zBuffer[i] = World.MaxRange;
//    _UnLock();
}
void Flip(void) {
  SDL_Flip(screen);
}

void dump(void) {
  _Lock();
  SDL_SaveBMP(screen, "dump.bmp");
  _UnLock();
}

float GetTicks(void){
  return (float) SDL_GetTicks();
}

// }
