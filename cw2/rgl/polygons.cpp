#include "RGL.h"

void FillPoly(int *x, int *y, int n, Uint32 colour); // xy algorithm, template

// namespace POLYGONS
// {
/*
 *
 *
 * Drawing Polygons
 *
 *
 */


void  PolygonXY(std::vector<int> xx, std::vector<int> yy, Uint32 colour)
{
  assert(xx.size() > 2);
  assert(xx.size() == yy.size());
  FillPoly(xx.data(), yy.data(), xx.size(), colour);
  return;    //draw a polygon with a fixed colour, using XY algorithm.
}
void  Polygon(std::vector<int> xx, std::vector<int> yy, Uint32 colour)    //draw a polygon with a fixed colour.
{
//
//    This is a low level routine.
//    Uses the edge following technique.
//
//
// assumes convex polygon, breaks if not.
// Shading helpers
//
  int start[HEIGHT];
  int stop[HEIGHT];
  int vals[HEIGHT];
//
// start up
//

  for(int i = 0; i < HEIGHT; i++)
  {
    start[i] = 0;
    stop[i] = 0;
    vals[i] = 0;
  }
//
// edge scan
//
  int number = xx.size();
  for(int i = 0; i < number; i++)     // loop over edges
  {
    int j = (i + 1) % number;         // last is wrapped.
    int diffx = xx[j] - xx[i];
    int diffy = yy[j] - yy[i];
    float deltax = (float) diffx / (float(abs(diffy)));
    int deltay = diffy >= 0 ? 1 : -1;
    float fx = (float) xx[i];
    int numberSteps = abs(diffy);
    int y, num;
    for(y = yy[i], num = 0; num < numberSteps; y += deltay, num++)
    {
      switch(vals[y])
      {
      case 0:
        start[y] = (int) fx;
        vals[y] = 1;
        break;
      case 1:
        stop[y] = (int) fx;
        vals[y] = 2;
        break;
      case 2:
        print("Can never get 2 with convex polygon");
        exit(1);
      }
      fx += deltax;
    }
  }
//
// Draw scan lines
//
  for(int y = 0; y < HEIGHT; y++)
  {
    if(vals[y] == 0)
    {
      continue;
    }
    else if(vals[y] == 1)
    {
      SetPixel(start[y], y, colour);
      continue;
    }
    else if(vals[y] == 2)
    {
      int xdiff = stop[y] - start[y];
      int xdelta = xdiff > 0 ? 1 : -1;
      int numx = abs(xdiff);
      int x, ix;
      for(x = start[y], ix = 0; ix <= numx; x += xdelta, ix++)
      {
        SetPixel(x, y, colour);
      }
      continue;
    }
    else
    {
      print("Bad nuber of values");
      continue;
    }
  }
  return;
}
void Polygon(std::vector<int> x, std::vector<int> y, glm::vec4 colour)    //draw a polygon with a fixed colour.
{
//
//    This is a low level routine.
//
  Polygon(x, y, Colour(colour));    //draw a polyline
}

void  Polygon(std::vector<int> x, std::vector<int> y, std::vector<glm::vec4> colours)    //draw a polygon with a pervertex colour.
{
//
//    This is a low level routine and is a place holder..
//    This needs to interpolate between vertices. Currently uses first colour.
//
  Polygon(x, y, Colour(colours[0]));    //draw a polyline with colour specified or the first vertex.
}
void PolyLine(std::vector<int> x, std::vector<int> y, Uint32 colour)    //draw a polyline
{
  int i, j;
  int n = x.size();
  for(i = 0, j = 1; i < n - 1; i++, j++)
  {
    Line(x[i], y[i], x[j], y[j], colour);
  }
  Line(x[n - 1], y[n - 1], x[0], y[0], colour);

}
void  FilledBox(int wxl, int wxr, int wyb, int wyt, Uint32 colour)
{
  for(int i = wyb; i < wyt; i++)
    Line(wxl, i, wxr, i, colour);
}
void  Box(int wxl, int wxr, int wyb, int wyt, Uint32 colour)
{
  Line(wxl, wyt, wxr, wyt, colour);
  Line(wxl, wyb, wxr, wyb, colour);
  Line(wxr, wyb, wxr, wyt, colour);
  Line(wxl, wyb, wxl, wyt, colour);
}
void Block(glm::vec2 p, glm::vec3 c)
{
  int r = (int) c[0];
  int g = (int) c[1];
  int b = (int) c[2];
  Uint32 cc  = Colour(r, g, b);
  FilledBox((int) p[0] - 4,(int) p[0] + 4,(int) p[1] - 4,(int) p[1] + 4, cc);
}
// } // End Polygons namespace
/* Old Code */
void DrawPoly(int *x, int *y, int n, Uint32 colour)
{
  // polyline
  int i;
  for(i = 0; i< n - 1; i++)
  {
    printf("%d %d %d\n", x[i], y[i], i);
    Line(x[i], y[i], x[i + 1], y[i + 1], colour);
  }
  Line(x[n - 1], y[n - 1], x[0], y[0], colour);
}

