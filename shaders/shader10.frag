#version 330 core

uniform float Aspect;
uniform vec3 Cam_pos;
uniform vec3 Cam_up;
uniform vec3 Cam_right;
uniform vec3 Cam_front;
uniform float FOV;
uniform int Marches;
uniform float timenow;
in vec2 _UV;
out vec4 FragColor;



float opSmoothUnion( float a, float b, float k )
{
    k *= 4.0;
    float h = max(k-abs(a-b),0.0);
    return min(a, b) - h*h*0.25/k;
}
float Union(float a, float b){
    return min(a,b);
}
float Substraction(float a, float b){
    return -Union(a,-b);
}
float Intersection(float a, float b){
    return -Union(-a, -b); 
}
float opSmoothSubtraction( float a, float b, float k )
{
    return -opSmoothUnion(a,-b,k);
}

float opSmoothIntersection( float a, float b, float k )
{
    return -opSmoothUnion(-a,-b,k);
}


float sdSphere(vec3 p, vec4 c){

    return length(p-c.xyz)-c.w;
}
float sdTriPrism( vec3 p, vec2 h, vec3 c)
{
  vec3 q = abs(p-c);
  return max(q.z-h.y,max(q.x*0.866025+p.y*0.5,-p.y)-h.x*0.5);
}
float sdCube(vec3 p,vec3 c, vec3 b){
    vec3 q = abs(p-c) - b;
    return length(max(q,0.0)) + min(max(q.x,max(q.y,q.z)),0.0);
}

float sdRoundBox( vec3 p, vec3 b, float r )
{
  vec3 q = abs(p) - b + r;
  return length(max(q,0.0)) + min(max(q.x,max(q.y,q.z)),0.0) - r;
}

vec2 cmp(vec2 a, vec2 b){
    return (a.x<b.x?a:b);
}
vec2 map(vec3 p){
    float d = 1e9;
    vec4 Sphere1 = vec4(5.0, 0.0, 0.0+sin(timenow)*6.0, 2.5);
    vec3 Cube1 = vec3(5.0, 0.0, 0.0);
    float d1 = opSmoothUnion(sdCube(p, Cube1, vec3(1.5)), sdSphere(p, Sphere1), 0.5);
    vec4 Sphere2 = vec4(10.0, 0.0, 0.0+sin(timenow)*6.0, 2.0);
    vec3 Cube2 = vec3(10.0, 0.0, 0.0);
    float d2 = Substraction(sdCube(p, Cube2, vec3(1.5)), sdSphere(p, Sphere2));
    vec4 Sphere3 = vec4(15.0, 0.0, 0.0+sin(timenow*0.7)*3.0, 2.0);
    vec3 Cube3 = vec3(15.0, 0.0, 0.0);
    float d3 = Substraction(sdSphere(p, Sphere3), sdCube(p, Cube3, vec3(1.5)));
    vec4 Sphere4 = vec4(20.0, 0.0, 0.0+sin(timenow*0.7)*3.0, 1.7);
    vec3 Cube4 = vec3(20.0, 0.0, 0.0);
    float d4 = opSmoothSubtraction(sdSphere(p, Sphere4), sdCube(p, Cube4, vec3(1.5)), 0.4);

    // float d3 = opSmoothSubtraction;
    d = min(d, d1);
    d = min(d, d2);
    d = min(d, d3);
    d = min(d, d4);
    // d = cmp(d, vec2(d2, 0));
    // d = cmp(d, vec2(d3, 0));

    return vec2(d,0 );
}
vec2 raymarch(vec3 ro, vec3 rd){
    vec2 t = vec2(0.0, 0);
    int Steps = Marches;
    const float Eps = 0.001;
    const float MaxDist = 100.0;

    for(int i=0;i<Steps;i++){
        vec3 p =ro+rd*t.x; 
        vec2 d = map(p);

        if(abs(d.x)<Eps) return t;
        t.x+=abs(d.x/2.0);
        t.y = d.y;
        if(t.x>MaxDist){
            break;
        }
    }
    return vec2(-1.0, 0);
}

vec3 Calcnorm(vec3 p){
    const float Eps = 0.005;
    return normalize(vec3(
        map(p+vec3(Eps,0.0,0.0)).x-map(p-vec3(Eps,0.0,0.0)).x,
        map(p+vec3(0.0,Eps,0.0)).x-map(p-vec3(0.0,Eps,0.0)).x,
        map(p+vec3(0.0,0.0,Eps)).x-map(p-vec3(0.0,0.0,Eps)).x
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
    vec2 t = raymarch(ro, rd);


    vec3 p = ro+rd*t.x;
    const vec3 colors[3] = vec3[](
       vec3(0.8),
       vec3(1.0, 0.0, 0),
       vec3(0.0, 1.0, 0.0)
    );
    if(t.x>0.0){
        vec3 colorBase = colors[int(t.y)];

        float onigiri = step(0.0, sin(p.x*25)+sin(p.y*25)+sin(p.z*25));
        vec3 even_strong = vec3(0.3);

       
        FragColor = vec4(colorBase + even_strong*onigiri,1.0);

        // FragColor = vec4(vec3(abs(k)), 1.0);
    }    
    else{
        
        vec3 colorBase = vec3(0.02, 0.47, 0.33);
        FragColor = vec4(colorBase, 1.0);
    }
    // FragColor = vec4(_UV, 0.0, 1.0);

}