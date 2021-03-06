//Coursework 3 - Shantnu Singh 

//Header files
#include "mylibs.h"
#include "World.h"

// tiny_obj_loader
#define TINYOBJLOADER_IMPLEMENTATION // only define in one file
#include "tiny_obj_loader.h"

//Bullet Physics 
btBroadphaseInterface* broadphase;
btDefaultCollisionConfiguration* collisionConfiguration;
btCollisionDispatcher* dispatcher;
btSequentialImpulseConstraintSolver* solver;
btDiscreteDynamicsWorld* dynamicsWorld;

//Moving items and static items
std::vector<btRigidBody*> MovingBits; // so that can get at all bits
std::vector<btRigidBody*> StaticBits; // especially during clean up.
std::vector<btRigidBody*> AsteroidBits; //

//Handles 
GLuint modelHandle;
GLuint viewHandle;
GLuint projectionHandle;
GLuint lightHandle;
GLuint texHandle;

//Programs 
GLuint program;
GLuint programDefault;
GLuint programSkybox;

//Camera 
glm::vec3 lightDirection = glm::vec3(1.0f, 0.0f, 0.0f);
float zoom = WORLDSIZE;
float theta = 0.1f;
float leftright = 0.0f;
float updown = 0.0f;
float tilt = 1.0f;
float speed = 0.0005f;
bool tour = true;
bool tempScene = false;
glm::mat4 view;
glm::vec3 coordinate;
glm::vec3 looking;
LerperSequencer camera;
int asteroidNumber=10;

//Structs 
struct Object {
	GLuint vao;
	GLuint vert_b;
	int size;
	glm::mat4 model;
	GLuint texID;
	glm::vec3 position;
};
struct Normal {
	glm::vec3 normal;
	glm::vec3 direction;
	glm::vec2 uv;
};

//Objects 
Object sphere1; //JUPITER
Object sphere2; //VENUS
Object sphere3; //METEOR
Object sphere4; //EARTH
Object sphere5; //MOON
Object sphere6; //MARS
Object model1; //UFO

Object boundarySphere; //BACKGROUND STARS

GLuint backgroundTex;
GLuint asteroidTex;

std::vector<Object> asteroids;

static void error_callback(int error, const char* description) {
	fputs(description, stderr);
	_fgetchar();
}

glm::vec2 getPolar(glm::vec3 v){
	float r = sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
	return glm::vec2(atan2(v.x , v.y)/glm::pi<float>()*0.5, asin(v.z / r)/glm::pi<float>()-0.5);
}

glm::vec3 getPolarCoordinate(float i, float j) {
	float x = cos(i)*cos(j);
	float y = sin(i)*cos(j);
	float z = sin(j);

	return glm::vec3(x, y, z);
}

std::vector<Normal> getNormal(std::vector<glm::vec3> v) {
	std::vector<glm::vec3> tempVec;
	std::vector<Normal> returnThing;

	for (int i = 0; i < v.size(); i += 3) {
		tempVec.push_back(normalize(cross(v[i + 1] - v[i], v[i + 2] - v[i])));
		tempVec.push_back(normalize(cross(v[i + 1] - v[i], v[i + 2] - v[i])));
		tempVec.push_back(normalize(cross(v[i + 1] - v[i], v[i + 2] - v[i])));
	}

	std::vector<glm::vec2> texCoord;
	for (int i = 0; i < v.size(); i++) {
		texCoord.push_back(getPolar(v[i]));
	}

	for (int i = 0; i < v.size(); i++) {
		Normal n;
		n.direction = v[i];
		n.normal = tempVec[i];
		n.uv = texCoord[i];
		returnThing.push_back(n);
	}

	return returnThing;
}

std::vector<Normal> generateCube(){
	std::vector<glm::vec3> v;
	for (int i = 0; i < 36; i++)
		v.push_back(glm::vec3(cube_VB[i * 3], cube_VB[i * 3 + 1], cube_VB[i * 3 + 2]));

	return getNormal(v);
}

std::vector<Normal> generateSphere(float step) {

	std::vector<glm::vec3> sphereVectors;

	for (float i = 0; i < glm::radians(360.0f); i += step) {
		for (float j = 0; j < glm::radians(360.0f); j += step) {
			//Triangle 1
			sphereVectors.push_back(getPolarCoordinate(i, j)); //A
			sphereVectors.push_back(getPolarCoordinate(i + step, j)); //B
			sphereVectors.push_back(getPolarCoordinate(i + step, j + step)); //C
			//Triangle 2
			sphereVectors.push_back(getPolarCoordinate(i + step, j + step)); //C
			sphereVectors.push_back(getPolarCoordinate(i, j + step)); //D
			sphereVectors.push_back(getPolarCoordinate(i, j)); //A
		}
	}
	return getNormal(sphereVectors);
}

