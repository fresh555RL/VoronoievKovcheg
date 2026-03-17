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

//----------------------------------------
// Простая функция свечения
vec4 getGlow(float minDist) {
    float mainGlow = minDist * 1.2;
    mainGlow = pow(mainGlow, 32.0);
    mainGlow = clamp(mainGlow, 0.0, 1.0);

    float outerGlow = minDist * 0.4;
    outerGlow = pow(outerGlow, 2.0);
    outerGlow = clamp(outerGlow, 0.0, 1.0);

    vec4 glow = vec4(10.0, 5.0, 3.0, mainGlow);
    glow += vec4(0.0, 0.0, 0.0, outerGlow);
    glow.a = min(glow.a, 1.0);
    return glow;
}

//----------------------------------------
// Простая сфера
float sdSphere(vec3 p, vec3 center, float radius) {
    return length(p - center) - radius;
}

//----------------------------------------
// Минимальное расстояние до сцены
float getDist(vec3 p) {
    // Сфера в центре
    float dSphere = sdSphere(p, vec3(0.0), 1.0);
    
    // Диск под сферой
    float diskDist = max(length(p.xz) - 2.0, abs(p.y) - 0.1);
    
    // Возвращаем минимальное расстояние
    return min(dSphere, diskDist);
}

//----------------------------------------
// Рэймарчинг с мягким "притягиванием" к центру
vec4 raymarch(vec3 ro, vec3 rd) {
    vec3 p = ro;
    float glow = 0.0;

    const int Steps = 128;
    const float MaxDist = 100.0;

    for (int i = 0; i < Steps; i++) {
        float d = getDist(p);
        glow = max(glow, 1.0 / (d + 1.0));

        // "Гравитация" к центру
        vec3 toCenter = normalize(-p);
        float coef = clamp(d * 0.05, 0.0, 1.0);
        rd = normalize(mix(rd, toCenter, coef));

        p += rd * max(d, 0.01);
        if(d < 0.001 || length(p - ro) > MaxDist) break;
    }

    vec4 gcol = getGlow(glow);
    vec3 baseColor = vec3(0.1, 0.3, 0.7); // базовый цвет объекта
    return vec4(mix(baseColor, gcol.rgb, gcol.a), 1.0);
}

//----------------------------------------
void main() {
    vec2 uv = _UV;
    uv.x *= Aspect;
    float scale = tan(radians(FOV) * 0.5);
    uv *= scale;

    vec3 rd = normalize(uv.x * Cam_right + uv.y * Cam_up + Cam_front);
    vec3 ro = Cam_pos;

    FragColor = raymarch(ro, rd);
}