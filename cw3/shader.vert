#version 400 core

precision highp float; 

layout(location = 0) in vec3 modelDirections;
layout(location = 1) in vec3 modelNormals;
layout(location = 2) in vec2 modelUVs;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection; 



out vec3 color;
out vec3 normals;
out vec2 uv;

void main(){

	vec4 tempVec = model * vec4(modelNormals,1);
	normals = tempVec.xyz;

	uv = modelUVs;

	gl_Position = projection * view * model * vec4(modelDirections,1); //since matrices multiply backwards
	color = vec3(0.0f,0.0f,1.0f);
}