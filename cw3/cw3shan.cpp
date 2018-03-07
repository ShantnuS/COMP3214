#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include "opengl.h"
#include "utils.h"
#include "stb_image\stb_image.h"

using namespace glm;
using namespace std;


GLuint col_b;

GLuint modelHandle;
GLuint viewHandle;
GLuint projectionHandle;
GLuint lightHandle;
GLuint texHandle;

vec3 lightDirection = vec3(1.0f, 0.0f, 0.0f);
float theta = 0.0f;
float zoom = 5;
bool spinRight = true;
bool lightOn = false;
bool rotateOn = true;
vec3 position = vec3(0.0f, -3.0f, 0.0f);

GLuint program;
GLuint programA;
GLuint programB;
GLuint programD;
int currentScreen;

struct Object {
	GLuint vao;
	GLuint vert_b;
	int size;
	mat4 model;
	GLuint texID;
};

struct Normal {
	vec3 normal;
	vec3 direction;
	vec2 uv;
};

Object sphere;
Object rocketCone;
Object rocketBody;
Object rocketPayload;
Object rocketBody2;
Object rocketBottom;
Object texSphere;

//Cube vertex data array
GLfloat cube_VB[] = {
	-1.0f,-1.0f,-1.0f, // triangle 1 : begin
	-1.0f,-1.0f, 1.0f,
	-1.0f, 1.0f, 1.0f, // triangle 1 : end
	1.0f, 1.0f,-1.0f, // triangle 2 : begin
	-1.0f,-1.0f,-1.0f,
	-1.0f, 1.0f,-1.0f, // triangle 2 : end
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

glm::vec2 getPolar(glm::vec3 v)
{
	float r = sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
	return glm::vec2(atan(v.y / v.x), acos(v.z / r));
}

//Error callback  
static void error_callback(int error, const char* description){
	fputs(description, stderr);
	_fgetchar();
}

GLuint loadTexture(const char *fname){
	int w, h, n;
	unsigned char *data = stbi_load(fname, &w, &h, &n, 0);

	GLuint tex = 1;
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	delete data;
	return tex;
}

vec3 getPolarCoordinate(float i, float j) {
	float x = cos(i)*sin(j);
	float y = sin(i)*sin(j);
	float z = cos(j);

	return vec3(x, y, z);
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

std::vector<Normal> getNormal(vector<vec3> v) {
	vector<vec3> tempVec;
	vector<Normal> returnThing;

	for (int i = 0; i < v.size(); i += 3) {
		tempVec.push_back(normalize(cross(v[i + 1] - v[i], v[i + 2] - v[i])));
		tempVec.push_back(normalize(cross(v[i + 1] - v[i], v[i + 2] - v[i])));
		tempVec.push_back(normalize(cross(v[i + 1] - v[i], v[i + 2] - v[i])));
	}

	std::vector<vec2> texCoord;
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

std::vector<Normal> generateCube()
{
	std::vector<glm::vec3> v;
	for (int i = 0; i < 36; i++)
		v.push_back(glm::vec3(cube_VB[i * 3], cube_VB[i * 3 + 1], cube_VB[i * 3 + 2]));

	return getNormal(v);
}

std::vector<Normal> generateCone(float size) {
	std::vector<vec3> v;

	//
	// generate cone
	//
	int lod = 32;
	float step = 2. * 3.141596 / float(lod);
	float Radius = 1.;
	float c, s, c2, s2;
	for (float a = 0; a <= (2.0f * 3.141596f); a += step)
	{
		v.push_back(vec3(0.0f,0.0f,0.0f));
		c = cos(a);
		s = sin(a);
		c2 = cos(a - step);
		s2 = sin(a - step);
		v.push_back(vec3(c, s, size));
		v.push_back(vec3(c2, s2, size));
	}

	return getNormal(v);
}

std::vector<Normal> generateCylinder(float size) {

	std::vector<vec3> v;
	int lod = 32;
	float step = 2. * 3.141596 / float(lod);
	float Radius = 1.;
	float c, s, c2, s2;

	for (float a = 0; a > -(2. * 3.141596); a -= step)
	{
		c = Radius * cos(a);
		s = Radius * sin(a);
		c2 = Radius * cos(a - step);
		s2 = Radius * sin(a - step);

		v.push_back(vec3(c, s, 0.0f));
		v.push_back(vec3(c, s, size));
		v.push_back(vec3(c2, s2, size));
		v.push_back(vec3(c2, s2, size));
		v.push_back(vec3(c2, s2, 0));
		v.push_back(vec3(c, s, 0.0f));
	}

	return getNormal(v);
}

std::vector<Normal> generateSphere(float step) {

	std::vector<vec3> sphereVectors; 

	for (float i = 0; i < radians(360.0f); i+= step) {
		for (float j = 0; j < radians(360.0f); j += step) {
			//Triangle 1
			sphereVectors.push_back(getPolarCoordinate(i,j));
			sphereVectors.push_back(getPolarCoordinate(i + step, j));
			sphereVectors.push_back(getPolarCoordinate(i + step, j + step));
			//Triangle 2
			sphereVectors.push_back(getPolarCoordinate(i + step, j + step));
			sphereVectors.push_back(getPolarCoordinate(i, j+step));
			sphereVectors.push_back(getPolarCoordinate(i, j));
		}
	}
	return getNormal(sphereVectors);
}

Object bufferInit(vector<Normal> vp) {

	Object myObj;
	Normal* v = vp.data();
	myObj.size = vp.size();

	glGenVertexArrays(1, &myObj.vao);
	glBindVertexArray(myObj.vao);

	glGenBuffers(1, &myObj.vert_b);

	glBindBuffer(GL_ARRAY_BUFFER, myObj.vert_b);
	glBufferData(GL_ARRAY_BUFFER, vp.size() * sizeof(struct Normal), v, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(struct Normal), (GLvoid*) offsetof(struct Normal, direction));
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(struct Normal), (GLvoid*) offsetof(struct Normal, normal));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(struct Normal), (GLvoid*)offsetof(struct Normal, uv));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);
	return myObj;
}

