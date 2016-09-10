#version 330 core

in vec2 UV;
uniform sampler2D renderedTexture;
out vec3 color;

void main(){
    color = texture( renderedTexture, UV ).xyz;
}