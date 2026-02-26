#version 330 core
layout (location = 0) in vec3 Vertexpos;

uniform mat4 MVP;
uniform mat4 model;
uniform vec3 VertexCol;
out vec3 Color;
out vec3 Pos;
void main(){

     
    Pos = (model * vec4(Vertexpos,1.0)).xyz;
    gl_Position =MVP*vec4(Vertexpos.xyz, 1.0);
    Color = VertexCol;
}