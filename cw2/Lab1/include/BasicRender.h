#ifndef BASE_RENDER_H_INCLUDED
#define BASE_RENDER_H_INCLUDED

#include <RGL.h>

using namespace std;

class BasicRender {
public:
vector<glm::vec4> vertices;
vector<glm::vec4> in_vertices;
glm::mat4 Transform = glm::mat4(1.);
int add(vector<glm::vec4> points){
  int newObjects = points.size();
  for(int i = 0; i < newObjects; i++)
    in_vertices.push_back(points[i]);
  return in_vertices.size();
}

int add(glm::vec4 point){
  in_vertices.push_back(point);
  return in_vertices.size();
}
int size(void){
  return in_vertices.size();
}

int remove(int n = 1){
  int objects = in_vertices.size();
  assert(n > 0);
  if(n >= objects) {
    in_vertices.clear();
  }
  else {
    for(int i = 0; i < n; i++)
      in_vertices.pop_back();
  }
  return in_vertices.size();
}

void vertex(glm::vec4 point){
  glm::vec4 t;
  t = Transform * point;
  t = _workstation(t);
  vertices.push_back(t);
}

int draw(void){
  glm::vec4 c  = glm::vec4(0., 0., 0., 0.);
  int objects = in_vertices.size();
  printf("Objects to draw %d\n", objects);
  if(objects < 1)
    return 0;
  vertices.clear();
  /*
   * Convert vertices to screen coordiates.
   */
  for(int i = 0; i < objects; i++) {
    vertex(in_vertices[i]);
  }
  for(int i = 0; i < objects; i++) {
    fragment(vertices[i], c);
  }
  return objects;
}


inline virtual void fragment(glm::vec4 p,  glm::vec4 c){
  glm::vec2 p2 = glm::vec2(p.x, p.y);
  glm::vec3 c2 = glm::vec3(c.r, c.g, c.b) * 250.0f;
  int i = (int) p[0];
  int j = (int) p[1];
  Uint32 p_c = Colour(c2[0], c2[1], c2[2]);
  SetPixel(i, j, p_c);
}


};

/*

   Note:

   Place inline function in the header file.

   I guess they must be available for imediate compilation.

   So this is why seperate cpmpilation fails.

   10/1/2017

 */



#endif // BASE_RENDER_H_INCLUDED
