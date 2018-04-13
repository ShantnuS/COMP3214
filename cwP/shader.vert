#version 400 core

precision highp float; 

layout(location = 0) in vec3 modelDirections;
layout(location = 1) in vec3 modelNormals;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection; 



out vec3 color;
out vec3 normals;

void main(){

	vec4 tempVec = model * vec4(modelNormals,1);
	normals = tempVec.xyz;

	gl_Position = projection * view * model * vec4(modelDirections,1); //since matrices multiply backwards
	color = vec3(0.5f,0.3f,0.3f);
}