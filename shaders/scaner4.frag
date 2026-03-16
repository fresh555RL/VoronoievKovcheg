#version 330 core

uniform float Aspect;
uniform vec3 Cam_pos;
uniform vec3 dForward;
uniform vec3 dRight;
uniform float Radius;
in vec2 _UV;
out vec4 FragColor;


#define PI 3.14159265359
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

float sdTorus( vec3 p, vec2 t )
{
    p.x-=2.0;
  vec2 q = vec2(length(p.xz)-t.x,p.y);
  return length(q)-t.y;
}
float sdSphere(vec3 p, vec4 c){return length(p-c.xyz)-c.w;}
float sdTriPrism( vec3 p, vec2 h, vec3 c )
{   vec3 q = abs(p-c);
    return max(q.z-h.y,max(q.x*0.866025+p.y*0.5,-p.y)-h.x*0.5);
}
vec2 cmp(vec2 a, vec2 b){return (a.x<b.x?a:b);}
float sdRoundBox( vec3 p, vec3 b, float r )
{
  vec3 q = abs(p) - b + r;
  return length(max(q,0.0)) + min(max(q.x,max(q.y,q.z)),0.0) - r;
}
float sdCylinder( vec3 p, vec3 c )
{
  return length(p.xz-c.xy)-c.z;
}
float sdCube(vec3 p,vec3 c, vec3 b){
    vec3 q = abs(p-c) - b;
    return length(max(q,0.0)) + min(max(q.x,max(q.y,q.z)),0.0);
}
float sdFlatWorld(vec3 p){
    return p.y;
}

float bordermap(vec3 p){
    float d = 1e9;
    vec3 BBox1 = vec3(10.0, 20.0, 10.0);
    float dBb = sdRoundBox(p, BBox1, 0.5);
   
    return -dBb;
}
float dPlayer(vec3 p, vec3 now){
    return sdSphere(now, vec4(p, 0.15));
}
vec2 map(vec3 p){
    vec2 d = vec2(1e9, 0);
    float dG = sdFlatWorld(p);
    vec4 Sphere1 = vec4(5.0, 0.0, 3.0, 1.0);
    vec4 Sphere2 = vec4(-5.0, 0.0, 3.0, 1.0);
    vec4 Sphere3 = vec4(5.0, 0.0, -3.0, 1.0);
    vec4 Sphere4 = vec4(-5.0, 0.0, -3.0, 1.0);
    float dW = opSmoothSubtraction(sdSphere(p, Sphere2), dG, 0.2);
    dW = opSmoothUnion(dW, sdSphere(p,Sphere1), 0.2);
    dW = min(dW, sdSphere(p, Sphere3));
    dW = max(dW, -sdSphere(p, Sphere4));
    d = cmp(d, vec2(dW,0));
    return d;
}


vec3 Calcnorm(vec3 p){
    const float Eps = 0.001;
    return normalize(vec3(
        map(p+vec3(Eps,0.0,0.0)).x-map(p-vec3(Eps,0.0,0.0)).x,
        map(p+vec3(0.0,Eps,0.0)).x-map(p-vec3(0.0,Eps,0.0)).x,
        map(p+vec3(0.0,0.0,Eps)).x-map(p-vec3(0.0,0.0,Eps)).x
        )
    );
}



vec3 mapmarch(vec3 ro, vec3 rd, float dist){
    float t = 0.0;
    float epstep = 0.020;
    epstep = max(epstep, epstep*(dist/0.25));
    vec3 n= vec3(0.0);
    while(t<dist ){
        
        n = Calcnorm(ro);
        float d = map(ro).x;
        ro-=d*n; 
        rd = normalize(rd-dot(rd,n)*n);

        ro+= rd*epstep;
        t+=epstep;
    }
    
    n = Calcnorm(ro);
    float d = map(ro).x;
    ro-=d*n; 
    return ro;
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


void MapColor(vec3 p, vec3 ro){
    const vec3 colors[3] = vec3[](
       vec3(0.8, 0.8, 0.8),
       vec3(1.0, 0.0, 0),
       vec3(0.0, 1.0, 0.0)
    );
     vec3 colorBase = colors[0];

        float onigiri = step(0.0, sin(p.x*25)+sin(p.y*25)+sin(p.z*25));
        vec3 even_strong = vec3(0.3);

        float k = curvature(p);
        k = tanh((k)*5.0);

        float convex  = clamp(k,0.0, 0.7);
        float concave = clamp(-k,0.0, 0.7);

        colorBase = mix(colorBase, colors[1], concave);
        colorBase = mix(colorBase, colors[2], convex);

        FragColor = vec4(colorBase + even_strong*onigiri,1.0);
        FragColor = mix(FragColor, vec4(0.83, 0.2, 0.75, 1.0), step(dPlayer(p, ro),0.0 ));

}

void main(){

    vec2 uv = _UV;
    uv.x *=Aspect;
    
    vec3 normal = Calcnorm(Cam_pos);
    vec3 ro = Cam_pos-map(Cam_pos).x*normal;    
    float dist = Radius * length(uv);
    //надо в кпу эту херь перенаправить
    
    float angle = atan(uv.y, uv.x);
    angle = (angle<0.0? angle+2*PI: angle);
    vec3 rd = cos(angle)*dForward + sin(angle)*dRight;
    //----
    vec3 point = mapmarch(ro, rd, dist);
    MapColor(point, ro);
    

}