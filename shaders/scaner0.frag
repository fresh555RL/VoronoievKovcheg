#version 330 core
//for tests
uniform float Aspect;
uniform vec3 Cam_pos;
uniform vec3 dForward;
uniform vec3 dRight;
uniform float Radius;
uniform float timenow;
uniform int MODE;
in vec2 _UV;
out vec4 FragColor;
#define PI 3.14159265359

float sdCapsule(vec2 p, vec2 dir, float L, float r)
{
    vec2 b = dir * L;

    vec2 pa = p;
    vec2 ba = b;

    float h = clamp(dot(pa,ba)/dot(ba,ba), 0.0, 1.0);
    return length(pa - ba*h) - r;
}

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

float sdSphere(vec3 p, vec4 c){return length(p-c.xyz)-c.w;}
float sdTriPrism( vec3 p, vec2 h, vec3 c )
{   vec3 q = abs(p-c);
    return max(q.z-h.y,max(q.x*0.866025+p.y*0.5,-p.y)-h.x*0.5);
}
float sdCube(vec3 p,vec3 c, vec3 b){
    vec3 q = abs(p-c) - b;
    return length(max(q,0.0)) + min(max(q.x,max(q.y,q.z)),0.0);
}
float sdTorus( vec3 p, vec2 t )
{
  vec2 q = vec2(length(p.xz)-t.x,p.y);
  return length(q)-t.y;
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
//SDFs


float bordermap(vec3 p){
    float d = 1e9;
    vec3 BBox1 = vec3(10.0, 20.0, 10.0);
    float dBb = sdRoundBox(p, BBox1, 0.5);
   
    return -dBb;
}
float dPlayer(vec3 p, vec3 now){
    return sdSphere(now, vec4(p, 0.5));
}
vec2 map(vec3 p){
    vec2 d = vec2(1e9, 0);

    vec3 Cylinder1 = vec3(3.0,5.0, 1.0);
    vec3 RBox1 = vec3(10.0, 1.0, 10.0);
   
    vec2 dPSph = vec2(sdSphere(p,vec4(p,0.2)),1);
    vec2 dRb = vec2(sdRoundBox(p, RBox1, 0.5),4);//rad was 0.5
    vec2 dCy =  vec2(sdCylinder(p, Cylinder1),3);
    float dirka = opSmoothSubtraction(dCy.x, dRb.x, 0.3);
    d = cmp(d, vec2(dirka,3));
    
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
    float epstep = 0.02;
    epstep = max(epstep, epstep*(dist/0.25));
    vec3 n= vec3(0.0);
    float bd = bordermap(ro);
    while(t<dist && bd>0.07){
        
        bd = bordermap(ro);
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



void MapColor1(vec3 p, vec3 ro){
    float coloridx = map(p).y;
    const vec3 colors[6] = vec3[](
    vec3(1.0),
    vec3(0.66, 0.07, 0.68),
    vec3(0.86, 0.79, 0.17),
    vec3(0.98, 0.247, 0.008),
    vec3(0.5, 0.5, 0.008),
    vec3(1.0, 0.0, 0.008)
    
    );
    vec3 colorBase = colors[int(coloridx)];
    float onigiri = step(0.0, sin(p.x*25)+sin(p.y*25)+sin(p.z*25));

    colorBase = mix(colorBase, vec3(0.0, 0.0, 0.7), step(p.y, 0.0));
    vec3 even_strong = vec3(0.3);
    vec3 color = colorBase+even_strong*onigiri;
    color = mix(color, vec3(0.83, 0.2, 0.75), step(dPlayer(p, ro),0.0 ));

    FragColor = vec4(color,1.0);
    if(bordermap(p)<0.1){
        FragColor = vec4(colors[5], 1.0);
    }


    
    
    // FragColor = vec4(colorBase+even_strong*space_even, 1.0);
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
    if(1==1){
        MapColor1(point, ro);
    }
    // FragColor = vec4(vec3(length(uv)), 1.0);
    

}