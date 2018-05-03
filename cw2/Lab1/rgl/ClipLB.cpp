#include <RGL.h>

/* ------------------------- Liang-Barskey -------------------------------- */


#define WL (WIDTH / 4) / 8
#define WR ((WL + WIDTH / 2 / 8))
#define WB (HEIGHT / 4) / 8
#define WT ((WB + HEIGHT / 2 / 8))

/* Liang-Barsky Polygon Clipping [CACM, Vol 26 (Nov, 1983)]
 *    (with corrections from [CACM (April, 1984)])
 *
 *    Note that this assumes the last point == the first in the
 *    polygon representation the code in the article adds the
 *    last point in at the start of the algorithm (probably
 *    a better approach).
 */



int npnt;

// #define INFINITY      (1.0e+30)

/* add a new vertex into the output polygon */

#define add(xp,yp) {npoly[npnt].x = xp; npoly[npnt].y = yp; ++npnt;}

/* window bounds (xleft,ybottom), (xright,ytop) */

int wy1 = WB;
int wy2 = WT;
int wx1 = WL;
int wx2 = WR;

typedef struct PLN
{
  int x;
  int y;
} PLN;


// namespace LINES
// {

int PolyClip(PLN *opoly, PLN *npoly, int opoints)
{
  int i, npnt;
  float deltax, deltay, xin,xout,  yin,yout;
  float tinx,tiny,  toutx,touty,  tin1, tin2,  tout1,tout2;
  float x1,y1, x2,y2;
  npnt = 0;
  for (i = 0; i < opoints - 1; ++i)
  {
    x1 = opoly[i].x;
    y1 = opoly[i].y;
    x2 = opoly[i + 1].x;
    y2 = opoly[i + 1].y;
    deltax = x2 - x1;
    deltay = y2 - y1;
    if (deltax > 0 || (deltax == 0 && x1>wx2))       /*  points to right */
    {
      xin = wx1;
      xout = wx2;
    }
    else
    {
      xin = wx2;
      xout = wx1;
    }
    if (deltay > 0 || (deltay == 0 && y1>wy2))       /*  points up */
    {
      yin = wy1;
      yout = wy2;
    }
    else
    {
      yin = wy2;
      yout = wy1;
    }
    tinx = (deltax != 0) ? ((xin - x1) / deltax) : -INFINITY;
    tiny = (deltay != 0) ? ((yin - y1) / deltay) : -INFINITY;
    if (tinx < tiny)             /* hits x first */
    {
      tin1 = tinx;
      tin2 = tiny;
    }
    else                         /* hits y first */
    {
      tin1 = tiny;
      tin2 = tinx;
    }
    if (1 >= tin1)
    {
      if (0 < tin1)
      {
        add(xin,yin);
      }
      if (1 >= tin2)
      {
        if (deltax != 0) toutx = (xout - x1) / deltax;
        else
        {
          if (wx1 <= x1 && x1 <= wx2) toutx = INFINITY;
          else toutx = -INFINITY;
        }
        if (deltay != 0) touty = (yout - y1) / deltay;
        else
        {
          if (wy1 <= y1 && y1 <= wy2) touty = INFINITY;
          else touty = -INFINITY;
        }
        tout1 = (toutx < touty) ? toutx : touty;
        if (0 < tin2 || 0 < tout1)
        {
          if (tin2 <= tout1)
          {
            if (0 < tin2)
            {
              if (tinx > tiny)
              {
                add (xin, y1 + tinx * deltay);
              }
              else
              {
                add (x1 + tiny * deltax, yin);
              }
            }
            if (1 > tout1)
            {
              if (toutx < touty)
              {
                add (xout, y1 + toutx * deltay);
              }
              else
              {
                add (x1 + touty * deltax, yout);
              }
            }
            else
            {
              add (x2,y2);
            }
          }
          else
          {
            if (tinx > tiny)
            {
              add (xin, yout);
            }
            else
            {
              add (xout, yin);
            }
          }
        }
      }
    }
  }

  if (npnt)
  {
    add(npoly[0].x, npoly[0].y);
  }
  return npnt;
}

// } // end namespace Lines
