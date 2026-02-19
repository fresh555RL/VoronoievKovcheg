#version 330 core
layout (location = 0) in vec3 Vertexpos;

uniform mat4 MVP;
uniform vec3 VertexCol;
uniform vec2 VertexTexCoord;
out vec3 Color;
// out vec2 TexCoord;
void main(){
    
    gl_Position =MVP*vec4(Vertexpos.xyz, 1.0);
    Color = VertexCol;
    // TexCoord = VertexTexCoord;
}