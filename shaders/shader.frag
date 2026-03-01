#version 330 core

uniform float Aspect;
uniform vec3 Cam_pos;
uniform vec3 Cam_up;
uniform vec3 Cam_right;
uniform vec3 Cam_front;
uniform float FOV;
uniform float timenow;
in vec2 _UV;
out vec4 FragColor;

float dSphere(vec3 p, vec4 c){

    return length(p-c.xyz)-c.w;
}

float dCube(vec3 p,vec3 c, vec3 b){
    vec3 q = abs(p-c) - b;
    return length(max(q,0.0)) + min(max(q.x,max(q.y,q.z)),0.0);
}
float sdTorus( vec3 p, vec2 t )
{
  vec2 q = vec2(length(p.xz)-t.x,p.y);
  return length(q)-t.y;
}

float smin(float a, float b, float k)
{
    float h = clamp(0.5 + 0.5*(b - a)/k, 0.0, 1.0);
    return mix(b, a, h) - k*h*(1.0 - h);
}
float map(vec3 p){
    float d = 1e9;
    vec4 Sphere1 = vec4(3.0+sin(timenow)*0, 0.0, -2.0, 1.2);
    vec3 Cube1 = vec3(1.0, 1, 1.0);
    vec3 Cubecoord1 = vec3(3.5, 0.0, -2.0);
    vec2 Torus1 = vec2(1.0, 0.3);
    
    float dS = min(d, dSphere(p,Sphere1));
    float dC = min(d, dCube(p,Cubecoord1,Cube1));
    d = min(dS, dC);

    d = max(-dS, dC);
    // d = min(d, sdTorus(p, Torus1));

    return d;
}
float raymarch(vec3 ro, vec3 rd){
    float t = 0.0;
    const int Steps = 256;
    const float Eps = 0.001;
    const float MaxDist = 100.0;

    for(int i=0;i<Steps;i++){
        vec3 p =ro+rd*t; 
        float d = map(p);

        if(abs(d)<Eps) return t;
        t+=abs(d);
        if(t>MaxDist){
            break;
        }
    }
    return -1.0;
}

vec3 Calcnorm(vec3 p){
    const float Eps = 0.0001;
    return normalize(vec3(
        map(p+vec3(Eps,0.0,0.0))-map(p-vec3(Eps,0.0,0.0)),
        map(p+vec3(0.0,Eps,0.0))-map(p-vec3(0.0,Eps,0.0)),
        map(p+vec3(0.0,0.0,Eps))-map(p-vec3(0.0,0.0,Eps))
        )
    );
}

void main(){
    vec2 uv = _UV;
    uv.x *=Aspect;
    float scale = tan(radians(FOV)*0.5);
    uv*=scale;
    vec3 rd = normalize(uv.x*Cam_right+ uv.y*Cam_up+ Cam_front);
    vec3 ro = Cam_pos;    
    float t = raymarch(ro, rd);



    vec3 p = ro+rd*t;
    vec3 lightDir = normalize(vec3(1.0, 2.0,1.3));
    vec3 n = Calcnorm(p);
    float diff = max(dot(n,lightDir),0.0);
    float ambient = 0.2;
    if(t>0.0){

        vec3 colorBase = vec3(1.0);
        FragColor = vec4(colorBase*(diff*0.9+ ambient), 1.0);
    }    
    else{
        vec3 colorBase = vec3(0.02, 0.47, 0.33);
        FragColor = vec4(colorBase, 1.0);
    }
    // FragColor = vec4(_UV, 0.0, 1.0);

}