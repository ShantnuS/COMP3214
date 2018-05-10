#include <RGL.h>

/*

   This function is not used in  coursework 2, please ignore.

 */

/*
    Bezier
 */

typedef struct point
{
  float x;
  float y;
} point;

typedef struct bezier
{
  point a;
  point b;
  point c;
  point d;
} bezier;

/*

   for testing

   glm::vec2 start[4] = { {10*5, 10*5 }, {25*5, 29*5}, {45*5, -10*5}, {70*5, 19*5}};
   DrawBezier(&start, Colour255, 128, 128));

 */

void DrawBezierBasic(bezier *, int);
void Divide( bezier *, bezier *, bezier *);
int same(point, point);


void DrawBezierBasic(bezier *in, int colour)
{
  if(same(in->a, in->d))
  {
    SetPixel((int)in->a.x, (int)in->a.y, colour);
  }
  else
  {
    bezier left, right;
    Divide(in, &left, &right);
    DrawBezierBasic(&right, colour);
    DrawBezierBasic(&left, colour);
  }
}

point average(point a, point b)
{
  point work;
  work.x = (a.x + b.x) / 2.0;
  work.y = (a.y + b.y) / 2.0;
  return work;
}

void Divide( bezier *p, bezier *l, bezier *r)
{
  point h;
  l->a = p->a;
  r->d = p->d;
  l->b = average(p->a, p->b);
  h = average(p->b, p->c);
  l->c = average(h, l->b);
  r->c = average(p->c, p->d);
  r->b = average(h, r->c);
  l->d = average(l->c, r->b);
  r->a = l->d;
}

int same(point a, point b)
{
  float dx, dy;
  dx = (a.x - b.x);
  dy = (a.y - b.y);
  if( fabs((double) dx) < 0.06 &&  fabs((double)dy) <0.06)
    return 1;
  else
    return 0;
}

void DrawBezier(glm::vec2 *control_points, int colour)
{
  bezier p;
  print("Bezier drawing has not been tested.");
  print("Use at own risk, your computer may explode.");
  print("Bezier interface is really bad.");
  p.a.x = control_points[0].x;
  p.a.y = control_points[0].y;
  p.b.x = control_points[1].x;
  p.b.y = control_points[1].y;
  p.c.x = control_points[2].x;
  p.c.y = control_points[2].y;
  p.d.x = control_points[3].x;
  p.d.y = control_points[3].y;
  DrawBezierBasic(&p, colour);
}

glm::vec2 interpolatedPosition(glm::vec2 p0, glm::vec2 p1, glm::vec2 p2, glm::vec2 p3, float t)
{
  /*
   * interpolation for basic Catmull-Rom spline etc.
   */
  float t3 = t * t * t;
  float t2 = t * t;

  float f1 = -0.5 * t3 + t2 - 0.5 * t;
  float f2 = 1.5 * t3 - 2.5 * t2 + 1.0;
  float f3 = -1.5 * t3 + 2.0 * t2 + 0.5 * t;
  float f4 = 0.5 * t3 - 0.5 * t2;

  float x = p0.x * f1 + p1.x * f2 + p2.x * f3 + p3.x * f4;
  float y = p0.y * f1 + p1.y * f2 + p2.y * f3 + p3.y * f4;

  return glm::vec2(x, y);
}
