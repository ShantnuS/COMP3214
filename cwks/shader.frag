#version 400 core

precision highp float; 

in vec3 color;
out vec4 FragColor;

void main(){
	FragColor = vec4(color,1);
}