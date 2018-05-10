#include <RGL.h>
#include "FullRender.h"


int main(int argc, char *argv[])
{
  vector<glm::vec4> toDraw, toColour;
  Render p;
  StartGraphics("Test Triangle Drawing.");
  World.DefaultColour = WHITE;
  World.ZBuffer = true;
  toDraw = {{ 1., 1., 0., 1.},{-1, 1., 0., 1.},{-1., -1.,  0.,  1.}};
  toColour = {{1.0, 0.0, 0.0, 1.}, {0.0, 1.0, 0.0, 1.}, {0.0, 0.0, 1.0, 1.}};
  p.add(toDraw, toColour);
  float time;
  while(Animate(&time)) {
    float angle = 0.05 * time;
    float z = fmod(time * 0.1, 5.) - 1.;
    Lock();       // needed for SDL 1.2, ignores for SDL 2+
    ClearScreen();
    glm::vec3 axis = glm::vec3(0., 0., 1.);
    p.Transform = glm::scale(glm::mat4(1.),  glm::vec3(0.5f));
    p.Transform = glm::translate(p.Transform,  glm::vec3(0.0f, 0.0f, z));
    p.draw();
    p.Transform = glm::rotate(glm::mat4(1.), angle * 3.5f, axis);
    p.draw();
    UnLock();
  }
  StopGraphics(false);
}
