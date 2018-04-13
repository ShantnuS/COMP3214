//Coursework P - Shantnu Singh 

//Basics 
#include <stdio.h>
#include <fstream>
#include <sstream>

// OpenGL (GLFW/GLEW)
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// GLM (basics)
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Bullet Physics (basics)
#include <btBulletDynamicsCommon.h>

// tiny_obj_loader
#define TINYOBJLOADER_IMPLEMENTATION // only define in one file
#include <tiny_obj_loader.h>

// stb_image
#define STB_IMAGE_IMPLEMENTATION // only define in one file
#include <stb_image.h>

//World header file
#include "World.h"

btBroadphaseInterface* broadphase;
btDefaultCollisionConfiguration* collisionConfiguration;
btCollisionDispatcher* dispatcher;
btSequentialImpulseConstraintSolver* solver;
btDiscreteDynamicsWorld* dynamicsWorld;

std::vector<btRigidBody*> MovingBits; // so that can get at all bits
std::vector<btRigidBody*> StaticBits; // especially during clean up.

GLuint modelHandle;
GLuint viewHandle;
GLuint projectionHandle;
GLuint lightHandle;
GLuint texHandle;

GLuint program;

glm::vec3 lightDirection = glm::vec3(1.0f, 0.0f, 0.0f);
float zoom =10;
float theta = 0.0f;

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

Object sphere1;
Object sphere2;
Object cube1;

GLfloat cube_VB[] = {
	-1.0f,-1.0f,-1.0f,
	-1.0f,-1.0f, 1.0f,
	-1.0f, 1.0f, 1.0f, 
	1.0f, 1.0f,-1.0f, 
	-1.0f,-1.0f,-1.0f,
	-1.0f, 1.0f,-1.0f,
	1.0f,-1.0f, 1.0f,
	-1.0f,-1.0f,-1.0f,
	1.0f,-1.0f,-1.0f,
	1.0f, 1.0f,-1.0f,
	1.0f,-1.0f,-1.0f,
	-1.0f,-1.0f,-1.0f,
	-1.0f,-1.0f,-1.0f,
	-1.0f, 1.0f, 1.0f,
	-1.0f, 1.0f,-1.0f,
	1.0f,-1.0f, 1.0f,
	-1.0f,-1.0f, 1.0f,
	-1.0f,-1.0f,-1.0f,
	-1.0f, 1.0f, 1.0f,
	-1.0f,-1.0f, 1.0f,
	1.0f,-1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f,-1.0f,-1.0f,
	1.0f, 1.0f,-1.0f,
	1.0f,-1.0f,-1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f,-1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f,-1.0f,
	-1.0f, 1.0f,-1.0f,
	1.0f, 1.0f, 1.0f,
	-1.0f, 1.0f,-1.0f,
	-1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	-1.0f, 1.0f, 1.0f,
	1.0f,-1.0f, 1.0f
};

static void error_callback(int error, const char* description) {
	fputs(description, stderr);
	_fgetchar();
}

GLuint LoadShader(const char * vertex_file_path, const char * fragment_file_path) {

	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
	if (VertexShaderStream.is_open()) {
		std::string Line = "";
		while (getline(VertexShaderStream, Line))
			VertexShaderCode += "\n" + Line;
		VertexShaderStream.close();
	}
	else {
		printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertex_file_path);
		getchar();
		return 0;
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
	if (FragmentShaderStream.is_open()) {
		std::string Line = "";
		while (getline(FragmentShaderStream, Line))
			FragmentShaderCode += "\n" + Line;
		FragmentShaderStream.close();
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;


	// Compile Vertex Shader
	printf("Compiling shader : %s\n", vertex_file_path);
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}



	// Compile Fragment Shader
	printf("Compiling shader : %s\n", fragment_file_path);
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		printf("%s\n", &FragmentShaderErrorMessage[0]);
	}



	// Link the program
	printf("Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}


	glDetachShader(ProgramID, VertexShaderID);
	glDetachShader(ProgramID, FragmentShaderID);

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}

glm::vec2 getPolar(glm::vec3 v){
	float r = sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
	return glm::vec2(atan(v.y / v.x), acos(v.z / r));
}

