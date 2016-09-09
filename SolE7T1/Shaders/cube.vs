#version 330 core
layout(location=0) in vec3 in_position;
layout(location=1) in vec3 in_color;

out vec4 ex_Color;

uniform mat4 transform;

void main(){
    gl_Position = transform * vec4(in_position, 1);
    ex_Color = vec4(in_color, 1);
}