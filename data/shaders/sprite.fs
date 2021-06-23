#version 330 core
in vec2 UV;

out vec3 color;

uniform sampler2D diffuse_texture;

void main(){
    // color = texture(diffuse_texture, UV).rgb;
    color = vec3(0, 0, 0);
}
