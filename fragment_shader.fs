#version 330 core

in vec3 pos;    // interpolated from the vertex shader values. 

in vec4 start;
in vec4 stop;

in vec4 clusterIndex;
// Ouput data
//out vec3 color; // pixel/sample colour

layout(location=0) out vec3 color; // pixel/sample colour

uniform vec3 colors[13] = {
	vec3(1.0, 0.0, 0.0),
	vec3(0.0, 1.0, 0.0),
	vec3(0.0, 0.0, 1.0),
	vec3(1.0, 1.0, 0.0),
	vec3(0.0, 1.0, 1.0),
	vec3(1.0, 0.0, 1.0),
	vec3(1.0, 0.5, 0.0),
	vec3(1.0, 0.0, 0.5),
	vec3(0.5, 1.0, 0.0),
	vec3(0.0, 1.0, 0.5),
	vec3(0.5, 0.0, 1.0),
	vec3(0.0, 0.5, 1.0),
	vec3(1.0, 1.0, 1.0)
};

void main() {
	// some periodic coloring function based on fragment position (model space) interpolated from triangle vertices
	vec4 inside = vec4(0.0, 0.0, 0.0, 0.0);

	if (pos.x < stop.x && pos.x > start.x)
		inside.x = 1.0;
	if (pos.x < stop.y && pos.x > start.y)
		inside.y = 1.0;
	if (pos.x < stop.z && pos.x > start.z)
		inside.z = 1.0;
	if (pos.x < stop.w && pos.x > start.w)
		inside.w = 1.0;



	float R, G, B;
	if (gl_FrontFacing) {
		color = vec3(0.5, 0.5, 0.5);
	} else {	
		if (inside.x == 1.0) 
			color = colors[int(clusterIndex.x+0.5)];
		else if (inside.y == 1.0)
			color = colors[int(clusterIndex.y+0.5)];
		else if (inside.z == 1.0)
			color = colors[int(clusterIndex.z+0.5)];
		else if (inside.w == 1.0)
			color = colors[int(clusterIndex.w+0.5)];
		else
			color = colors[12];
	}
		
/*
	float R = 0.5 + 0.4*cos(2*3.14*pos.x);
	float G = 0.6 + 0.3*sin(2*3.14*2*pos.y);
	float B = 0.4 + 0.7*cos(2*3.14*3*pos.z);
    color = vec3(R, G, B);
*/

}
