#version 330 core

in vec3 pos;    // interpolated from the vertex shader values. 

// Ouput data
//out vec3 color; // pixel/sample colour

layout(location=0) out vec3 color; // pixel/sample colour

void main() {
	// some periodic coloring function based on fragment position (model space) interpolated from triangle vertices
	float R = 0.5 + 0.4*cos(2*3.14*pos.x);
	float G = 0.6 + 0.3*sin(2*3.14*2*pos.y);
	float B = 0.4 + 0.7*cos(2*3.14*3*pos.z);
    color = vec3(R, G, B);

}
