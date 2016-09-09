#version 330 core
layout(location=0) in vec3 in_position;
layout(location=1) in vec2 in_texture;

out vec2 ex_texture;

uniform mat4 transform;

void main(){
    gl_Position = transform * vec4(in_position, 1);
    ex_texture = in_texture;
}