std::vector<Normal> generateModel(const char *filename, glm::vec3 move, glm::vec3 rotate) {
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::vector< glm::vec3 > vertices;

	printf("Loading object located at : %s \n", filename);
	std::string obj_err = 
		tinyobj::LoadObj(shapes, materials, filename, NULL);
	printf("Object has been loaded \n");

	for (int i = 0; i < shapes.size(); i++)
		for (int j = 0; j < shapes[i].mesh.indices.size(); j++)
			vertices.push_back(glm::quat(rotate) * glm::vec3(
				shapes[i].mesh.positions[shapes[i].mesh.indices[j] * 3] + move.x,
				shapes[i].mesh.positions[shapes[i].mesh.indices[j] * 3 + 1] + move.y,
				shapes[i].mesh.positions[shapes[i].mesh.indices[j] * 3 + 2] + move.z
			));
	printf("Readied the object vertex \n", filename);

	printf("Object ready file loaded: \n");
	return getNormal(vertices);
}

std::vector<Normal> generateSkybox(float step) {

	std::vector<glm::vec3> sphereVectors;

	for (float i = 0; i < glm::radians(360.0f); i += step) {
		for (float j = 0; j < glm::radians(360.0f); j += step) {
			//Triangle 1
			sphereVectors.push_back(getPolarCoordinate(i + step, j + step));
			sphereVectors.push_back(getPolarCoordinate(i + step, j));
			sphereVectors.push_back(getPolarCoordinate(i, j));
			//Triangle 2	
			sphereVectors.push_back(getPolarCoordinate(i + step, j + step));
			sphereVectors.push_back(getPolarCoordinate(i, j + step));
			sphereVectors.push_back(getPolarCoordinate(i, j));
		}
	}
	return getNormal(sphereVectors);
}

btRigidBody* SetSphere(float size, btTransform T, btVector3 velocity, btScalar _mass) {
	btCollisionShape* fallshape = new btSphereShape(size);
	btDefaultMotionState* fallMotionState = new btDefaultMotionState(T);
	btScalar mass = _mass;
	btVector3 fallInertia(0, 0, 0);
	fallshape->calculateLocalInertia(mass, fallInertia);
	btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI(mass, fallMotionState, fallshape, fallInertia);
	btRigidBody* fallRigidBody = new btRigidBody(fallRigidBodyCI);
	fallRigidBody->setLinearVelocity(velocity);
	fallRigidBody->setRestitution(COE);
	dynamicsWorld->addRigidBody(fallRigidBody);
	return fallRigidBody;
}

btRigidBody* SetCube(btVector3 size, btTransform T, btVector3 velocity) {
	btCollisionShape* fallshape = new btBoxShape(size);
	btDefaultMotionState* fallMotionState = new btDefaultMotionState(T);
	btScalar mass = 1;
	btVector3 fallInertia(0, 0, 0);
	fallshape->calculateLocalInertia(mass, fallInertia);
	btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI(mass, fallMotionState, fallshape, fallInertia);
	btRigidBody* fallRigidBody = new btRigidBody(fallRigidBodyCI);
	fallRigidBody->setLinearVelocity(velocity);
	fallRigidBody->setRestitution(COE);
	dynamicsWorld->addRigidBody(fallRigidBody);
	return fallRigidBody;
}

