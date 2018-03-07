#include "RGL.h"
/*

   This is a change

   /*
   /* helper routines, not exposed. */

/** \brief
 *
 * \param x int*
 * \param y int*
 * \return void
 *
 * Swap two integers
 */
void iSwap( int *x, int *y)
{
  int t;
  t = *x;
  *x = *y;
  *y = t;
}
void fSwap( float *x, float *y)
{
  float t;
  t = *x;
  *x = *y;
  *y = t;
}


/* ------------------- Use SetPixel below this line---------------------------------------- */

/*

   Unsafe circle algorithm, does not clip to window, so probably will crash s over writes scren.

 */

static int _cx, _cy; //global for center pf the circle

void _CirclePoint(int x, int y, Uint32 colour)
{
  //draw point with sumetry
  SetPixel(_cx + x, _cx + y, colour);
  SetPixel(_cx + y, _cx + x, colour);
  SetPixel(_cx + x, _cx - y, colour);
  SetPixel(_cx + y, _cx - x, colour);
  SetPixel(_cx - x, _cx + y, colour);
  SetPixel(_cx - y, _cx + x, colour);
  SetPixel(_cx - x, _cx - y, colour);
  SetPixel(_cx - y, _cx - x, colour);
}


void _Circle(int R, Uint32 colour)
{
  /*
       Draw a real Circle
       Assume centre at origin
       R is the radius
   */

  int x, y, d;
//    _cx = xc;
//    _cy = yc; /* fudge to transform the origion */
  x = 0;
  y = R;
  d = 1 - R;
  _CirclePoint(x, y, colour);
  while ( y > x)
  {
    if (d < 0 )
    {
      /* Select E */
      d = d + 2 * x + 3;
      x = x + 1;
    }
    else
    {
      d = d + 2 * (x - y) + 5;
      x = x + 1;
      y = y - 1;
    }
    _CirclePoint(x, y, colour);
  }
}



// namespace LINES
//{


void Line(int x0, int y0, int x1, int y1, Uint32 colour)   //draw a real line
{
  int dx, dy, incrE, incrNE, d, x, y, x_end;
  int incrN, incrEN, y_end;
  int Xinc, Yinc;
  dx = x1 - x0;
  dy = y1 - y0;
  if(abs(dx) > abs(dy))
  {
    if(dx<0)
    {
      iSwap(&x0, &x1);
      iSwap(&y0, &y1);
    }
    if(y1>y0)
      Yinc = 1;
    else
      Yinc = -1;

    dx = abs(dx);
    dy = abs(dy);

    d = 2 * dy - dx;
    incrE = 2 * dy;
    incrNE = 2 * (dy - dx);

    x_end = x1;
    x = x0;
    y = y0;

    SetPixel(x, y, colour);
    while(x < x_end)
    {
      if( d <= 0 )
      {
        d = d + incrE;
        x = x + 1;
      }
      else
      {
        d = d + incrNE;
        x = x + 1;
        y = y + Yinc;
      }
      SetPixel(x, y, colour);
    }
  }
  else
  {
    if(dy<0)
    {
      iSwap(&x0, &x1);
      iSwap(&y0, &y1);
    }
    if(x1>x0)
      Xinc = 1;
    else
      Xinc = -1;

    dx = abs(dx);
    dy = abs(dy);

    d = 2 * dx - dy;
    incrN = 2 * dx;
    incrEN = 2 * (dx - dy);

    y_end = y1;
    x = x0;
    y = y0;

    SetPixel(x, y, colour);
    while(y < y_end)
    {
      if( d <= 0 )
      {
        d = d + incrN;
        y = y + 1;
      }
      else
      {
        d = d + incrEN;
        y = y + 1;
        x = x + Xinc;
      }
      SetPixel(x, y, colour);
    }
  }
}




//    void Line(int x0, int y0, int x1, int y1, Uint32 c) { //draw a real RGB line
//        Line(x0, y0, x1, y1, c);
//    }
void Line(int x0, int y0, int x1, int y1, int r, int g, int b)   //draw a real RGB line
{
  Line(x0, y0, x1, y1, Colour(r, g, b));
}

void Line(glm::vec2 s, glm::vec2 e, glm::vec3 c)
{
  Line(s.x, s.y, e.x, e.y, Colour(c.r, c.g, c.b));
}

void Circle(int x, int y, int r, int g, int b, int R)
{
  _cx = x;
  _cy = y;
  _Circle(R, Colour(r, g, b));
}

void Circle(int x, int y, glm::vec3 c, int R)
{
  _cx = x;
  _cy = y;
  _Circle(R, Colour(c));
}


/*

   This function is not used in coursework, please ignore.
   I dont think it works.


            A General Conics Sections Scan Line Algorithm



   The following code is the complete algorithm for the general conic
   drawer as mentioned in Foley/VanDam. It is included here with the
   permission of Andrew W. Fitzgibbon, who derived the remaining code
   sections not included in the book.

   Taken from WWW and mofified for SDL jnc
 */

