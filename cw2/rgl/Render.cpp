#include <RGL.h>
#include <Render.h>

bool sorty(std::valarray<float> a, std::valarray<float> b) {
  return a[1] > b[1];
}
void printv(char const *t, std::valarray<float> a) {
  printf("V: %20s:", t);
  for(int i = 0; i < a.size(); i++) {
    printf(" %8.1f", a[i]);
  }
  printf(";\n");
}

void Render::fillBottomFlatTriangle(std::valarray<float> v1, std::valarray<float> v2, std::valarray<float> v3) {
  /*
   *   v1








     v2 *------------------------------------------- * v3
   */

  if(v2[0] > v3[0])
  {
    v3.swap(v2);
  }
  std::valarray<float> invslope1 = (v2 - v1) / (v2[1] - v1[1]);
  std::valarray<float> invslope2 = (v3 - v1) / (v3[1] - v1[1]);
  std::valarray<float> curx1 = v2;
  std::valarray<float> curx2 = v3;
  for (; curx1[1] < v1[1]; curx1 += invslope1, curx2 += invslope2) {
    std::valarray<float> xinc = (curx2 - curx1) / (curx2[0] - curx1[0]);       // xsteps
    for(std::valarray<float> scancolX = curx1; scancolX[0] < (curx2[0] - 1.); scancolX += xinc)
    {
      fragment(scancolX);
    }
  }

}
void Render::fillTopFlatTriangle(std::valarray<float> v1, std::valarray<float> v2, std::valarray<float> v3) {
  /*
     v1 *------------------------------------------- * v2









   *   v3
   */
  if(v1[0] > v2[0]) {
    v1.swap(v2);
  }
  std::valarray<float> invslope1 = (v1 - v3) / (v1[1] - v3[1]);
  std::valarray<float> invslope2 = (v2 - v3) / (v2[1] - v3[1]);
  std::valarray<float> curx1 = v3;
  std::valarray<float> curx2 = v3;
  for (; curx1[1] < v1[1]; curx1 += invslope1, curx2 += invslope2)  {
    std::valarray<float> xinc = (curx2 - curx1) / (curx2[0] - curx1[0]);       // xsteps
    for(std::valarray<float> scancolX = curx1; scancolX[0] < (curx2[0] - 1.); scancolX += xinc)
    {
      fragment(scancolX);
    }
  }
}


bool isEdge(std::valarray<float> v1, std::valarray<float> v2, std::valarray<float> v3){
	bool isInside = (((v3[0] - v1[0] ) * (v2[1] - v1[1])) - ((v3[1] - v1[1]) * (v2[0] - v1[0])) >= 0);
	return isInside;
}

void Render::drawShadedTriangle(std::valarray<float> v1, std::valarray<float> v2, std::valarray<float> v3) {
  /*
      After sort.
   * v1
	



   * v2                                         * v4



   * v3
   
   
   
   */
   int width = WIDTH;
   int height = HEIGHT; 
   
   //Loop throgh each pixel on screen and then check if inside and then draw if it is
   for(int x = 0; x < width; x++){
	   for(int y = 0; y < height; y++){
			std::valarray<float> p = {x,y}; 
		    bool inside = true; 
			inside &= isEdge(v1,v2,p);
			inside &= isEdge(v2,v3,p);
			inside &= isEdge(v3,v1,p);
			if (inside){
				fragment(p);
			}
	   }
   }
   
   
  /* at first sort the three glm::vec2's by y-coordinate ascending so v1 is the topmost. */
  
  // assert(v1.size() == v2.size() && v2.size() == v3.size());
  // assert(v1.size() >= 4);
  // assert(v2.size() >= 4);
  // assert(v3.size() >= 4);

  // std::vector<std::valarray<float> > co = {v1, v2, v3};

  // std::sort(co.begin(), co.end(), sorty);

  // if((int) co[1][1] == (int) co[2][1] && (int) co[0][1] == (int) co[2][1])       //degenerate triangle.
    // return;

  // if ((int) co[1][1] == (int) co[2][1])        /* check for trivial case of bottom-flat triangle */
  // {
    // fillBottomFlatTriangle(co[0], co[1], co[2]);
  // }
  // else if ((int) co[0][1] == (int) co[1][1])           /* check for trivial case of top-flat triangle */
  // {
    // fillTopFlatTriangle(co[0], co[1], co[2]);
  // }
  // else           /* general case - split the triangle in a topflat and bottom-flat one */
  // {
    // std::valarray<float> v4;
    // v4 =  co[0]  + ((co[1][1] - co[0][1]) / (co[2][1] - co[0][1])) * (co[2] - co[0]);
    // fillBottomFlatTriangle(co[0], co[1], v4);
    // fillTopFlatTriangle(co[1], v4, co[2]);
  // }
}


int Render::add(vector<glm::vec4> points) {     // adds a vector of vertices
  int newObjects = points.size();
  for(int i = 0; i < newObjects; i++)
    Render::in_vertices.push_back(points[i]);
  return Render::in_vertices.size();
}

int Render::add(glm::vec4 point)  {    // adds a single vertices
  Render::in_vertices.push_back(point);
  return Render::in_vertices.size();
}
int Render::size(void) {     // returns the size.
  return Render::in_vertices.size();
}

int Render::remove(int n) {     // removes one or more vertices
  int objects = Render::in_vertices.size();
  assert(n > 0);
  if(n >= objects)
  {
    Render::in_vertices.clear();
  }
  else
  {
    for(int i = 0; i < n; i++)
      Render::in_vertices.pop_back();
  }
  return Render::in_vertices.size();
}

void Render::vertex(glm::vec4 point) {     // this constructs the geometry data
  glm::vec4 t;
  t = Transform * point;
  t[0] = t[0] / t[3];     // homogenous normalization
  t[1] = t[1] / t[3];
  t[2] = t[2] / t[3];
  t[3] = 1.0;
  t = _workstation(t);

  std::valarray<float> vv = {t[0], t[1], t[2], t[3]};        //x, y, z, w, b brings all the attributes togethrer for rendering.

  Render::vertices.push_back(vv);
}


int Render::draw()
{
  int objects = in_vertices.size();
  /*
   * Convert vertices to screen coordiates.
   */
  Render::vertices.clear();       // need this otherwise draw previous values as well
  for(int i = 0; i < objects; i++)
  {
    Render::vertex(Render::in_vertices[i]);
  }
  /*
   * Render every 3 as a triangle..
   */
  for(int i = 0; i < objects; i += 3)
  {
    drawShadedTriangle(Render::vertices[i], Render::vertices[i + 1], Render::vertices[i + 2]);
  }
}