void bullet_init() {
	//CREATE THE WORLD
	broadphase = new btDbvtBroadphase();
	collisionConfiguration = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(collisionConfiguration);
	solver = new btSequentialImpulseConstraintSolver;
	dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);
	dynamicsWorld->setGravity(btVector3(0., GRAVITY, 0));
	
	//GROUND (BOTTOM)
	//btCollisionShape* groundShape = new btStaticPlaneShape(btVector3(0, 1, 0), 1);
	//btDefaultMotionState* groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, -WORLDSIZE, 0)));
	//btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0, groundMotionState, groundShape, btVector3(0, 0, 0));
	//btRigidBody* groundRigidBody = new btRigidBody(groundRigidBodyCI);
	//groundRigidBody->setRestitution(COE);
	//dynamicsWorld->addRigidBody(groundRigidBody);

	////TOP (CEILING)
	//btCollisionShape* topShape = new btStaticPlaneShape(btVector3(0, -1, 0), 1);
	//btDefaultMotionState* topMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, WORLDSIZE, 0)));
	//btRigidBody::btRigidBodyConstructionInfo topRigidBodyCI(0, topMotionState, topShape, btVector3(0, 0, 0));
	//btRigidBody* topRigidBody = new btRigidBody(topRigidBodyCI);
	//topRigidBody->setRestitution(COE);
	//dynamicsWorld->addRigidBody(topRigidBody);

	//JUPITER
	MovingBits.push_back(SetSphere(2., btTransform(btQuaternion(0, 0, 1, 1), btVector3(-1000,0, 0)), btVector3(0, 0, 0), 0));

	//VENUS
	MovingBits.push_back(SetSphere(200., btTransform(btQuaternion(0, 1, 0, 1), btVector3(200, -220, 0)), btVector3(0, 0, 0), 0));
	
	//METEOR 
	MovingBits.push_back(SetSphere(0.01f, btTransform(btQuaternion(1, 0, 0, 1), btVector3(-10, 5, 20)), btVector3(0, -0.5, -1), 1));

	//EARTH 
	MovingBits.push_back(SetSphere(2., btTransform(btQuaternion(0, 1, 0, 1), btVector3(0, -3, 0)), btVector3(0, 0, 0), 0));

	//MOON
	MovingBits.push_back(SetSphere(0.25f, btTransform(btQuaternion(0, 1, 0, 1), btVector3(0, -3, 5)), btVector3(-0.1, 0, 0), 0));

	//MARS
	MovingBits.push_back(SetSphere(2.0f, btTransform(btQuaternion(0, 1, 0, 1), btVector3(-100, 50, 0)), btVector3(0, 0, 0), 0));

	//UFO
	//MovingBits.push_back(SetSphere(0.01f, btTransform(btQuaternion(1, 0, 0, 1), btVector3(0, 5, 20)), btVector3(0, -0.1, -0.1)));
}

glm::vec3 bullet_step(int i) {
	btTransform trans;
	btRigidBody* moveRigidBody;
	int n = MovingBits.size();
	moveRigidBody = MovingBits[i];
	dynamicsWorld->stepSimulation(1 / 200.f, 10);
	//dynamicsWorld->stepSimulation(0.01, 5);
	moveRigidBody->getMotionState()->getWorldTransform(trans);
	return glm::vec3(trans.getOrigin().getX(), trans.getOrigin().getY(), trans.getOrigin().getZ());
}

glm::vec3 asteroid_step(int i) {
	btTransform trans;
	btRigidBody* moveRigidBody;
	int n = AsteroidBits.size();
	moveRigidBody = AsteroidBits[i];
	dynamicsWorld->stepSimulation(1 / 200.f, 10);
	//dynamicsWorld->stepSimulation(0.01, 5);
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

Object bufferInit(std::vector<Normal> vp) {

	Object myObj;
	Normal* v = vp.data();
	myObj.size = vp.size();

	glGenVertexArrays(1, &myObj.vao);
	glBindVertexArray(myObj.vao);

	glGenBuffers(1, &myObj.vert_b);

	glBindBuffer(GL_ARRAY_BUFFER, myObj.vert_b);
	glBufferData(GL_ARRAY_BUFFER, vp.size() * sizeof(struct Normal), v, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(struct Normal), (GLvoid*)offsetof(struct Normal, direction));
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(struct Normal), (GLvoid*)offsetof(struct Normal, normal));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(struct Normal), (GLvoid*)offsetof(struct Normal, uv));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);
	return myObj;
}

