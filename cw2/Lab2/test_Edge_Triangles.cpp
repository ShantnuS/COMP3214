#include <RGL.h>

#include "FlatShader.h"

int main(int argc, char *argv[]) {
  vector<glm::vec4> toDraw;
  FlatShader p;
  StartGraphics("Test Triangle Drawing Using Edge Shader.");
  World.DefaultColour = WHITE;
  ClearScreen();
  toDraw = {{ 1., 1., 0., 1.},{-1, 1., 0., 1.},{-1., -1.,  0.,  1.}};
  int v = p.add(toDraw);
  printf("Vertices converted %d\n", v);
  float angle = 3.14159f / 4.0f;
  glm::vec3 axis = glm::vec3(0., 0., 1.);
  p.Transform = glm::rotate(glm::mat4(1.), angle, axis);
  World.ObjectColour = RED;
  p.draw();
  p.Transform = glm::rotate(glm::mat4(1.), angle * 2.f, axis);
  p.Transform = glm::scale(p.Transform,  glm::vec3(0.5f));
  World.ObjectColour = GREEN;
  p.draw();
  StopGraphics(true);
}
