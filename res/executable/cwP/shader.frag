#version 400 core

precision highp float; 

in vec3 color;
in vec3 normals;

vec3 ambientColor = vec3(0.2f,0.0f,0.0f);
vec3 diffuseColor = vec3(1.0f,1.0f,1.0f);
uniform vec3 lightDirection;

void main(){
	vec3 ambient = ambientColor; 
	//vec3 diffuse = clamp(dot(normals, lightDirection), 0 , 1) * diffuseColor;
	//vec3 resultColor = color * (diffuse + ambient);
	gl_FragColor = vec4(color,1);
	//gl_FragColor = vec4(resultColor,1);
}