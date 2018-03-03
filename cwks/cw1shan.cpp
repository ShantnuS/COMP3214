#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include "opengl.h"
#include "utils.h"

using namespace glm;
using namespace std;


GLuint col_b;

GLuint modelHandle;
GLuint viewHandle;
GLuint projectionHandle;
GLuint lightHandle;

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
int currentScreen;

struct Object {
	GLuint vao;
	GLuint vert_b;
	int size;
	mat4 model;
};

struct Normal {
	vec3 normal;
	vec3 direction;
};

Object sphere;
Object sphere2;
Object sphere3;
Object sphere4;

//Error callback  
static void error_callback(int error, const char* description){
	fputs(description, stderr);
	_fgetchar();
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

std::vector<Normal> getNormal(vector<vec3> sphereVectors) {
	vector<vec3> tempVec;
	vector<Normal> returnThing;

	for (int i = 0; i < sphereVectors.size(); i += 3) {
		tempVec.push_back(normalize(cross(sphereVectors[i + 1] - sphereVectors[i], sphereVectors[i + 2] - sphereVectors[i])));
		tempVec.push_back(normalize(cross(sphereVectors[i + 1] - sphereVectors[i], sphereVectors[i + 2] - sphereVectors[i])));
		tempVec.push_back(normalize(cross(sphereVectors[i + 1] - sphereVectors[i], sphereVectors[i + 2] - sphereVectors[i])));
	}

	for (int i = 0; i < sphereVectors.size(); i++) {
		Normal n;
		n.direction = sphereVectors[i];
		n.normal = tempVec[i];
		returnThing.push_back(n);
	}

	return returnThing;
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

	vector<Normal> returnThing = getNormal(sphereVectors);
	return returnThing;
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
			glBindVertexArray(sphere.vao);
			sphere.model = translate(mat4(1), vec3(0, 0, 0)) * rotate(mat4(1), theta, vec3(0, 1, 0));
			glUniformMatrix4fv(modelHandle, 1, GL_FALSE, &sphere.model[0][0]);
			glDrawArrays(GL_LINE_LOOP, 0, sphere.size);
			break;
		//Screen B
		case 2:
			glBindVertexArray(sphere.vao);
			sphere.model = translate(mat4(1), vec3(0, 0, 0)) * rotate(mat4(1), theta, vec3(0, 1, 0));
			glUniformMatrix4fv(modelHandle, 1, GL_FALSE, &sphere.model[0][0]);
			glDrawArrays(GL_TRIANGLES, 0, sphere.size);
			break;
		case 3:
			glBindVertexArray(sphere2.vao);
			position.y += 0.0001f;
			mat4 model = translate(mat4(1), position) * rotate(mat4(1), theta, vec3(0, 1, 0));

			sphere2.model = translate(mat4(1), vec3(0,1,0)) * rotate(mat4(1), theta, vec3(0, 1, 0));
			sphere2.model = model * sphere2.model;
			glUniformMatrix4fv(modelHandle, 1, GL_FALSE, &sphere2.model[0][0]);
			glDrawArrays(GL_TRIANGLES, 0, sphere2.size);

			sphere3.model = translate(mat4(1), vec3(0, 0, 0)) * rotate(mat4(1), theta, vec3(0, 1, 0));
			sphere3.model = model * sphere3.model;
			glUniformMatrix4fv(modelHandle, 1, GL_FALSE, &sphere3.model[0][0]);
			glDrawArrays(GL_TRIANGLES, 0, sphere3.size);

			sphere4.model = translate(mat4(1), vec3(0, -1, 0)) * rotate(mat4(1), theta, vec3(0, 1, 0));
			sphere4.model = model * sphere4.model;
			glUniformMatrix4fv(modelHandle, 1, GL_FALSE, &sphere4.model[0][0]);
			glDrawArrays(GL_TRIANGLES, 0, sphere4.size);

			break;
	}

	glBindVertexArray(0);
	glFinish();
}

void handleInit() {
	modelHandle = glGetUniformLocation(program, "model");
	viewHandle = glGetUniformLocation(program, "view");
	projectionHandle = glGetUniformLocation(program, "projection");
	lightHandle = glGetUniformLocation(program, "lightDirection");
}

void init() {
	//Initialise objects
	vector<Normal> object1 = generateSphere(radians(10.0f));
	sphere = bufferInit(object1);

	sphere2 = bufferInit(object1);
	sphere3 = bufferInit(object1);
	sphere4 = bufferInit(object1);
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

int cw1shan_main() {
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
	program = programA;
	
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	handleInit();
	init();

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

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