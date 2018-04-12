/*
 * This is a very simple example of using Bullet
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "World.h"

// FILE *flog;

const double PI = 3.141592653589793;
const double PIo2 = PI / 2.;
const double PIo4 = PI / 4.;
const double PI2 = PI * 2.;
const float lod = PI / 32.;

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "VertexBufferObject.h"
#include "glslprogram.h"
#include "stb_image.h"
#include "utils.h"
/*
 * Set up bullet - globals.
 */
#include <btBulletDynamicsCommon.h>

btBroadphaseInterface* broadphase;
btDefaultCollisionConfiguration* collisionConfiguration;
btCollisionDispatcher* dispatcher;
btSequentialImpulseConstraintSolver* solver;
btDiscreteDynamicsWorld* dynamicsWorld;

std::vector<btRigidBody*> MovingBits; // so that can get at all bits
std::vector<btRigidBody*> StaticBits; // especially during clean up.
/*
 * Bullet Code
 */
btRigidBody* SetSphere(float size, btTransform T) {
  btCollisionShape* fallshape = new btSphereShape(size);
  btDefaultMotionState* fallMotionState = new btDefaultMotionState(T);
  btScalar mass = 1;
  btVector3 fallInertia(0,0,0);
  fallshape->calculateLocalInertia(mass,fallInertia);
  btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI(mass,fallMotionState,fallshape,fallInertia);
  btRigidBody* fallRigidBody = new btRigidBody(fallRigidBodyCI);
  fallRigidBody->setLinearVelocity(btVector3(-5, 20, 0));
  fallRigidBody->setRestitution(COE);
  dynamicsWorld->addRigidBody(fallRigidBody);
  return fallRigidBody;
}
void bullet_init() {
  /*
   * set up world
   */
  broadphase = new btDbvtBroadphase();
  collisionConfiguration = new btDefaultCollisionConfiguration();
  dispatcher = new btCollisionDispatcher(collisionConfiguration);
  solver = new btSequentialImpulseConstraintSolver;
  dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher,broadphase,solver,collisionConfiguration);
  dynamicsWorld->setGravity(btVector3(0., GRAVITY, 0));
  /*
   * Set up ground
   */
  btCollisionShape* groundShape = new btStaticPlaneShape(btVector3(0,1,0),1);
  btDefaultMotionState* groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),btVector3(0,-1,0)));
  btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0,groundMotionState,groundShape,btVector3(0,0,0));
  btRigidBody* groundRigidBody = new btRigidBody(groundRigidBodyCI);
  groundRigidBody->setRestitution(COE);
  dynamicsWorld->addRigidBody(groundRigidBody);
  /*
   * Set up left
   */
  btCollisionShape* leftShape = new btStaticPlaneShape(btVector3(1,0,0),1);
  btDefaultMotionState* leftMotionState = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),btVector3(-50, 0,0)));
  btRigidBody::btRigidBodyConstructionInfo leftRigidBodyCI(0,leftMotionState,leftShape,btVector3(0,0,0));
  btRigidBody* leftRigidBody = new btRigidBody(leftRigidBodyCI);
  leftRigidBody->setRestitution(COE);
  dynamicsWorld->addRigidBody(leftRigidBody);
  /*
   * Set up right
   */
  btCollisionShape* rightShape = new btStaticPlaneShape(btVector3(-1,0,0),1);
  btDefaultMotionState* rightMotionState = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),btVector3(50, 0,0)));
  btRigidBody::btRigidBodyConstructionInfo rightRigidBodyCI(0,rightMotionState,rightShape,btVector3(0,0,0));
  btRigidBody* rightRigidBody = new btRigidBody(rightRigidBodyCI);
  rightRigidBody->setRestitution(COE);
  dynamicsWorld->addRigidBody(rightRigidBody);
  /*
   * Set up top
   */
  btCollisionShape* topShape = new btStaticPlaneShape(btVector3(0,-1,0),1);
  btDefaultMotionState* topMotionState = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),btVector3(0, 100,0)));
  btRigidBody::btRigidBodyConstructionInfo topRigidBodyCI(0,topMotionState,topShape,btVector3(0,0,0));
  btRigidBody* topRigidBody = new btRigidBody(topRigidBodyCI);
  topRigidBody->setRestitution(COE);
  dynamicsWorld->addRigidBody(topRigidBody);
  /*
   * Set up sphere 0
   */
  MovingBits.push_back(SetSphere(5., btTransform(btQuaternion(0,0,1,1),btVector3(-10,45,0))));
  /*
   * Set up sphere 1
   */
  MovingBits.push_back(SetSphere(5., btTransform(btQuaternion(0,1,0,1),btVector3(-10,25,0))));
  /*
   * Set up sphere 2
   */
  MovingBits.push_back(SetSphere(5., btTransform(btQuaternion(1,0,0,1),btVector3(-10,65,0))));

  Print("Setup Bullet ");
  int n = MovingBits.size();
  print(n);
}
glm::vec3 bullet_step(int i) {
  btTransform trans;
  btRigidBody* moveRigidBody;
  int n = MovingBits.size();
  moveRigidBody = MovingBits[i];
  dynamicsWorld->stepSimulation(1 / 60.f,10);
  moveRigidBody->getMotionState()->getWorldTransform(trans);
  return glm::vec3(trans.getOrigin().getX(), trans.getOrigin().getY(), trans.getOrigin().getZ());
}
void bullet_close() {
  /*
   * This is very minimal and relies on OS to tidy up.
   */
  btRigidBody* moveRigidBody;
  moveRigidBody = MovingBits[0];
  dynamicsWorld->removeRigidBody(moveRigidBody);
  delete moveRigidBody->getMotionState();
  delete moveRigidBody;
  delete dynamicsWorld;
  delete solver;
  delete collisionConfiguration;
  delete dispatcher;
  delete broadphase;
}
void Render(int i, GLSLProgram O, VertexBufferObject vb) {
  glm::mat4 Projection = glm::mat4(1.0f);
  Projection = glm::ortho(-55., 55., -5., 105., -100., 100.);
  glm::mat4 View = glm::mat4(1.);
  glm::mat4 Model = glm::mat4(1.);
  O.Use();
  vb.SelectVAO();
  O.SetUniform("uProjection", Projection);
  O.SetUniform("uView", View);
  for(int i = 0; i < MovingBits.size(); i++) {    // loop over shapes
    glm::vec3 position = bullet_step(i);
    // fprintf(flog, "%f %f %f ", position[0], position[1], position[2]);
    Model = glm::translate(position);
    O.SetUniform("uModel", Model);
    vb.Draw();
  }
  // fprintf(flog, "\n");
  vb.DeSelectVAO();
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
  if ((key == GLFW_KEY_ESCAPE || key == GLFW_KEY_Q) && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GL_TRUE);
}
VertexBufferObject makeWireBoxMesh(void) {
  VertexBufferObject Box;
  Box.vboName = "Box";
  Box.SetVerbose(true);
  Box.CollapseCommonVertices( false );
  Box.SetTol( .001f );  // how close need to be to collapse vertices, ignored at the moment.
  Box.UseBufferObjects(true);   // Not needed as this is the only option.
  Box.glBegin( GL_LINES );

  Box.glVertex3f(0., 0., 0.);
  Box.glVertex3f(-50., 0., 0.);

  Box.glVertex3f(-50., 0., 0.);
  Box.glVertex3f(-50., 100., 0.);

  Box.glVertex3f(-50., 100., 0.);
  Box.glVertex3f(50., 100., 0.);

  Box.glVertex3f(50., 100., 0.);
  Box.glVertex3f(50., 0., 0.);

  Box.glVertex3f(50., 0., 0.);
  Box.glVertex3f(0., 0., 0.);
  Box.glEnd();
  Box.Print();
  return Box;
}

