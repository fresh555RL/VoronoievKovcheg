#version 330 core
layout (location = 0) in vec3 Vertexpos;

out vec2 _UV;
void main(){

     
    gl_Position =vec4(Vertexpos.xyz, 1.0);
    _UV = gl_Position.xy;
}