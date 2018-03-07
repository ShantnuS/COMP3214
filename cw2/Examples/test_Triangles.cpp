#include <RGL.h>
#include "Render.h"

int main(int argc, char *argv[]) {
  vector<glm::vec4> toDraw;
  Render p;
  StartGraphics("Test Triangle Drawing.");
  World.DefaultColour = WHITE;
  ClearScreen();
  toDraw = {{ 1., 1., 0., 1.},{-1, 1., 0., 1.},{-1., -1.,  0.,  1.}};
  p.flat = true;
  p.add(toDraw);
  float angle = 3.14159f / 4.0f;
  glm::vec3 axis = glm::vec3(0., 0., 1.);
  p.Transform = glm::rotate(glm::mat4(1.), angle, axis);
  World.ObjectColour = RED;
  p.draw();
  p.Transform = glm::rotate(glm::mat4(1.), angle * 2.f, axis);
  p.Transform = glm::scale(p.Transform,  glm::vec3(0.5f));
  World.ObjectColour = BLUE;
  p.draw();
  StopGraphics(true);
}
