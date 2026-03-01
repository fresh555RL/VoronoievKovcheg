#version 330 core
layout (location = 0) in vec3 Vertexpos;
layout (location = 1) in vec3 Vertexcol;

out vec3 Color;
uniform mat4 MVP;
void main(){

    Color = Vertexcol; 
    gl_Position =MVP*vec4(Vertexpos.xyz, 1.0);
    
}