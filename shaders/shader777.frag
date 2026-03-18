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

// --- SDF ---

float sdSphere(vec3 p, float r){
    return length(p) - r;
}

// аккреционный диск (без шума)
float sdDisk(vec3 p)
{
    float disk = sdSphere(p, 5.0);

    // тонкий слой
    disk = max(disk, abs(p.y) - 0.02);

    // внутренняя пустота
    float inner = sdSphere(p, 1.5);

    return max(disk, -inner);
}

// --- scene ---

vec2 map(vec3 p){
    float d = sdDisk(p);
    return vec2(d, 0.0);
}

// --- raymarch с гравитацией ---

vec2 raymarch(vec3 ro, vec3 rd){
    vec2 t = vec2(0.0, 0.0);

    const int Steps = 400;
    const float MaxDist = 100.0;
    const float Eps = 0.001;

    vec3 p = ro;

    for(int i = 0; i < Steps; i++){
        float dS = map(p).x;

        float distToCenter = length(p);

        // 💥 ГРАВИТАЦИЯ (сохраняем!)
        vec3 bendDir = normalize(-p);
        float bend = dS / pow(distToCenter + 1.0, 2.0);

        rd = mix(rd, bendDir, bend);

        // шаг
        float stepSize = min(dS, distToCenter) * 0.05;

        p += rd * max(stepSize, 0.01);
        t.x += stepSize;

        // попадание
        if(abs(dS) < Eps){
            t.y = 0.0;
            return t;
        }

        // "поглощение" в центре (чёрная дыра)
        if(distToCenter < 1.0){
            return vec2(-2.0, 0.0); // специальный код
        }

        if(t.x > MaxDist) break;
    }

    return vec2(-1.0, 0.0);
}

// --- нормаль ---

vec3 Calcnorm(vec3 p){
    const float e = 0.001;
    return normalize(vec3(
        map(p + vec3(e,0,0)).x - map(p - vec3(e,0,0)).x,
        map(p + vec3(0,e,0)).x - map(p - vec3(0,e,0)).x,
        map(p + vec3(0,0,e)).x - map(p - vec3(0,0,e)).x
    ));
}

// --- main ---

void main(){
    vec2 uv = _UV;
    uv.x *= Aspect;

    float scale = tan(radians(FOV)*0.5);
    uv *= scale;

    vec3 rd = normalize(uv.x * Cam_right + uv.y * Cam_up + Cam_front);
    vec3 ro = Cam_pos;

    vec2 t = raymarch(ro, rd);

    // попадание в сингулярность
    if(t.x == -2.0){
        FragColor = vec4(0.0, 0.0, 0.0, 1.0);
        return;
    }

    if(t.x > 0.0){
        vec3 p = ro + rd * t.x;
        vec3 n = Calcnorm(p);

        vec3 lightDir = normalize(vec3(0.5, 1.0, 0.3));
        float diff = max(dot(n, lightDir), 0.0);

        vec3 baseColor = vec3(1.0, 0.7, 0.3);

        FragColor = vec4(baseColor * diff, 1.0);
    }
    else{
        // фон (можно потом текстуру космоса воткнуть)
        FragColor = vec4(0.01, 0.01, 0.02, 1.0);
    }
}