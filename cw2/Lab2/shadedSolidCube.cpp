#include "FullRender.h"

using namespace std;
/*
 * vertices, norm, colours.
 */

float vertices[][3] = {{-1.0, -1.0, -1.0,}, // 0
                       { 1.0, -1.0, -1.0}, // 1
                       { 1.0,  1.0, -1.0}, // 2
                       {-1.0,  1.0, -1.0}, // 3
                       {-1.0, -1.0,  1.0}, // 4
                       {1.0,  -1.0,  1.0}, // 5
                       {1.0,   1.0,  1.0}, // 6
                       {-1.0,  1.0,  1.0}};// 7
					   
					   

float normals[][3] = {{-1.0,-1.0,-1.0},{1.0,-1.0,-1.0},
                      {1.0,1.0,-1.0}, {-1.0,1.0,-1.0}, {-1.0,-1.0,1.0},
                      {1.0,-1.0,1.0}, {1.0,1.0,1.0}, {-1.0,1.0,1.0}};

float colors[][3] =  {{0.0,0.0,0.0},{1.0,0.0,0.0},
                      {1.0,1.0,0.0}, {0.0,1.0,0.0}, {0.0,0.0,1.0},
                      {1.0,0.0,1.0}, {1.0,1.0,1.0}, {0.0,1.0,1.0}};


float verticesHAHA[][4] = {{-1.0, -1.0, -1.0, 1.}, // 0
						{ 1.0, -1.0, -1.0, 1.}, // 1
                       { 1.0,  1.0, -1.0, 1.}, // 2
                       {-1.0,  1.0, -1.0, 1.}, // 3
                       {-1.0, -1.0,  1.0, 1.}, // 4
                       {1.0,  -1.0,  1.0, 1.}, // 5
                       {1.0,   1.0,  1.0, 1.}, // 6
                       {-1.0,  1.0,  1.0, 1.}};// 7					  
					  
void polygon(Render *face, int a, int b, int c, int d) {
	
	// First triangle

	vector<glm::vec4> triangle = vector<glm::vec4>();
	triangle.push_back({vertices[a][0], vertices[a][1], vertices[a][2], 1.});
	triangle.push_back({vertices[b][0], vertices[b][1], vertices[b][2], 1.});
	triangle.push_back({vertices[c][0], vertices[c][1], vertices[c][2], 1.});
	
  	vector<glm::vec4> colours = vector<glm::vec4>();
	colours.push_back({colors[a][0], colors[a][1], colors[a][2], 1.});
	colours.push_back({colors[b][0], colors[b][1], colors[b][2], 1.});
	colours.push_back({colors[c][0], colors[c][1], colors[c][2], 1.});
	
  
	// second triangle
	triangle.push_back({vertices[a][0], vertices[a][1], vertices[a][2], 1.});
	triangle.push_back({vertices[d][0], vertices[d][1], vertices[d][2], 1.});
	triangle.push_back({vertices[c][0], vertices[c][1], vertices[c][2], 1.});
	
	colours.push_back({colors[a][0], colors[a][1], colors[a][2], 1.});
	colours.push_back({colors[d][0], colors[d][1], colors[d][2], 1.});
	colours.push_back({colors[c][0], colors[c][1], colors[c][2], 1.});
	
	
	face->add(triangle, colours);
}

void cube(Render *C){
  polygon(C, 0, 3, 2, 1);
  polygon(C, 2, 3, 7, 6);
  polygon(C, 0, 4, 7, 3);
  polygon(C, 1, 2, 6, 5);
  polygon(C, 4, 5, 6, 7);
  polygon(C, 0, 1, 5, 4);
}


int main(int argc, char *argv[]) {
  Render p;
  StartGraphics("Solid Cube");
  World.DefaultColour = WHITE;
  World.ZBuffer = true;
  cube(&p);      // generate faces, setup colours, same per vertex.

  float time;
  while(Animate(&time)) {
    float angle = 0.05 * time;
    Lock();     // needed for SDL 1.2, ignored for SDL 2+, nested locks can cause flickering, as screen is released and aquired.
    ClearScreen();
    glm::vec3 axis = glm::vec3(0., 1., 1.);
    p.Transform = glm::rotate(glm::mat4(1.), angle, axis);
    p.draw();
    /*
       while(!TestKey(' ')){
       ;
       }
     */
    UnLock();
    // break;
  }
  StopGraphics(false);
}
