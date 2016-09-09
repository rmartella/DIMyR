#version 330 core
// Input color from the vertex program.
in vec4 ex_Color;
out vec4 out_color;

void main(){
    out_color = ex_Color;
}