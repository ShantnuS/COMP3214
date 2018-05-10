#include <RGL.h>

#include "GouraudShader.h"

int main(int argc, char *argv[]) {
  vector<glm::vec4> toDraw, toColour;
  GouraudShader p;
  StartGraphics("Test Triangle Drawing Using Per Vertex Edge Shader.");
  World.DefaultColour = WHITE;
  ClearScreen();
  toDraw = {{ 1., 1., 0., 1.}, {-1, 1., 0., 1.}, {-1., -1.,  0.,  1.}};
  toColour = {{ 1., 0., 0., 1.}, {0., 1., 0., 1.}, {0., 0.,  1.,  1.}}; // fload colour x in range 0. > x < 1.0
  int v = p.add(toDraw, toColour);
  printf("Vertices converted %d\n", v);
  float angle = 3.14159f / 4.0f;

  glm::vec3 axis = glm::vec3(0., 0., 1.);
  p.Transform = glm::rotate(glm::mat4(1.), angle * 2.f, axis);
  p.Transform = glm::scale(p.Transform,  glm::vec3(0.5f));
  p.Transform = glm::translate(p.Transform,  glm::vec3(2., 2., 0.));
  p.draw();

  p.Transform = glm::rotate(glm::mat4(1.), angle, axis);
  p.draw();
  StopGraphics(true);
}
