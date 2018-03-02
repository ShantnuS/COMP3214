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

float theta = 0.0f;
float zoom = 5;
bool spinRight = true;

GLuint program;
int currentScreen;

struct Object {
	GLuint vao;
	GLuint vert_b;
	int size;
};

Object sphere;

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

std::vector<vec3> generateSphere(float step) {

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

	return sphereVectors;
}

Object bufferInit(vector<vec3> vp) {

	Object myObj;
	vec3* v = vp.data();
	myObj.size = vp.size();

	glGenVertexArrays(1, &myObj.vao);
	glBindVertexArray(myObj.vao);

	glGenBuffers(1, &myObj.vert_b);

	glBindBuffer(GL_ARRAY_BUFFER, myObj.vert_b);
	glBufferData(GL_ARRAY_BUFFER, vp.size() * sizeof(glm::vec3), v, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
	return myObj;
}

void draw() {
	if (spinRight) {
		theta += 0.0001f;
	}
	else {
		theta -= 0.0001f;
	}

	mat4 model = translate(mat4(1), vec3(0, 0, 0)) * rotate(mat4(1), theta, vec3(0,1,0) )  ;
	mat4 view = lookAt(vec3(zoom, 0, 0), vec3(0,0,0), vec3(0,1,0));
	mat4 projection =perspective(radians(90.0f), 16.0f / 9.0f, 0.1f, 100.0f);

	glUniformMatrix4fv(modelHandle, 1, GL_FALSE, &model[0][0]);
	glUniformMatrix4fv(viewHandle, 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(projectionHandle, 1, GL_FALSE, &projection[0][0]);

	switch(currentScreen){
		//Screen A
		case 1:
			glBindVertexArray(sphere.vao);
			glDrawArrays(GL_LINE_LOOP, 0, sphere.size);
			break;
		//Screen B
		case 2:
			glBindVertexArray(sphere.vao);
			glDrawArrays(GL_TRIANGLES, 0, sphere.size);
			break;
	}

	glBindVertexArray(0);
	glFinish();
}

void init() {
	//Initialise objects
	vector<vec3> object1 = generateSphere(radians(10.0f));
	sphere = bufferInit(object1);
}


static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {

	//Close
	if ((key == GLFW_KEY_ESCAPE || key == GLFW_KEY_Q) && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	//Screens
	if ((key == GLFW_KEY_A) && action == GLFW_PRESS) {
		currentScreen = 1;
		printf("Switching to screen 1\n");
	}

	if ((key == GLFW_KEY_B) && action == GLFW_PRESS) {
		currentScreen = 2;
		printf("Switching to screen 2\n");
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
}

int handleInit() {
	modelHandle = glGetUniformLocation(program, "model");
	viewHandle = glGetUniformLocation(program, "view");
	projectionHandle = glGetUniformLocation(program, "projection");
	return 0;
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

	program = LoadShader("shader.vert", "shader.frag");
	glUseProgram(program);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	handleInit();
	init();

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glEnable(GL_CULL_FACE);


	while (!glfwWindowShouldClose(window)){
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		draw();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}