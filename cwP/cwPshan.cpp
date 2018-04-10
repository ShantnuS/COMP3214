#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "World.h"

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

int main(){
	printf("Hello World");
	return 0;
}