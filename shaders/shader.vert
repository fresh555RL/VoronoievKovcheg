#version 330 core
layout (location = 0) in vec3 Vertexpos;
layout (location = 1) in vec3 VertexCol;

uniform float screen_aspect;
uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;


out vec3 Color;
void main(){
    
    gl_Position =proj*view*model*vec4(Vertexpos.xyz, 1.0);
    // gl_Position =vec4(Vertexpos.xyz, 1.0);

    Color = (VertexCol);
}