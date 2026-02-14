#version 330 core

in vec3 Color;
out vec4 FragColor;
uniform float angle;
void main(){
    float f1 = ((sin(angle)+1.0f)/2.0f);
    float f2 = ((sin(angle*7)+1.0f)/2.0f);
    float f3 = ((sin(angle*31)+1.0f)/2.0f);
    FragColor = vec4(Color.x*f1, Color.y*f2, Color.z*f3,1.0f);
    // FragColor = vec4(Color.xyz, 1.0f);
}