#version 400 core

precision highp float; 

layout(location = 0) in vec3 modelVertex;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection; 

out vec3 color;

void main(){
	gl_Position = projection * view * model * vec4(modelVertex,1); //since matrices multiply backwards
	color = vec3(1.0f,1.0f,1.0f);
}