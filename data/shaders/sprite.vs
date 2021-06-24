#version 330 core

// Input vertex data: position and UV
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexUV;

out vec2 UV;

uniform mat4 V;
uniform mat4 M;

void main(){
    gl_Position = V * M * vec4(vertexPosition_modelspace, 1);
    UV = vertexUV;
}
