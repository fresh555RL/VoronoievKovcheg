#version 330 core

in vec3 Color;
in vec3 Pos;
out vec4 FragColor;

void main(){
    FragColor = vec4(Color.xyz, 1.0);
        
    // float scale =1;
    // float radius = 0.4;
    // vec3 cell = fract(Pos.xyz * scale);
    // float d = distance(cell, vec3(0.5));
    // float dot = step(d,radius);
    // vec3 color = mix(vec3(1.0), vec3(0.0), dot);
    // FragColor = vec4(vec3(min(d/10.0f, 1.0f),255.0, 255.0 ).xyz, 1.0);
    // FragColor = vec4(color.xyz, 1.0);    
        
    float scale =0.1;
    float radius = scale*0.6;
    vec3 position = vec3(
    mod(abs(Pos.x), scale),
    mod(abs(Pos.y), scale),
    mod(abs(Pos.z), scale)
    );
    float d = distance(position.xyz, vec3(scale/2.0));
    float dot = step(d,radius);
    vec3 color = mix(vec3(1.0), vec3(0.0), dot);
    // FragColor = vec4(vec3(min(d/10.0f, 1.0f),255.0, 255.0 ).xyz, 1.0);
    FragColor = vec4(color.xyz, 1.0);    


   

}