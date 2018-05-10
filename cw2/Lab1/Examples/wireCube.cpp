#include <RGL.h>
#include "LinesRender.h"

//
// vertices, normals and colours for a cube, 8 of each, see vertices.h
//

float vertices[][3] = {{-1.0,-1.0,-1.0},{1.0,-1.0,-1.0},
                       {1.0,1.0,-1.0}, {-1.0,1.0,-1.0}, {-1.0,-1.0,1.0},
                       {1.0,-1.0,1.0}, {1.0,1.0,1.0}, {-1.0,1.0,1.0}};

void polygon(std::vector<std::vector<glm::vec4> > *faces, int a, int b, int c, int d) {
  std::vector<glm::vec4> face;
  face.push_back(glm::vec4(vertices[a][0], vertices[a][1], vertices[a][2], 1.));
  face.push_back(glm::vec4(vertices[b][0], vertices[b][1], vertices[b][2], 1.));
  face.push_back(glm::vec4(vertices[c][0], vertices[c][1], vertices[c][2], 1.));
  face.push_back(glm::vec4(vertices[d][0], vertices[d][1], vertices[d][2], 1.));
  faces->push_back(face);
}

void cube(std::vector<std::vector<glm::vec4> > *faces) {
  polygon(faces, 0, 3, 2, 1);
  //~ polygon(faces, 2, 3, 7, 6);
  //~ polygon(faces, 0, 4, 7, 3);
  //~ polygon(faces, 1, 2, 6, 5);
  //~ polygon(faces, 4, 5, 6, 7);
  //~ polygon(faces, 0, 1, 5, 4);
}


int main(int argc, char *argv[]) {
  std::vector<std::vector<glm::vec4> > faces;   // In '> >' the space is required.
  cube(&faces);      // generate faces, setup
  LinesRender p;
  StartGraphics("Wire Cube Demo");
  World.DefaultColour = BLACK;
  for(int k = 0; k < 6; k++) {
    p.add(faces[k]);
  }
  float time;
  while(Animate(&time)) {
    float angle = 0.05 * time;
    Lock();     // needed for SDL 1.2, ignores for SDL 2+
    ClearScreen();
    glm::vec3 axis = glm::vec3(0., 1., 1.);
    p.Transform = glm::rotate(glm::mat4(1.), angle, axis);
    p.draw(true, RED);
    UnLock();
  }
  StopGraphics(false);
}
