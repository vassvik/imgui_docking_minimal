#version 330 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 instanceSize;
layout(location = 2) in vec3 instancePosition;
layout(location = 3) in float instanceRotation;
layout(location = 4) in vec4 instanceStart;
layout(location = 5) in vec4 instanceStop;
layout(location = 6) in vec4 instanceIndex;

uniform mat4 MVP;
out vec3 pos; // to fragment shader

#define M_PI 3.1415926535897932384626433832795

out vec4 start;
out vec4 stop;
out vec4 clusterIndex;

void main() {
	vec3 new_pos = vertexPosition*instanceSize;
	float cost = cos(instanceRotation*M_PI/180.0);
	float sint = sin(instanceRotation*M_PI/180.0);
	float x = new_pos.x * cost - new_pos.y * sint;
	float y = new_pos.x * sint + new_pos.y * cost;
	new_pos.x = x;
	new_pos.y = y;


	new_pos = new_pos + instancePosition;
	gl_Position = MVP * vec4(new_pos, 1.0);
	pos = vertexPosition;
	
	start = instanceStart*2-1;
	stop = instanceStop*2-1;
	clusterIndex = instanceIndex;
}