glm::vec3 getPolarCoordinate(float i, float j) {
	float x = cos(i)*sin(j);
	float y = sin(i)*sin(j);
	float z = cos(j);

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
			sphereVectors.push_back(getPolarCoordinate(i, j));
			sphereVectors.push_back(getPolarCoordinate(i + step, j));
			sphereVectors.push_back(getPolarCoordinate(i + step, j + step));
			//Triangle 2
			sphereVectors.push_back(getPolarCoordinate(i + step, j + step));
			sphereVectors.push_back(getPolarCoordinate(i, j + step));
			sphereVectors.push_back(getPolarCoordinate(i, j));
		}
	}
	return getNormal(sphereVectors);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {

	//Close
	if ((key == GLFW_KEY_ESCAPE || key == GLFW_KEY_Q) && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	//Zoom
	if ((key == GLFW_KEY_UP) && action == GLFW_REPEAT || action == GLFW_PRESS) {
		if (zoom > 0) {
			zoom -= 0.1f;
			printf("%f\n", zoom);
		}
	}
	if ((key == GLFW_KEY_DOWN) && action == GLFW_REPEAT || action == GLFW_PRESS) {
		zoom += 0.1f;
		printf("%f\n", zoom);
	}

}

btRigidBody* SetSphere(float size, btTransform T) {
	btCollisionShape* fallshape = new btSphereShape(size);
	btDefaultMotionState* fallMotionState = new btDefaultMotionState(T);
	btScalar mass = 1;
	btVector3 fallInertia(0, 0, 0);
	fallshape->calculateLocalInertia(mass, fallInertia);
	btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI(mass, fallMotionState, fallshape, fallInertia);
	btRigidBody* fallRigidBody = new btRigidBody(fallRigidBodyCI);
	fallRigidBody->setLinearVelocity(btVector3(0, 0, 0));
	fallRigidBody->setRestitution(COE);
	dynamicsWorld->addRigidBody(fallRigidBody);
	return fallRigidBody;
}

btRigidBody* SetCube(float size, btTransform T) {
	btCollisionShape* fallshape = new btSphereShape(size);
	btDefaultMotionState* fallMotionState = new btDefaultMotionState(T);
	btScalar mass = 1;
	btVector3 fallInertia(0, 0, 0);
	fallshape->calculateLocalInertia(mass, fallInertia);
	btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI(mass, fallMotionState, fallshape, fallInertia);
	btRigidBody* fallRigidBody = new btRigidBody(fallRigidBodyCI);
	fallRigidBody->setLinearVelocity(btVector3(0, 0, 0));
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
	dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);
	dynamicsWorld->setGravity(btVector3(0., GRAVITY, 0));
	/*
	* Set up ground
	*/
	btCollisionShape* groundShape = new btStaticPlaneShape(btVector3(0, 1, 0), 1);
	btDefaultMotionState* groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, -10, 0)));
	btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0, groundMotionState, groundShape, btVector3(0, 0, 0));
	btRigidBody* groundRigidBody = new btRigidBody(groundRigidBodyCI);
	groundRigidBody->setRestitution(COE);
	dynamicsWorld->addRigidBody(groundRigidBody);

	/*
	btCollisionShape* leftShape = new btStaticPlaneShape(btVector3(1, 0, 0), 1);
	btDefaultMotionState* leftMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(-10, 0, 0)));
	btRigidBody::btRigidBodyConstructionInfo leftRigidBodyCI(0, leftMotionState, leftShape, btVector3(0, 0, 0));
	btRigidBody* leftRigidBody = new btRigidBody(leftRigidBodyCI);
	leftRigidBody->setRestitution(COE);
	dynamicsWorld->addRigidBody(leftRigidBody);
	
	btCollisionShape* rightShape = new btStaticPlaneShape(btVector3(-1, 0, 0), 1);
	btDefaultMotionState* rightMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(10, 0, 0)));
	btRigidBody::btRigidBodyConstructionInfo rightRigidBodyCI(0, rightMotionState, rightShape, btVector3(0, 0, 0));
	btRigidBody* rightRigidBody = new btRigidBody(rightRigidBodyCI);
	rightRigidBody->setRestitution(COE);
	dynamicsWorld->addRigidBody(rightRigidBody);
	

	btCollisionShape* topShape = new btStaticPlaneShape(btVector3(0, -1, 0), 1);
	btDefaultMotionState* topMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 10, 0)));
	btRigidBody::btRigidBodyConstructionInfo topRigidBodyCI(0, topMotionState, topShape, btVector3(0, 0, 0));
	btRigidBody* topRigidBody = new btRigidBody(topRigidBodyCI);
	topRigidBody->setRestitution(COE);
	dynamicsWorld->addRigidBody(topRigidBody);
	*/

	//Sphere1
	MovingBits.push_back(SetSphere(1., btTransform(btQuaternion(0, 0, 1, 1), btVector3(0, 0, 0))));

	/*
	//Sphere2
	MovingBits.push_back(SetSphere(1., btTransform(btQuaternion(0, 1, 0, 1), btVector3(-3, 0, 0))));
	
	//Cube1
	MovingBits.push_back(SetCube(1., btTransform(btQuaternion(1, 0, 0, 1), btVector3(-5, 0, 0))));
	*/
}