//
// CONIC  2D Bresenham-like conic drawer.
//       CONIC(Sx,Sy, Ex,Ey, A,B,C,D,E,F) draws the conic specified
//       by A x^2 + B x y + C y^2 + D x + E y + F = 0, between the
//       start point (Sx, Sy) and endpoint (Ex,Ey).

// Author: Andrew W. Fitzgibbon (andrewfg@ed.ac.uk),
//         Machine Vision Unit,
//         Dept. of Artificial Intelligence,
//         Edinburgh University,
//
// Date: 31-Mar-94

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

static int DIAGx[] = {999, 1,  1, -1, -1, -1, -1,  1,  1};
static int DIAGy[] = {999, 1,  1,  1,  1, -1, -1, -1, -1};
static int SIDEx[] = {999, 1,  0,  0, -1, -1,  0,  0,  1};
static int SIDEy[] = {999, 0,  1,  1,  0,  0, -1, -1,  0};
static int BSIGNS[] = {99, 1,  1, -1, -1,  1,  1, -1, -1};


inline int odd(int n)
{
  return n & 1;
}

inline int abs(int a)
{
  if (a > 0)
    return a;
  else
    return -a;
}

int getoctant(int gx, int gy)
{
  // Use gradient to identify octant.
  int upper = abs(gx)>abs(gy);
  if (gx>=0)                              // Right-pointing
    if (gy>=0)                            //    Up
      return 4 - upper;
    else                                  //    Down
      return 1 + upper;
  else                                    // Left
  if (gy>0)                               //    Up
    return 5 + upper;
  else                                    //    Down
    return 8 - upper;
}

int debugging = 1;

#define max(x, y) x<y ? y : x
#define min(x, y) x>y ? y : x

void SSetPixel(int x, int y, int c)   // safe set pixel
{
  x = max(min(x, WIDTH - 2), 1);
  y = max(min(y, HEIGHT - 2), 1);
  SetPixel(x, y, c);
}

