#ifndef RENDER_H_INCLUDED
#define RENDER_H_INCLUDED

#include <RGL.h>

#include "GouraudShader.h"

#include <algorithm>

#define EDGE_DEBUG 0

using namespace std;

class Render :  public GouraudShader {
public:
edge e12, e23, e31;

attribute Red;
attribute Green;
attribute Blue;
attribute Z;

void drawEdgeShadedTriangle(glm::vec4 v1, glm::vec4 v2, glm::vec4 v3, glm::vec4 c1, glm::vec4 c2, glm::vec4 c3) {
/*
 *
 * Draw a triangle, shaded with a constant colour, World.ObjectColour
 *
 */
  if(EDGE_DEBUG) printf("[%f, %f], [%f, %f], [%f, %f]\n", v1.x, v1.y, v2.x, v2.y, v3.x, v3.y);
/*
 *
 * Checking the winding of the triagle.
 *
 *         |     1,     1,     1 |
 * d = det | v1[0], v2[0], v3[0] |
 *         | v1[1], v2[1], v3[1] |
 *
 * d = 0, in straight line, d < 0 counterclockwise. d > 0 counterclockwise
 *
 * Edge algorithm assumes clockwise. This may be removed if handedness of triangles are quaranteeded.
 *
 */
  glm::mat3 wind = glm::mat3({1, 1, 1, v1[0], v2[0], v3[0], v1[1], v2[1], v3[1]});
  float d = glm::determinant(wind);
  if(EDGE_DEBUG) printf("Determinent %f\n", d);
  if(d < 0.0) {
    glm::vec4 temp;
    temp = v2;
    v2   = v3;
    v3   = temp;
    temp = c2;
    c2   = c3;
    c3   = temp;
  }

  BoundingBox bb;
  bb.set(v1, v2, v3); // edges/size of box stored in class.
/*
 * set up edges.
 */
  e12.set(v1, v2);
  e23.set(v2, v3);
  e31.set(v3, v1);

/*
 * Set up attributes.
 */
  Red.set(v1, v2, v3, c1[0], c2[0], c3[0]);

  Green.set(v1, v2, v3, c1[1], c2[1], c3[1]);

  Blue.set(v1, v2, v3, c1[2], c2[2], c3[2]);

  Z.set(v1 , v2, v3, v1.z, v2.z, v3.z);
  // Hint Z is another attribute.

  for(int X = bb.ix; X < bb.jx; X++) {
    for(int Y = bb.iy; Y < bb.jy; Y++) {
		
		//Check if X  Y are below 0
      if((e12.E(X, Y) < 0) && (e23.E(X, Y) < 0) && (e31.E(X, Y) < 0) ) {
        float red = Red.eval(NULL, X, Y);
        float green = Green.eval(NULL,X, Y);
        float blue = Blue.eval(NULL, X, Y);
/*
 * Ckeck if z-Buffer required
 */
        if(World.ZBuffer) {
          float z = Z.eval(NULL, X, Y);
		  if (World.zBuffer[WIDTH * X + Y]<z){
			SetPixel(X, Y, Colour(red, green, blue));
			World.zBuffer[WIDTH * X + Y]=z;
		  }
        }
        else{
          SetPixel(X, Y, Colour(red, green, blue));
        }
      }
    }
  }
}
inline virtual void fragment(glm::vec4 p){ // overloads basicRenderer::fragment
  // should really change this to add z-buufer here,
  // but easiest in the draw triangle code.
  int i = (int) p[0];
  int j = (int) p[1];
  SetPixel(i, j, World.ObjectColour);
}

void vertex(glm::vec4 point, glm::vec4 colour){
  glm::vec4 t;
  t = Transform * point;
  t = _workstation(t);
  vertices.push_back(t);
  vcolours.push_back(colour);
}


int draw() // overloads basicRenderer::draw
{
  assert(in_vertices.size() == in_vcolours.size());
  int objects = in_vertices.size();
  /*
   * Convert vertices to screen coordiates.
   * vertices and in_vertices are in base class, BasicRender.
   */
  vertices.clear();        // need this otherwise draw previous values as well
  vcolours.clear();        // and will run out of memory as you keep adding triangles.
  for(int i = 0; i < objects; i++)
  {
    vertex(in_vertices[i], in_vcolours[i]);
  }
  /*
   * Render every 3 as a triangle..
   */
  for(int i = 0; i < objects; i += 3)
  {
    drawEdgeShadedTriangle(vertices[i], vertices[i + 1], vertices[i + 2], vcolours[i], vcolours[i + 1], vcolours[i + 2]);
  }
}

}; // end of class EdgeRender

#endif // RENDER_H_INCLUDED