glm::vec3 bullet_step(int i) {
	btTransform trans;
	btRigidBody* moveRigidBody;
	int n = MovingBits.size();
	moveRigidBody = MovingBits[i];
	dynamicsWorld->stepSimulation(1 / 60.f, 10);
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

void handleInit() {
	modelHandle = glGetUniformLocation(program, "model");
	viewHandle = glGetUniformLocation(program, "view");
	projectionHandle = glGetUniformLocation(program, "projection");
	lightHandle = glGetUniformLocation(program, "lightDirection");
	texHandle = glGetUniformLocation(program, "tex");
}

void init() {
	//Initialise objects
	//Sphere object
	std::vector<Normal> objectSphere = generateSphere(glm::radians(4.0f));
	std::vector<Normal> objectCube = generateCube();

	//Spheres and cubes init
	sphere1 = bufferInit(objectSphere);
	sphere2 = bufferInit(objectSphere);
	cube1 = bufferInit(objectCube);

	sphere1.position = glm::vec3(0, 0, 0);
	sphere2.position = glm::vec3(0, 0, 0);
	cube1.position = glm::vec3(0, 0, 0);

}

void draw() {
	glm::mat4 view = glm::lookAt(glm::vec3(zoom, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	glm::mat4 projection = glm::perspective(glm::radians(90.0f), 16.0f / 9.0f, 0.1f, 100.0f);

	glUniformMatrix4fv(viewHandle, 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(projectionHandle, 1, GL_FALSE, &projection[0][0]);
	glUniform3f(lightHandle, lightDirection.x, lightDirection.y, lightDirection.z);

	//use bullet to move shapes
	sphere1.position = bullet_step(0);
	//sphere2.position = bullet_step(1);
	//cube1.position = bullet_step(2);

	//printf("%f\n",sphere1.position.x);

	sphere1.model = glm::translate(glm::mat4(1), sphere1.position) * glm::rotate(glm::mat4(1), theta, glm::vec3(0, 1, 0));
	glUniformMatrix4fv(modelHandle, 1, GL_FALSE, &sphere1.model[0][0]);
	glBindVertexArray(sphere1.vao);
	glDrawArrays(GL_TRIANGLES, 0, sphere1.size);
	glBindVertexArray(0);
	glFinish();

	/*
	sphere2.model = glm::translate(glm::mat4(1), sphere2.position) * glm::rotate(glm::mat4(1), theta, glm::vec3(0, 1, 0));
	glUniformMatrix4fv(modelHandle, 1, GL_FALSE, &sphere2.model[0][0]);
	glBindVertexArray(sphere2.vao);
	glDrawArrays(GL_TRIANGLES, 0, sphere2.size);
	glBindVertexArray(0);
	glFinish();

	cube1.model = glm::translate(glm::mat4(1), cube1.position) * glm::rotate(glm::mat4(1), theta, glm::vec3(0, 1, 0));
	glUniformMatrix4fv(modelHandle, 1, GL_FALSE, &cube1.model[0][0]);
	glBindVertexArray(cube1.vao);
	glDrawArrays(GL_TRIANGLES, 0, cube1.size);
	glBindVertexArray(0);
	glFinish();
	*/
}

int main(){
	glfwSetErrorCallback(error_callback);
	glfwInit();

	GLFWwindow* window;
	window = glfwCreateWindow(1920 / 2, 1080 / 2, "Hello", NULL, NULL);
	if (!window) {
		printf("Failed to open window");
		glfwTerminate();
		return 0;
	}
	glfwMakeContextCurrent(window);

	glfwSetKeyCallback(window, key_callback);
	glewInit();

	program = LoadShader("shader.vert", "shader.frag");

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	bullet_init();

	handleInit();
	init();

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_TEXTURE_2D);

	glEnable(GL_CULL_FACE);

	while (!glfwWindowShouldClose(window)) {
		glUseProgram(program);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		draw();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	bullet_close();

	return 0;
}