VertexBufferObject makeWireCircleMesh(float radius) {
  VertexBufferObject Circle;
  Circle.vboName = "Circle";
  Circle.SetVerbose(true);
  Circle.CollapseCommonVertices( false );
  Circle.SetTol( .001f );  // how close need to be to collapse vertices, ignored at the moment.
  Circle.UseBufferObjects(true);   // Not needed as this is the only option.
  float theta;
  const float Radius = radius;
  Circle.glBegin( GL_LINES );
  glm::vec3 p, n;
  for(theta = 0.0f; theta < PI2; theta += lod) {
    Circle.glVertex3f(Radius * cos(theta), Radius * sin(theta), 0.);
    Circle.glVertex3f(Radius * cos(theta + lod), Radius * sin(theta + lod), 0.);
  }
  Circle.glEnd();
  Circle.Print();
  Circle.makeObj("Circle.obj");
  return Circle;
}

int main( void ) {
  int k = 0;
  bool good;
  GLFWwindow* window;
  if( !glfwInit() ) {
    printf("Failed to start GLFW\n");
    exit( EXIT_FAILURE );
  }
  window = glfwCreateWindow(640, 480, "Visible axies", NULL, NULL);
  if (!window) {
    glfwTerminate();
    printf("GLFW Failed to start\n");
    return -1;
  }
  /* Make the window's context current */
  glfwMakeContextCurrent(window);   // IMPORTANT: Must be done so glew recognises OpenGL
  glewExperimental = GL_TRUE;
  int err = glewInit();
  if (GLEW_OK != err) {
    /* Problem: glewInit failed, something is seriously wrong. */
    fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
  }
  fprintf(stderr, "Glew done\n");
  glfwSetKeyCallback(window, key_callback);
  fprintf(stderr, "GL INFO %s\n", glGetString(GL_VERSION));
  /*
   * Vertex information
   */
  VertexBufferObject Circle = makeWireCircleMesh(5.);
  VertexBufferObject Box  = makeWireBoxMesh();
  /*
   * shader programs for circle.
   */
  GLSLProgram O2;
  O2.SetVerbose(true);
  O2.SetGstap(false);
  good = O2.Create("box.vert", "box.frag");
  if( !good ) {
    fprintf( stderr, "GLSL Program wasn�t created.\n" );
    exit(0);
  }
  Circle.SelectVAO();
  O2.SetAttribute( "aPosition", Circle, VERTEX_ATTRIBUTE );   // or NORMAL_ATTRIBUTE or TEXTURE_ATTRIBUTE or ...
  Circle.DeSelectVAO();
  /*
   * shader programs for boundary.
   */
  GLSLProgram O3;
  O3.SetVerbose(true);
  O3.SetGstap(false);
  good = O3.Create("box.vert", "box.frag");
  if( !good ) {
    fprintf( stderr, "GLSL Program wasn�t created.\n" );
    exit(0);
  }
  Box.SelectVAO();
  O3.SetAttribute( "aPosition", Box, VERTEX_ATTRIBUTE );   // or NORMAL_ATTRIBUTE or TEXTURE_ATTRIBUTE or ...
  Box.DeSelectVAO();


  bullet_init();   // set up physics

  glEnable(GL_DEPTH_TEST);
  Check("Before render loop");
  glClearColor(0.0, 0., 0., 1.0);  /* Make our background black */
  // flog = fopen("velocity.log", "w");
  print("Starting to render");
  while(!glfwWindowShouldClose(window)) {   // Main loop
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glm::mat4 Projection = glm::mat4(1.0f);
    Projection = glm::ortho(-55., 55., -5., 105., -100., 100.);
    glm::mat4 View = glm::mat4(1.);
    glm::mat4 Model = glm::mat4(1.);
    O3.Use();
    O3.SetUniform("uProjection", Projection);
    O3.SetUniform("uView", View);
    O3.SetUniform("uModel", Model);
    Box.SelectVAO();
    Box.Draw();
    Box.DeSelectVAO();
    O2.Use();
    k++;
    Render(k, O2, Circle);
    glFlush();
    glfwSwapBuffers(window);    // Swap front and back rendering buffers
    glfwPollEvents();     // Poll for events.
  }
  void bullet_close();
  glfwTerminate();  // Close window and terminate GLFW
  exit( EXIT_SUCCESS );  // Exit program
}
