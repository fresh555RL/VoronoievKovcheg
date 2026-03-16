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

    vec4 Sphere1 = vec4(0.0, 0.0, 0.0, 2.0);

    vec2 dSph = vec2(sdSphere(p,Sphere1), 5);//5
    d = cmp(d,dSph);

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



void MapColor(vec3 p, vec3 ro){
    float coloridx = map(p).y;
    const vec3 colors[1] = vec3[](
       vec3(0.337, 0.969, 0.596)
    );
    vec3 colorBase = colors[int(coloridx)];
    float onigiri = step(0.0, sin(p.x*25)+sin(p.y*25)+sin(p.z*25));

    vec3 even_strong = vec3(0.3);
    vec3 color = colorBase+even_strong*onigiri;
    color = mix(color, vec3(0.83, 0.2, 0.75), step(dPlayer(p, ro),-0.3 ));

    FragColor = vec4(color,1.0);
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