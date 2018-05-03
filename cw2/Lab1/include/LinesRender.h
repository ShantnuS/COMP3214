#ifndef LINES_RENDER_H_INCLUDED
#define LINES_RENDER_H_INCLUDED

#include <RGL.h>
#include <BasicRender.h>

using namespace std;

class LinesRender : public BasicRender  {
public:
int draw(bool lineLoop, int col){
  int objects = in_vertices.size();
  /*
   * Convert vertices to screen coordiates.
   */
  vertices.clear();       // need this otherwise draw previous values as well
  for(int i = 0; i < objects; i++) {
    BasicRender::vertex(in_vertices[i]);
  }
  /*
   * Render lines as line strip or line looo.
   */
  for(int i = 1; i < objects; i++) {
    Line((int) vertices[i].x, (int) vertices[i].y, (int) vertices[i - 1].x, (int) vertices[i - 1].y, col);
  }
  if(lineLoop)
    Line((int) vertices[objects - 1].x, (int) vertices[objects - 1].y, (int) vertices[0].x, (int) vertices[0].y, col);

}
private:

void iSwap( int *x, int *y) {
  int t;
  t = *x;
  *x = *y;
  *y = t;
}
void fSwap( float *x, float *y) {
  float t;
  t = *x;
  *x = *y;
  *y = t;
}


void Line(int x0, int y0, int x1, int y1, Uint32 colour) {     //draw a real line
  int dx, dy, incrE, incrNE, d, x, y, x_end;
  int incrN, incrEN, y_end;
  int Xinc, Yinc;
  dx = x1 - x0;
  dy = y1 - y0;
  if(abs(dx) > abs(dy)) {
    if(dx<0) {
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
    while(x < x_end) {
      if( d <= 0 ) {
        d = d + incrE;
        x = x + 1;
      } else {
        d = d + incrNE;
        x = x + 1;
        y = y + Yinc;
      }
      SetPixel(x, y, colour);
    }
  } else {
    if(dy<0) {
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
    while(y < y_end) {
      if( d <= 0 ) {
        d = d + incrN;
        y = y + 1;
      } else {
        d = d + incrEN;
        y = y + 1;
        x = x + Xinc;
      }
      SetPixel(x, y, colour);
    }
  }
}
};

#endif // LINES_RENDER_H_INCLUDED