void initLerper() {
	camera.sequence.clear();
	Lerper part1 = Lerper(glm::vec3(WORLDSIZE,0,0), glm::vec3(5, 0, 0), speed, 0.5f);
	Lerper part2 = Lerper(glm::vec3(5, 0, 0), glm::vec3(400, 0, 0), speed, 0.1f);
	Lerper part3 = Lerper(glm::vec3(400, 0, 0), glm::vec3(400, -50, 0), speed, 0.5f);
	Lerper part4 = Lerper(glm::vec3(400, -50, 0), glm::vec3(350, -50, 0), speed, 0.5f);
	Lerper part5 = Lerper(glm::vec3(350, -50, 0), glm::vec3(600, -50, 0), speed);
	Lerper part6 = Lerper(glm::vec3(600, -50, 0), glm::vec3(500, -225, 0), speed, 0.5f);
	Lerper part7 = Lerper(glm::vec3(500, -225, 0), glm::vec3(700, -225, 0), speed);
	Lerper part8 = Lerper(glm::vec3(700, -225, 0), glm::vec3(700, -500, 0), speed);
	Lerper part9 = Lerper(glm::vec3(700, -500, 0), glm::vec3(500, -700, 0), speed);
	Lerper part10 = Lerper(glm::vec3(500, -700, 0), glm::vec3(500, 0, 0), speed);
	Lerper part11 = Lerper(glm::vec3(500, 0, 0), glm::vec3(WORLDSIZE, 0, 0), speed);
	camera.addLerper(part1);
	camera.addLerper(part2);
	camera.addLerper(part3);
	camera.addLerper(part4);
	camera.addLerper(part5);
	camera.addLerper(part6);
	camera.addLerper(part7);
	camera.addLerper(part8);
	camera.addLerper(part9);
	camera.addLerper(part10);
	camera.addLerper(part11);
}

void resetCameraAttributes() {
	lightDirection = glm::vec3(1.0f, 0.0f, 0.0f);
	zoom = WORLDSIZE;
	theta = 0.1f;
	leftright = 0.0f;
	updown = 0.0f;
	tilt = 1.0f;
	speed = 0.0005f;
}

void asteroidInitPhysics() {
	for (int i = 0; i < asteroidNumber; i++) {
		AsteroidBits.push_back(SetSphere(0.01f, btTransform(btQuaternion(1, 0, 0, 1), btVector3(-10, RandomFloat(-10, 10), RandomFloat(-10, 10))), btVector3(1, RandomFloat(-0.5, 0.5), RandomFloat(-0.5, 0.5)), 1));
	}
}

void resetAnimations() {
	MovingBits.clear();
	AsteroidBits.clear();
	bullet_init();
	asteroidInitPhysics();
}

void resetLerper() {
	camera.reset();
	resetCameraAttributes();
	initLerper();
	resetAnimations();
}

glm::vec3 stepCamera(float step) {
	return camera.lerpStepSmooth(step);
}

void asteroidInit() {
	printf("Generating asteroids\n");
	std::vector<Normal> objectSphere = generateSphere(glm::radians(5.0f));
	for (int i = 0; i < asteroidNumber; i++) {
		Object asteroid;
		asteroid = bufferInit(objectSphere);
		asteroid.texID = asteroidTex;
		asteroids.push_back(asteroid);
	}
	asteroidInitPhysics();
}

void handleInit() {
	modelHandle = glGetUniformLocation(program, "model");
	viewHandle = glGetUniformLocation(program, "view");
	projectionHandle = glGetUniformLocation(program, "projection");
	lightHandle = glGetUniformLocation(program, "lightDirection");
	texHandle = glGetUniformLocation(program, "tex");
}


void init() {
	//Initialise objects
	printf("Initialising objects\n");
	//Sphere object
	std::vector<Normal> objectSphere = generateSphere(glm::radians(1.0f));
	std::vector<Normal> objectSphereHigh = generateSphere(glm::radians(0.5f));
	std::vector<Normal> objectCube = generateCube();
	std::vector<Normal> objectSkybox = generateSphere(glm::radians(5.0f));
	//std::vector<Normal> objectUFO = generateModel("alien.obj", glm::vec3(1, 0, 0), glm::vec3(1, 0, 0));

	//Spheres and cubes init
	sphere1 = bufferInit(objectSphere);
	sphere2 = bufferInit(objectSphereHigh);
	sphere3 = bufferInit(objectSphere);
	sphere4 = bufferInit(objectSphere);
	sphere5 = bufferInit(objectSphere);
	sphere6 = bufferInit(objectSphere);
	//model1 = bufferInit(objectUFO);
	boundarySphere = bufferInit(objectSkybox);

	asteroidInit();

	//Textures 
	sphere1.texID = loadTexture("texture/jupiter.jpg");
	sphere2.texID = loadTexture("texture/venus.jpg");
	sphere3.texID = asteroidTex;
	sphere4.texID = loadTexture("texture/earth.jpg");
	sphere5.texID = loadTexture("texture/moon.jpg");
	sphere6.texID = loadTexture("texture/mars.jpg");
	//model1.texID = loadTexture("texture/meteor.bmp");
	boundarySphere.texID = backgroundTex;

	boundarySphere.position= glm::vec3(0, 0, 0);
}

