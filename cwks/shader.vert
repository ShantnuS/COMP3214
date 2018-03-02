#version 400 core

precision highp float; 

layout(location = 0) in vec3 modelVertex;
layout(location = 1) in vec3 colorVertex;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection; 

out vec3 color;

void main(){
	mat4 mvp = projection * view * model; //since matrices multiply backwards
	gl_Position = mvp * vec4(modelVertex, 1);
	color = colorVertex;
}