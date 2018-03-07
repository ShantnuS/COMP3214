#include <RGL.h>

#define CHECK(w, v) (v >= 0 && v < w)

int outcode(int wxl, int wxr, int wyb, int wyt, int x, int y)
{
  int out = 0;
  if(x < wxl)
    out = out | 1;
  if(x > wxr)
    out = out | 2;
  if(y < wyb)
    out = out | 4;
  if(y > wyt)
    out = out | 8;
  return out;
}

// namespace LINES
// {
bool Clip(glm::vec2 x)
{
// clips pixel x where wxl < x < wxr, wyb < y < wyt.
// If pixel outside window then return false (0) otherwise returns outcode.
  int in = outcode(0, WIDTH, 0, HEIGHT, (int) x[0], (int) x[1]);
  return (bool) in;
}
int Clip(int wxl, int wxr, int wyb, int wyt, int x, int y)
{
// clips pixel [x, y] where wxl < x < wxr, wyb < y < wyt.
// If pixel outside window then return false (0) otherwise returns outcode.
  int in = outcode(wxl, wxr, wyb, wyt, x, y);
  return in;
}

int Clip(int wxl, int wxr, int wyb, int wyt, int *x0, int *y0, int *x1, int *y1)
// clips line from v1 to v2 to wxl < x < wxr,
// wyb < y < wyt.  If line outside window then
// reject is set true, otherwise the visible
// portion is from the amended v1 to v2.
{
  int c,c1,c2,done, reject;
// const int wyb =0;
// const int wxl = 0;
  double t,x,y;
  wxr--;   // set proper bounds
  wyt--;
  reject = FALSE;
  done = FALSE;
  c1 = outcode(wxl, wxr, wyb, wyt, *x0, *y0);
  c2 = outcode(wxl, wxr, wyb, wyt, *x1, *y1);
//printf("Codes %x %x\n", c1, c2);
  while (!done && (c1 != 0 || c2 != 0))
  {
    if ((c1 & c2) != 0)
    {
      reject = TRUE;
      done = TRUE;
    }
    else if(c1 == c2 && c1 != 0)
    {
      reject = TRUE;
      done = TRUE;
    }
    else
    {
      c = c1;
      if (c == 0) c = c2;          // c is outside
      if ((c & 0x0001) != 0)
      {
        // intersects left edge
        // finds point of intersecion
        t = (double)(wxl - *x0) / (double)(*x1 - *x0);
        x = wxl;
        y = (1.0 - t) * ((double)*y0) + t * ((double)*y1);
      }
      else if ((c & 0x0002) != 0)
      {
        // intersects right edge
        t = (double)(wxr - *x0) / (double)(*x1 - *x0);
        x = wxr;
        y = (1.0 - t) * ((double)*y0) + t * ((double)*y1);
      }
      else if ((c & 0x0004) != 0)
      {
        // intersects bottom edge
        t = (double)(wyb - *y0) / (double)(*y1 - *y0);
        y = wyb;
        x = (1.0 - t) * ((double)*x0) + t * ((double)*x1);
      }
      else if ((c & 0x0008) != 0)
      {
        // intersects top edge
        t = (double)(wyt - *y0) / (double)(*y1 - *y0);
        y = wyt;
        x = (1.0 - t) * ((double)*x0) + t * ((double)*x1);
      }
      if (c == c1)          // v1 was outside, replace it by intersection
      {
        *x0 = (int) x;
        *y0 = (int) y;
        c1 = outcode(wxl, wxr, wyb, wyt, *x0, *y0);
      }
      else                 // v2 was outside, replace it by intersection
      {
        *x1 = (int) x;
        *y1 = (int) y;
        c2 = outcode(wxl, wxr, wyb, wyt, *x1, *y1);
      }
    }
  }
  return reject;
}
// } // end of namespce 'Lines' for clip