void Conic(int xs, int ys, int xe, int ye, int A, int B, int C, int D, int E, int F, int color)
{
  A *= 4;
  B *= 4;
  C *= 4;
  D *= 4;
  E *= 4;
  F *= 4;

  // Translate start point to origin...
  F = A * xs * xs + B * xs * ys + C * ys * ys + D * xs + E * ys + F;
  D = D + 2 * A * xs + B * ys;
  E = E + B * xs + 2 * C * ys;

  // Work out starting octant
  int octant = getoctant(D,E);

  int dxS = SIDEx[octant];
  int dyS = SIDEy[octant];
  int dxD = DIAGx[octant];
  int dyD = DIAGy[octant];

  int bsign = BSIGNS[octant];
  int d,u,v;
  switch (octant)
  {
  case 1:
    d = A + B / 2 + C / 4 + D + E / 2 + F;
    u = A + B / 2 + D;
    v = u + E;
    break;
  case 2:
    d = A / 4 + B / 2 + C + D / 2 + E + F;
    u = B / 2 + C + E;
    v = u + D;
    break;
  case 3:
    d = A / 4 - B / 2 + C - D / 2 + E + F;
    u = -B / 2 + C + E;
    v = u - D;
    break;
  case 4:
    d = A - B / 2 + C / 4 - D + E / 2 + F;
    u = A - B / 2 - D;
    v = u + E;
    break;
  case 5:
    d = A + B / 2 + C / 4 - D - E / 2 + F;
    u = A + B / 2 - D;
    v = u - E;
    break;
  case 6:
    d = A / 4 + B / 2 + C - D / 2 - E + F;
    u = B / 2 + C - E;
    v = u - D;
    break;
  case 7:
    d = A / 4 - B / 2 + C + D / 2 - E + F;
    u =  -B / 2 + C - E;
    v = u + D;
    break;
  case 8:
    d = A - B / 2 + C / 4 + D - E / 2 + F;
    u = A - B / 2 + D;
    v = u - E;
    break;
  default:
    fprintf(stderr,"FUNNY OCTANT\n");
    exit(0);
  }

  int k1sign = dyS * dyD;
  int k1 = 2 * (A + k1sign * (C - A));
  int Bsign = dxD * dyD;
  int k2 = k1 + Bsign * B;
  int k3 = 2 * (A + C + Bsign * B);

  // Work out gradient at endpoint
  int gxe = xe - xs;
  int gye = ye - ys;
  int gx = 2 * A * gxe +   B * gye + D;
  int gy =   B * gxe + 2 * C * gye + E;

  int octantcount = getoctant(gx,gy) - octant;
  if (octantcount <= 0)
    octantcount = octantcount + 8;
  fprintf(stderr,"octantcount = %d\n", octantcount);

  int x = xs;
  int y = ys;

  while (octantcount > 0)
  {
    if (debugging)
      fprintf(stderr,"-- %d -------------------------\n", octant);

    if (odd(octant))
    {
      while (2 * v <= k2)
      {
        // Plot this point
        SSetPixel(x, y, color);

        // Are we inside or outside?
        if (d < 0)                              // Inside
        {
          x = x + dxS;
          y = y + dyS;
          u = u + k1;
          v = v + k2;
          d = d + u;
        }
        else                                    // outside
        {
          x = x + dxD;
          y = y + dyD;
          u = u + k2;
          v = v + k3;
          d = d + v;
        }
      }

      d = d - u + v / 2 - k2 / 2 + 3 * k3 / 8;
      // error (^) in Foley and van Dam p 959, "2nd ed, revised 5th printing"
      u = -u + v - k2 / 2 + k3 / 2;
      v = v - k2 + k3 / 2;
      k1 = k1 - 2 * k2 + k3;
      k2 = k3 - k2;
      int tmp = dxS;
      dxS = -dyS;
      dyS = tmp;
    }
    else                                    // Octant is even
    {
      while (2 * u < k2)
      {
        // Plot this point
        SSetPixel(x, y, color);

        // Are we inside or outside?
        if (d > 0)                              // Outside
        {
          x = x + dxS;
          y = y + dyS;
          u = u + k1;
          v = v + k2;
          d = d + u;
        }
        else                                    // Inside
        {
          x = x + dxD;
          y = y + dyD;
          u = u + k2;
          v = v + k3;
          d = d + v;
        }
      }
      int tmpdk = k1 - k2;
      d = d + u - v + tmpdk;
      v = 2 * u - v + tmpdk;
      u = u + tmpdk;
      k3 = k3 + 4 * tmpdk;
      k2 = k1 + tmpdk;

      int tmp = dxD;
      dxD = -dyD;
      dyD = tmp;
    }

    octant = (octant & 7) + 1;
    octantcount--;
  }

  // Draw final octant until we reach the endpoint
  if (debugging)
    fprintf(stderr,"-- %d (final) -----------------\n", octant);

  if (odd(octant))
  {
    while (2 * v <= k2 && x != xe && y != ye)
    {
      // Plot this point
      SSetPixel(x, y, color);
      // Are we inside or outside?
      if (d < 0)                              // Inside
      {
        x = x + dxS;
        y = y + dyS;
        u = u + k1;
        v = v + k2;
        d = d + u;
      }
      else                                    // outside
      {
        x = x + dxD;
        y = y + dyD;
        u = u + k2;
        v = v + k3;
        d = d + v;
      }
    }
  }
  else                            // Octant is even
  {
    while ((2 * u < k2) && (x != xe) && (y != ye))
    {
      // Plot this point
      SSetPixel(x, y, color);
      // Are we inside or outside?
      if (d > 0)                              // Outside
      {
        x = x + dxS;
        y = y + dyS;
        u = u + k1;
        v = v + k2;
        d = d + u;
      }
      else                                    // Inside
      {
        x = x + dxD;
        y = y + dyD;
        u = u + k2;
        v = v + k3;
        d = d + v;
      }
    }
  }
}
// } // end namespace LINES

/*
   void MidpointEllipse(int xc,int yc,int a,int b,int color)
   {
   float aa=a*a,bb=b*b;
   float twoaa=2*aa,twobb=2*bb;
   float x=0,y=b;
   float d;
   float dx=0;
   float dy=twoaa*y;
   printf("%d %d %d %d %x\n", xc, yc, a, b, color);
   d=(float)(bb+aa*(-b+0.25));
   SetPixel((int) (xc+x),(int) (yc+y),color);
   SetPixel((int) (xc+x),(int) (yc-y),color);
   SetPixel((int)(xc-x),(int)(yc+y),color);
   SetPixel((int)(xc-x),(int)(yc-y),color);
   while(dx<dy)
   {
     printf("%d %d %f %f %f\n", x, y, dx, dy, d);
   x++;
   dx+=twobb;
   if(d<0)
      d+=bb+dx;
   else
   {
    dy-=twoaa;
    d+=bb+dx-dy;
    y--;
   }
   SetPixel((int) (xc+x),(int) (yc+y),color);
   SetPixel((int) (xc+x),(int) (yc-y),color);
   SetPixel((int)(xc-x),(int)(yc+y),color);
   SetPixel((int)(xc-x),(int)(yc-y),color);
   }
   d=(float)(bb*(x+0.5)*(x+0.5)+aa*(y-1)*(y-1)-aa*bb);
   while(y>0)
   {
    y--;
    dy-=twoaa;
    if(d>0)
      d+=aa-dy;
    else
     {
       x++;
       dx+=twobb;
       d+=aa-dy+dx;
     }
   SetPixel((int) (xc+x),(int) (yc+y),color);
   SetPixel((int) (xc+x),(int) (yc-y),color);
   SetPixel((int)(xc-x),(int)(yc+y),color);
   SetPixel((int)(xc-x),(int)(yc-y),color);
   }
   }
 */