void drawSkyBox(Object object) {
	//glDisable(GL_CULL_FACE);
	object.model = glm::translate(glm::mat4(1), glm::vec3(0, 0, 0)) * glm::rotate(glm::mat4(1), 0.1f, glm::vec3(1, 0, 0)) * glm::scale(glm::mat4(1), glm::vec3(WORLDSIZE*100, WORLDSIZE*100, WORLDSIZE*100));
	glUniformMatrix4fv(modelHandle, 1, GL_FALSE, &object.model[0][0]);

	glUniform1i(texHandle, object.texID);
	glActiveTexture(GL_TEXTURE0 + object.texID);
	glBindTexture(GL_TEXTURE_2D, object.texID);

	glBindVertexArray(object.vao);
	glDrawArrays(GL_TRIANGLES, 0, object.size);
	glBindVertexArray(0);

	glActiveTexture(GL_TEXTURE0 + object.texID);
	glBindTexture(GL_TEXTURE_2D, GL_TEXTURE0);
	glFinish();
	//glEnable(GL_CULL_FACE);
}


void drawBackground() {
	//    if these lines were still there, i get a black screen
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// background render

	glDisable(GL_DEPTH_TEST); ///!!!!
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0f, 1024.0, 512.0, 0.0, 0.0, 1.f);

	glEnable(GL_TEXTURE_2D); // se ugotovi kam dat
	glBindTexture(GL_TEXTURE_2D, backgroundTex);

	glBegin(GL_QUADS);
	glTexCoord2d(0.0, 0.0); glVertex2d(0.0, 0.0);
	glTexCoord2d(1.0, 0.0); glVertex2d(1024.0, 0.0);
	glTexCoord2d(1.0, 1.0); glVertex2d(1024.0, 512.0);
	glTexCoord2d(0.0, 1.0); glVertex2d(0.0, 512.0);
	glEnd();

	glDisable(GL_TEXTURE_2D);
}

void drawObject(Object object, float scale) {
	object.model = (glm::translate(glm::mat4(1), object.position) * glm::rotate(glm::mat4(1), theta, glm::vec3(0, 1, 0)) * glm::scale(glm::mat4(1), glm::vec3(scale, scale, scale)));
	glUniformMatrix4fv(modelHandle, 1, GL_FALSE, &object.model[0][0]);

	glUniform1i(texHandle, object.texID);
	glActiveTexture(GL_TEXTURE0 + object.texID);
	glBindTexture(GL_TEXTURE_2D, object.texID);

	glBindVertexArray(object.vao);
	glDrawArrays(GL_TRIANGLES, 0, object.size);
	glBindVertexArray(0);

	glActiveTexture(GL_TEXTURE0 + object.texID);
	glBindTexture(GL_TEXTURE_2D, GL_TEXTURE0);
	glFinish();
}


