#include <RGL.h>
#include "Render.h"


class Shader : public Render {
public:
vector<glm::vec4> colours;

inline virtual void fragment(std::valarray<float> p) {
  //defined as virual so it can be overwride base class, allows drawing code to be customized.
  assert(p.size() >= 8);
  int i = (int) p[0];
  int j = (int) p[1];
  SetPixel(i, j, Colour(p[4], p[5], p[6]));
}

int add(glm::vec4 point, glm::vec4 colour);
int add(vector<glm::vec4> points, vector<glm::vec4> shades);
void vertex(glm::vec4 point, glm::vec4 col);     // this constructs the geometry data
int draw();
};
