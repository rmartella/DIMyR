#version 330 core
// Input color from the vertex program.
in vec3 ex_color;
out vec4 out_color;

//uniform sampler2D sampler;

void main(){
    out_color = vec4(ex_color, 1.0);
}