void draw() {
	//glm::mat4 view = glm::lookAt(glm::vec3(zoom, pan, leftright ), glm::vec3(zoom-WORLDSIZE, pan, leftright), glm::vec3(0, tilt, 0));
	//glm::vec3 newPos = stepCamera();
	//zoom = newPos.x;
	//pan = newPos.y;
	//leftright = newPos.z;
	
	if (tour) {
		coordinate = stepCamera(speed);
	}
	looking = glm::vec3(coordinate.x - WORLDSIZE, coordinate.y - updown, coordinate.z - leftright);
	view = glm::lookAt(coordinate, looking, glm::vec3(0, tilt, 0));
	glm::mat4 projection = glm::perspective(glm::radians(90.0f), 16.0f / 9.0f, 0.1f, 100000.0f);
	//lightDirection = glm::quat(glm::vec3(0.0001f, 0.0001f, 0.0001f)) * lightDirection;
	//theta += 0.001f;

	glUniformMatrix4fv(viewHandle, 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(projectionHandle, 1, GL_FALSE, &projection[0][0]);
	glUniform3f(lightHandle, lightDirection.x, lightDirection.y, lightDirection.z);


	//use bullet to move shapes
	sphere1.position = bullet_step(0);
	sphere2.position = bullet_step(1);
	sphere3.position = bullet_step(2);
	sphere4.position = bullet_step(3);
	sphere5.position = bullet_step(4);
	sphere6.position = bullet_step(5);
	//model1.position = bullet_step(6);

	drawSkyBox(boundarySphere);

	//Draw shapes
	drawObject(sphere1, 100); //Jupiter
	drawObject(sphere2, 200); //Venus
	drawObject(sphere3, 0.1f); //Meteor
	drawObject(sphere4, 2); //Earth
	drawObject(sphere5, 0.25f); //Moon
	drawObject(sphere6, 2.0f); //Mars
	//drawObject(model1, 1.25f); //Moon

	
	//Draw asteroids
	for (int i = 0; i < asteroidNumber; i++) {
		asteroids.at(i).position = asteroid_step(i);
		drawObject(asteroids.at(i),0.1f);
	}
}

void goToLocation(float x, float y, float z) {
	coordinate = glm::vec3(x, y, z);
	looking = glm::vec3(x - WORLDSIZE, y - updown, z - leftright);
	view = glm::lookAt(coordinate, looking, glm::vec3(0, tilt, 0));
}

void setCamera(float tZoom, float tPan, float tLeftRight) {
	zoom = tZoom;
	updown = tPan;
	leftright = tLeftRight;
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {

	//Close window
	if ((key == GLFW_KEY_ESCAPE || key == GLFW_KEY_Q) && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	//Camera controls
	if ((key == GLFW_KEY_UP) && action == GLFW_REPEAT || action == GLFW_PRESS) {
		if (speed < 1.0f) {
			speed += 0.0001f;
		}
	}
	if ((key == GLFW_KEY_DOWN) && action == GLFW_REPEAT || action == GLFW_PRESS) {
		if (speed >= 0) {
			speed -= 0.0001f;
		}
		if (speed < 0) {
			speed = 0;
		}
	}
	if ((key == GLFW_KEY_LEFT || key == GLFW_KEY_A) && action == GLFW_REPEAT || action == GLFW_PRESS) {
		//Turn camera to left 
		leftright -= 0.8f;
	}
	if ((key == GLFW_KEY_RIGHT || key == GLFW_KEY_D) && action == GLFW_REPEAT || action == GLFW_PRESS) {
		//Turn camera to right 
		leftright += 0.8f;
	}
	if ((key == GLFW_KEY_PAGE_UP || key == GLFW_KEY_W) && action == GLFW_REPEAT || action == GLFW_PRESS) {
		//Pan Up
		updown -= 0.8f;
	}
	if ((key == GLFW_KEY_PAGE_DOWN || key == GLFW_KEY_S) && action == GLFW_REPEAT || action == GLFW_PRESS) {
		//Pan Down
		updown += 0.8f;
	}


	//Other controls
	if ((key == GLFW_KEY_P) && action == GLFW_PRESS) {
		//Camera to picture location
		if (!tempScene) {
			tempScene = true;
			tour = false;
			goToLocation(WORLDSIZE, 0, 0);
		}
		else {
			tempScene = false;
			tour = true;
		}
	}
	if ((key == GLFW_KEY_Y) && action == GLFW_PRESS) {
		resetLerper();
	}
	if ((key == GLFW_KEY_T) && action == GLFW_PRESS) {
		//Start or Pause tour
		tour = !tour;
	}
	if ((key == GLFW_KEY_H) && action == GLFW_PRESS) {
		//Display help on screen out printed on console
		printHelp();
	}
	if ((key == GLFW_KEY_R) && action == GLFW_PRESS) {
		//Reset Animations
		resetAnimations();
	}
	if ((key == GLFW_KEY_I) && action == GLFW_PRESS) {
		//Print attributes
		printf("X: %f, Y: %f, Z: %f, Tilt: %f\n", coordinate.x, coordinate.y, coordinate.z, tilt);
	}
}

int main(){
	glfwSetErrorCallback(error_callback);
	glfwInit();

	GLFWwindow* window;
	window = glfwCreateWindow(1920 * 3 / 4, 1080 * 3 / 4, "Coursework 3 - Shantnu Singh", NULL, NULL);
	if (!window) {
		printf("Failed to open window");
		glfwTerminate();
		return 0;
	}
	glfwMakeContextCurrent(window);

	glfwSetKeyCallback(window, key_callback);
	glewInit();

	programDefault = LoadShader("shader.vert", "shader.frag");
	//programSkybox = LoadShader("skybox.vert", "skybox.frag");
	program = programDefault;
	backgroundTex = loadTexture("texture/stars.jpg");
	asteroidTex= loadTexture("texture/meteor.jpg");

	bullet_init();
	handleInit();
	init();
	initLerper();

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_CULL_FACE);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);	//Black Background
	//glClearColor(1.0f, 1.0f, 1.0f, 0.0f);	// White Background

	while (!glfwWindowShouldClose(window)) {
		glUseProgram(program);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		draw(); //DRAW!

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	bullet_close();

	return 0;
}
