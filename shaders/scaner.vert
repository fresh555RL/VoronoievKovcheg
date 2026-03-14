#version 330 core
layout (location = 0) in vec3 Vertexpos;
layout (location = 1) in vec3 Vertexcol;

out vec2 _UV;
uniform mat4 MVP;
void main(){

    _UV = Vertexpos.xy; 
    gl_Position =vec4(Vertexpos.xyz, 1.0);
    
}