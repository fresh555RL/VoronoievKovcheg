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
float sdSphere(vec4 c){

    return length(c.xyz)-c.w;
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
float hash(vec3 p) {
    p = fract(p * 0.3183099 + 0.1);
    p *= 17.0;
    return fract(p.x * p.y * p.z * (p.x + p.y + p.z));
}


float noise(vec3 x) {
    vec3 i = floor(x);
    vec3 f = fract(x);
    f = f * f * (3.0 - 2.0 * f);
    return mix(mix(mix(hash(i + vec3(0, 0, 0)), 
                       hash(i + vec3(1, 0, 0)), f.x),
                   mix(hash(i + vec3(0, 1, 0)), 
                       hash(i + vec3(1, 1, 0)), f.x), f.y),
               mix(mix(hash(i + vec3(0, 0, 1)), 
                       hash(i + vec3(1, 0, 1)), f.x),
                   mix(hash(i + vec3(0, 1, 1)), 
                       hash(i + vec3(1, 1, 1)), f.x), f.y), f.z);
}

vec2 cmp(vec2 a, vec2 b){
    return (a.x<b.x?a:b);
}
vec4 getGlow(float minPDist) {
    float mainGlow = minPDist * 0.8;
    mainGlow = pow(mainGlow, 128.0);
    mainGlow = clamp(mainGlow, 0.0, 1.0);
    float outerGlow = minPDist * 0.4;
    outerGlow = pow(outerGlow, 2.0);
    outerGlow = clamp(outerGlow, 0.0, 1.0);
    vec4 glow = vec4(7,7, 9, mainGlow);
    glow += vec4(0, 0, 0, outerGlow);
    glow.a = min(glow.a, 1.0);
   
    return glow;
}
float sphere(vec4 s) {
    return length(s.xyz) - s.w;
}
mat2 rotate(float a) {
    float s = sin(a);
    float c = cos(a);
    return mat2(c, -s, s, c);
}
vec2 map(vec3 p) {
    vec3 diskPos = -p;

    float diskDist = sphere(vec4(diskPos, 5.0));
    diskDist = max(diskDist, diskPos.y - 0.01);
    diskDist = max(diskDist, -diskPos.y - 0.01);
    diskDist = max(diskDist, -sphere(vec4(-p, 1.5) * 10.0));

    if(diskDist < 1.0)
{
    vec3 c = vec3(
        length(diskPos),
        diskPos.y,
        atan(diskPos.z, diskPos.x)
    );

    c *= 10.0;

    float r = length(diskPos);
    float innerFade = smoothstep(2.0, 3.5, r);

    float n = noise(c) * 0.3 + noise(c * 2.5) * 0.15;

    diskDist += n * innerFade;
}

    return vec2(diskDist,1);
}
void MapColor(vec3 p){
    vec2 mapa = map(p);
    const float Eps = 0.012;//= raymarch_eps+eps
    const vec3 colors[3] = vec3[](
       vec3(-10.0),
       vec3(1.0),
       vec3(0.0, 1.0, 0.0)
    );
    if(mapa.x<Eps){
        float onigiri = step(0.0, sin(p.x*25)+sin(p.y*25)+sin(p.z*25));
        vec3 even_strong = vec3(0.3);

       //- even_strong*onigiri
        FragColor = vec4(colors[int(mapa.y)]-even_strong*onigiri, 1.0);
    }
    else FragColor = vec4(0.11, 0.08, 0.28, 1.0);

}
vec4 raymarch(vec3 ro, vec3 rd){
    int Steps = Marches;
    const float Eps = 0.01;
    const float MaxDist = 100.0;

    float glow = 0.0;
    vec3 p = ro;

    for(int i=0;i<Steps;i++){         
        vec2 d = map(p);

        glow = max(glow , 1.0/(d.x+1.0));

        if(d.x > MaxDist) break;

        vec3 bdir = normalize(-p);
        float bdist = length(p);
        bdist = pow(bdist + 1.0, 2.0);
        bdist = d.x / bdist;

        rd = normalize(mix(rd, bdir, bdist));
        p += rd * d.x;

        if(d.x < Eps){
            return vec4(p, glow);  
        }
    }

    return vec4(p, glow);
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
    uv *= rotate(0.5);
    vec3 rd = normalize(uv.x*Cam_right+ uv.y*Cam_up+ Cam_front);
    vec3 ro = Cam_pos;    
    // vec2 t = raymarch(ro, rd);


    // vec3 p = ro+rd*t.x;
    // const vec3 colors[3] = vec3[](
    //    vec3(0.0),
    //    vec3(1.0),
    //    vec3(0.0, 1.0, 0.0)
    // );
    // if(t.x>0.0){
    //     vec3 colorBase = colors[int(t.y)];

    //     float onigiri = step(0.0, sin(p.x*25)+sin(p.y*25)+sin(p.z*25));
    //     vec3 even_strong = vec3(0.3);

    //    //- even_strong*onigiri
    //     FragColor = vec4(colorBase ,1.0);

    //     // FragColor = vec4(vec3(abs(k)), 1.0);
    // }    
    // else{
        
    //     vec3 colorBase = vec3(0.02, 0.47, 0.33);
    //     FragColor = vec4(colorBase, 1.0);
    // }
    // // FragColor = vec4(_UV, 0.0, 1.0);
    
    vec4 res = raymarch(ro, rd);
    vec3 p = res.xyz;
    float glow = res.w;

    // базовый цвет
    MapColor(p);

    // добавляем glow сверху
    vec4 g = getGlow(glow);
    FragColor.rgb += g.rgb * g.a;
}