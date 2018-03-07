#include <RGL.h>
#include "Shader.h"


int Shader::add(glm::vec4 point, glm::vec4 colour)
{
  in_vertices.push_back(point);
  colours.push_back(colour);
  return in_vertices.size();
}

int Shader::add(vector<glm::vec4> points, vector<glm::vec4> shades)
{
  int newObjects = points.size();
  printf("SizE %d %d\n", points.size(), shades.size());
  assert(points.size() == shades.size());
  for(int i = 0; i < newObjects; i++)
  {
    in_vertices.push_back(points[i]);
    colours.push_back(shades[i]);
  }
  return in_vertices.size();
}

void Shader::vertex(glm::vec4 point, glm::vec4 col) {     // this constructs the geometry data
  glm::vec4 t;
  t = Transform * point;
  t[0] = t[0] / t[3];     // homogenous normalization
  t[1] = t[1] / t[3];
  t[2] = t[2] / t[3];
  t[3] = 1.0;
  t = _workstation(t);

  std::valarray<float> vv = {t[0], t[1], t[2], t[3], col[0], col[1], col[2], col[3]};        //x, y, z, w, b brings all the attributes togethrer for rendering.

  vertices.push_back(vv);
}


int Shader::draw()
{
  int objects = in_vertices.size();
  /*
   * Convert vertices to screen coordiates.
   */
  vertices.clear();       // need this otherwise draw previous values as well
  for(int i = 0; i < objects; i++)
  {
    vertex(in_vertices[i], colours[i]);
  }
  /*
   * Render lines as line strip or line looo.
   */
  for(int i = 0; i < objects; i += 3)
  {
    drawShadedTriangle(vertices[i], vertices[i + 1], vertices[i + 2]);
  }
}
