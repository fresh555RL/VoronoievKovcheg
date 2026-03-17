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



float opSmoothUnion( float a, float b, float k )
{
    k *= 4.0;
    float h = max(k-abs(a-b),0.0);
    return min(a, b) - h*h*0.25/k;
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
float sdTorus( vec3 p, vec2 t )
{
    p.x-=2.0;
  vec2 q = vec2(length(p.xz)-t.x,p.y);
  return length(q)-t.y;
}
float sdRoundBox( vec3 p, vec3 b, float r )
{
  vec3 q = abs(p) - b + r;
  return length(max(q,0.0)) + min(max(q.x,max(q.y,q.z)),0.0) - r;
}
float sdCylinder( vec3 p, vec3 c )
{
  return length(p.xz-c.xy)-c.z;
}
float sdFlatWorld(vec3 p){
    return p.y;
}
float sdHyperbolicParaboloid(vec3 p, float a, float b) {
    float f = (p.x*p.x)/(a*a) - (p.y*p.y)/(b*b) - p.z;
    float gradNorm = length(vec2(2.0*p.x/(a*a), -2.0*p.y/(b*b)));
    return f / sqrt(gradNorm*gradNorm + 1.0);
}

float sdMobius(vec3 p)
{
    float R = 1.5;
    float w = 1.0;
    float t = 0.2;

    float a = atan(p.y, p.x);
    float r = length(p.xy);

    float ca = cos(a*0.5);
    float sa = sin(a*0.5);

    vec2 q = vec2(r - R, p.z);

    mat2 m = mat2(ca,-sa,sa,ca);
    q = m * q;

    vec2 d = abs(q) - vec2(w, t);
    return min(max(d.x,d.y),0.0) + length(max(d,0.0));
}

vec2 cmp(vec2 a, vec2 b){
    return (a.x<b.x?a:b);
}
vec2 map(vec3 p){
    vec2 d = vec2(1e9, 0);

    d = cmp(d, vec2(sdMobius(p), 0));

    return d;
}
vec2 raymarch(vec3 ro, vec3 rd){
    vec2 t = vec2(0.0, 0);
    const int Steps = 1024;
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

float curvature(vec3 p)
{
    float e = 0.008;

    vec3 n  = Calcnorm(p);

    vec3 nx = Calcnorm(p + vec3(e,0,0));
    vec3 ny = Calcnorm(p + vec3(0,e,0));
    vec3 nz = Calcnorm(p + vec3(0,0,e));

    return (nx.x - n.x +
            ny.y - n.y +
            nz.z - n.z) / e;
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
       vec3(0.8, 0.8, 0.8),
       vec3(1.0, 0.0, 0),
       vec3(0.0, 1.0, 0.0)
    );
    if(t.x>0.0){
        vec3 colorBase = colors[int(t.y)];

        float onigiri = step(0.0, sin(p.x*25)+sin(p.y*25)+sin(p.z*25));
        vec3 even_strong = vec3(0.3);

        float k = curvature(p);
        k = tanh((k)*5.0);

        float convex  = clamp(k,0.0, 0.7);
        float concave = clamp(-k,0.0, 0.7);

        colorBase = mix(colorBase, colors[1], concave);
        colorBase = mix(colorBase, colors[2], convex);
        
        FragColor = vec4(colorBase + even_strong*onigiri,1.0);

        // FragColor = vec4(vec3(abs(k)), 1.0);
    }    
    else{
        
        vec3 colorBase = vec3(0.02, 0.47, 0.33);
        FragColor = vec4(colorBase, 1.0);
    }
    // FragColor = vec4(_UV, 0.0, 1.0);

}