void PlotPoly( int *x, int *y, int n, Uint32 colour )
{
  // poly points
  int i;
  for(i = 0; i<n; i++)
    SetPixel(x[i], y[i], colour);
}

//
// * XY algorithm
//
typedef struct VERTEX
{
  int Ymax;
  int Ymin;
  int Xmin;
  int over;
  int under;
  int delta;
  float start, interp;
  struct VERTEX *next;
}   VERTEX;
#define MAXSCAN HEIGHT
Uint32 plot_colour;
void add(int, int, int, int, VERTEX **);
void intadd(int, int, float, int, int, float, VERTEX **);
void out(VERTEX *);
void dump( VERTEX **);
VERTEX *move( VERTEX *, VERTEX *);
VERTEX *sort(VERTEX *);
VERTEX *prune(VERTEX *, int);
VERTEX *move(VERTEX *, VERTEX *);
VERTEX *convert(VERTEX *, int);
VERTEX *sort(VERTEX *a)
{
  VERTEX *h[MAXSCAN];
  int i,n,change;
  n = 0;
  while(a != NULL)
  {
    h[n] = a;
    a = a->next;
    n++;
  }
  change = TRUE;
  while(change)
  {
    change = FALSE;
    for(i = 1; i<n; i++)
      if(h[i]->Xmin < h[i - 1]->Xmin)
      {
        VERTEX *tmp;
        change = TRUE;
        tmp = h[i];
        h[i] = h[i - 1];
        h[i - 1] = tmp;
      }
  }
  for(i = 0; i<n - 1; i++)
    h[i]->next = h[i + 1];
  h[n - 1]->next = NULL;
  return h[0];
}
void FillPoly(int *x, int *y, int n, Uint32 colour)    //draw a real polygon
{
  int i;
  VERTEX *ET[MAXSCAN];
  VERTEX *AET;
  plot_colour = colour;
  for(i = 0; i < MAXSCAN; i++)      // Initialize Scan array
    ET[i] = NULL;

  AET = NULL;

  for(i = 0; i < n; i++)
  {
    int j = (i + 1) % n;
    add(x[i], y[i], x[j], y[j], ET);
  }

  for(i = 0; i<MAXSCAN; i++)
  {
    if(ET[i] != NULL)
    {
      AET =  move(ET[i], AET);
    }
    if(AET != NULL)
      AET = convert(AET, i);
  }
}
VERTEX *prune(VERTEX *a, int i)
{
  if(a == NULL)
    return a;
  a->next = prune(a->next, i);
  if(a->Ymax == i)
    return a->next;
  else
    return a;
}
void scan(VERTEX *a, int j)
{
  int i;
  if(a == NULL)
    return;
  for(i = a->Xmin; i<a->next->Xmin; i++)
    SetPixel(i, j, plot_colour);
  scan(a->next->next, j);
  return;
}
void increment(VERTEX *a)
{
  if(a == NULL)
    return;
  a->delta = a->delta + a->under;
  if(abs(a->delta) > abs(a->over))
  {
    int inc = a->delta / a->over;
    a->Xmin = a->Xmin + inc;
    a->delta = a->delta - ( inc * a->over);
  }
  increment(a->next);
}
VERTEX *convert(VERTEX *a, int i)
{
  a = sort(a);
  scan(a, i);
  increment(a);
  return prune(a, i + 1);
}
VERTEX *move( VERTEX *a, VERTEX *b)
{
  if(b == NULL)
  {
    return a;
  }
  b->next = move( a, b->next);
  return b;
}
void add(int x0, int y0, int x1, int y1, VERTEX **list)
{
  VERTEX *v;
  if( y0 == y1)
    return;
  if( y0 > y1)     // swap order here
  {
    int tmp;
    tmp = y1;
    y1 = y0;
    y0 = tmp;
    tmp = x1;
    x1 = x0;
    x0 = tmp;
  }

  //   printf("Adding %d %d %d %d\n", x0, y0, x1, y1);

  v = (VERTEX *) malloc( sizeof(VERTEX));

  v->Ymax = y1;
  v->Xmin = x0;
  v->over = y1 - y0;
  v->under = x1 - x0;
  v->delta = 0;
  v->next = NULL;

  if(list[y0] == NULL)
    list[y0] = v;
  else
  {
    v->next = list[y0];
    list[y0] = v;
  }
  return;
}
void out(VERTEX *e)
{
  if( e == NULL)
    printf("NULL ");
  else
  {
    printf("%2d %2d %3d %3d %3d -> ", e->Ymax, e->Xmin, e->over, e->under, e->delta);
    out(e->next);
  }
}
void dump(VERTEX **list)
{
  int i;
  for(i = 0; i< MAXSCAN; i++)
  {
    printf("%3d ", i);
    out(list[i]);
    putchar('\n');
  }
}
