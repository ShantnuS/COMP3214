#version 400 core

precision highp float; 

in vec3 color;
in vec3 normals;
in vec2 uv;

vec3 ambientColor = vec3(0.0f,0.0f,0.0f);
vec3 diffuseColor = vec3(1.0f,1.0f,1.0f);
uniform vec3 lightDirection;
uniform sampler2D tex;

void main(){
	vec3 ambient = ambientColor; 
	float nld = dot(normals, lightDirection); 
	vec3 diffuse = nld * diffuseColor;
	vec3 resultColor = texture2D(tex,uv).rgb * (diffuse + ambient);
	
	gl_FragColor = vec4(resultColor,1);
}