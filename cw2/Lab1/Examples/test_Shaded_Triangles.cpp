#include <RGL.h>
#include "Render.h"


class Shader : public Render {
public:
vector<glm::vec4> colours;

inline virtual void fragment(std::valarray<float> p) {
  //defined as virual so it can be overwride base class, allows drawing code to be customized.
  assert(p.size() >= 8); // x, y, z, w, r, g, b, a
  int i = (int) p[0];
  int j = (int) p[1];
  SetPixel(i, j, Colour(p[4], p[5], p[6]));
}

int add(glm::vec4 point, glm::vec4 colour);
int add(vector<glm::vec4> points, vector<glm::vec4> shades);
void vertex(glm::vec4 point, glm::vec4 col);     // this constructs the geometry data
int draw();
};


int main(int argc, char *argv[])
{
  vector<glm::vec4> toDraw, toColour;
  Shader p;
  StartGraphics("Test Triangle Drawing.");
  World.DefaultColour = WHITE;
  ClearScreen();
  toDraw = {{ 1., 1., 0., 1.},{-1, 1., 0., 1.},{-1., -1.,  0.,  1.}};
  toColour = {{1.0, 0.0, 0.0, 1.}, {0.0, 1.0, 0.0, 1.}, {0.0, 0.0, 1.0, 1.}};
  p.add(toDraw, toColour);
  float angle = 3.14159f / 9.0f;
  glm::vec3 axis = glm::vec3(0., 0., 1.);
  p.Transform = glm::rotate(glm::mat4(1.), angle, axis);
  p.draw();
  p.Transform = glm::rotate(glm::mat4(1.), angle * 2.f, axis);
  p.Transform = glm::scale(p.Transform,  glm::vec3(0.5f));
  p.draw();
  StopGraphics(true);
}