void draw() {

	if (rotateOn) {
		if (spinRight) {
			theta += 0.0001f;
		}
		else {
			theta -= 0.0001f;
		}
	}

	if (lightOn) {
		lightDirection = quat(vec3(0.0001f, 0.0001f, 0.0001f)) * lightDirection;
	}

	mat4 view = lookAt(vec3(zoom, 0, 0), vec3(0,0,0), vec3(0,1,0));
	mat4 projection =perspective(radians(90.0f), 16.0f / 9.0f, 0.1f, 100.0f);


	glUniformMatrix4fv(viewHandle, 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(projectionHandle, 1, GL_FALSE, &projection[0][0]);
	glUniform3f(lightHandle, lightDirection.x, lightDirection.y, lightDirection.z);

	switch(currentScreen){
		//Screen A
		case 1:
			sphere.model = translate(mat4(1), vec3(0, 0, 0)) * rotate(mat4(1), theta, vec3(0, 1, 0));
			glUniformMatrix4fv(modelHandle, 1, GL_FALSE, &sphere.model[0][0]);
			glBindVertexArray(sphere.vao);
			glDrawArrays(GL_LINE_LOOP, 0, sphere.size);
			glBindVertexArray(0);
			glFinish();
			break;
		//Screen B
		case 2:
			sphere.model = translate(mat4(1), vec3(0, 0, 0)) * rotate(mat4(1), theta, vec3(0, 1, 0));
			glUniformMatrix4fv(modelHandle, 1, GL_FALSE, &sphere.model[0][0]);
			glBindVertexArray(sphere.vao);
			glDrawArrays(GL_TRIANGLES, 0, sphere.size);
			glBindVertexArray(0);
			glFinish();
			break;
		case 3:	
			position.y += 0.001f;
			mat4 model = translate(mat4(1), position) * rotate(mat4(1), theta, vec3(0, 1, 0));

			//Rocket Cone
			rocketCone.model = translate(mat4(1), vec3(0,2,0)) * rotate(mat4(1), theta, vec3(1, 0, 0));
			rocketCone.model = model * rocketCone.model;
			glUniformMatrix4fv(modelHandle, 1, GL_FALSE, &rocketCone.model[0][0]);
			glBindVertexArray(rocketCone.vao);
			glDrawArrays(GL_TRIANGLES, 0, rocketCone.size);
			glBindVertexArray(0);
			glFinish();

			//Rocket Body
			rocketBody.model = translate(mat4(1), vec3(0, 0, 0)) * rotate(mat4(1), theta, vec3(1, 0, 0));
			rocketBody.model = model * rocketBody.model;
			glUniformMatrix4fv(modelHandle, 1, GL_FALSE, &rocketBody.model[0][0]);
			glBindVertexArray(rocketBody.vao);
			glDrawArrays(GL_TRIANGLES, 0, rocketBody.size);
			glBindVertexArray(0);
			glFinish();

			//Rocket Payload
			rocketPayload.model = translate(mat4(1), vec3(0, -2.7f, 0)) * rotate(mat4(1), theta, vec3(0, 1, 0)) * scale(mat4(1), vec3(0.9f, 0.9f, 0.9f));
			rocketPayload.model = model * rocketPayload.model;
			glUniformMatrix4fv(modelHandle, 1, GL_FALSE, &rocketPayload.model[0][0]);
			glBindVertexArray(rocketPayload.vao);
			glDrawArrays(GL_TRIANGLES, 0, rocketPayload.size);
			glBindVertexArray(0);
			glFinish();

			//Rocket Body 2
			rocketBody2.model = translate(mat4(1), vec3(0, -3.5f, 0)) * rotate(mat4(1), theta, vec3(1, 0, 0));
			rocketBody2.model = model * rocketBody2.model;
			glUniformMatrix4fv(modelHandle, 1, GL_FALSE, &rocketBody2.model[0][0]);
			glBindVertexArray(rocketBody2.vao);
			glDrawArrays(GL_TRIANGLES, 0, rocketBody2.size);
			glBindVertexArray(0);
			glFinish();

			//Rocket Bottom
			rocketBottom.model = translate(mat4(1), vec3(0, -4.0f, 0)) * rotate(mat4(1), theta, vec3(1, 0, 0)) * scale(mat4(1), vec3(1.5f, 1.5f, 1.5f));;
			rocketBottom.model = model * rocketBottom.model;
			glUniformMatrix4fv(modelHandle, 1, GL_FALSE, &rocketBottom.model[0][0]);
			glBindVertexArray(rocketBottom.vao);
			glDrawArrays(GL_TRIANGLES, 0, rocketBottom.size);
			glBindVertexArray(0);
			glFinish();

			break;
		case 4: 
			texSphere.model = translate(mat4(1), vec3(0, 0, 0)) * rotate(mat4(1), theta, vec3(0, 1, 0)) * scale(mat4(1), vec3(2,2,2));
			glUniform1i(texHandle, texSphere.texID);
			glUniformMatrix4fv(modelHandle, 1, GL_FALSE, &texSphere.model[0][0]);
			glActiveTexture(GL_TEXTURE0 + texSphere.texID);
			glBindTexture(GL_TEXTURE_2D, texSphere.texID);
			glBindVertexArray(texSphere.vao);
			glDrawArrays(GL_TRIANGLES, 0, texSphere.size);
			glBindVertexArray(0);
			glFinish();
			break;
	}

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
	vector<Normal> objectSphere = generateSphere(radians(4.0f));
	vector<Normal> objectCube = generateCube();
	vector<Normal> objectCone = generateCone(2.0f);
	vector<Normal> objectCylinder = generateCylinder(2.0f);

	//Sphere for part A and B
	sphere = bufferInit(objectSphere);

	//Rocket for part C
	rocketCone = bufferInit(objectCone);
	rocketBody = bufferInit(objectCylinder);
	rocketPayload = bufferInit(objectCube);
	rocketBody2 = bufferInit(objectCylinder);
	rocketBottom = bufferInit(objectCone);

	//Sphere for part D
	texSphere = bufferInit(objectSphere);
	texSphere.texID = loadTexture("texture/sun.png");
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {

	//Close
	if ((key == GLFW_KEY_ESCAPE || key == GLFW_KEY_Q) && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	//Screens
	if ((key == GLFW_KEY_A) && action == GLFW_PRESS) {
		currentScreen = 1;
		program = programA;
		rotateOn = true;
		handleInit();
		printf("Switching to screen A\n");
	}

	if ((key == GLFW_KEY_B) && action == GLFW_PRESS) {
		currentScreen = 2;
		program = programB;
		rotateOn = true;
		handleInit();
		printf("Switching to screen B\n");
	}

	if ((key == GLFW_KEY_C) && action == GLFW_PRESS) {
		currentScreen = 3;
		position = vec3(0.0f, -3.0f, 0.0f);
		program = programB;
		theta = 1.5f;
		rotateOn = false;
		handleInit();
		printf("Switching to screen C\n");
	}

	if ((key == GLFW_KEY_D) && action == GLFW_PRESS) {
		currentScreen = 4;
		program = programD;
		rotateOn = true;
		handleInit();
		printf("Switching to screen D\n");
	}

	//Zoom
	if ((key == GLFW_KEY_UP) && action == GLFW_REPEAT || action == GLFW_PRESS) {
		if (zoom > 0) {
			zoom-= 0.1f;
		}
	}
	if ((key == GLFW_KEY_DOWN) && action == GLFW_REPEAT || action == GLFW_PRESS) {
		zoom+=0.1f;
	}

	//Spin Direction
	if ((key == GLFW_KEY_RIGHT) && action == GLFW_PRESS) {
		spinRight = true;
	}
	if ((key == GLFW_KEY_LEFT) && action == GLFW_PRESS) {
		spinRight = false;
	}

	//Light 
	if ((key == GLFW_KEY_L) && action == GLFW_PRESS) {
		lightOn = !lightOn;
		printf("Toggle Light\n");
	}
}

int cw3shan_main() {
	currentScreen = 1;

	glfwSetErrorCallback(error_callback);
	glfwInit();

	GLFWwindow* window; 
	window = glfwCreateWindow(1920/2, 1080/2, "Hello", NULL, NULL);
	if (!window) {
		printf("Failed to open window");
		glfwTerminate();
		return 0;
	}
	glfwMakeContextCurrent(window);

	glfwSetKeyCallback(window, key_callback);
	glewInit();

	programA = LoadShader("shader/shader.vert", "shader/shader.frag");
	programB = LoadShader("shader/shader2.vert", "shader/shader2.frag");
	programD = LoadShader("shader/shader4.vert", "shader/shader4.frag");

	program = programA;
	
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	handleInit();
	init();

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_TEXTURE_2D);

	glEnable(GL_CULL_FACE);


	while (!glfwWindowShouldClose(window)){
		glUseProgram(program);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		draw();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}