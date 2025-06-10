#include "shader.hh"

// Vertex Shader Source
const char *vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

void main()
{
    gl_Position = vec4(aPos, 1.0);
    TexCoord = aTexCoord;
}
)";

// Fragment Shader Source - Procedural Fire
const char *fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform float u_time;
uniform float u_intensity;
uniform float u_speed;
uniform int u_octaves;
uniform float u_scale;

// Hash function for noise generation
vec3 hash3(vec2 p) {
    vec3 q = vec3(dot(p, vec2(127.1, 311.7)), 
                  dot(p, vec2(269.5, 183.3)), 
                  dot(p, vec2(419.2, 371.9)));
    return fract(sin(q) * 43758.5453);
}

// 2D Simplex noise
float simplex2D(vec2 p) {
    const float K1 = 0.366025404; // (sqrt(3)-1)/2
    const float K2 = 0.211324865; // (3-sqrt(3))/6
    
    vec2 i = floor(p + (p.x + p.y) * K1);
    vec2 a = p - i + (i.x + i.y) * K2;
    vec2 o = (a.x > a.y) ? vec2(1.0, 0.0) : vec2(0.0, 1.0);
    vec2 b = a - o + K2;
    vec2 c = a - 1.0 + 2.0 * K2;
    
    vec3 h = max(0.5 - vec3(dot(a, a), dot(b, b), dot(c, c)), 0.0);
    vec3 n = h * h * h * h * vec3(dot(a, hash3(i).xy), 
                                   dot(b, hash3(i + o).xy), 
                                   dot(c, hash3(i + 1.0).xy));
    
    return dot(n, vec3(70.0));
}

// Fractal Brownian Motion
float fbm(vec2 p, int octaves) {
    float value = 0.0;
    float amplitude = 0.5;
    float frequency = 1.0;
    
    for (int i = 0; i < octaves; i++) {
        value += amplitude * simplex2D(p * frequency);
        frequency *= 2.0;
        amplitude *= 0.5;
    }
    
    return value;
}

// Turbulence function
float turbulence(vec2 p, int octaves) {
    float value = 0.0;
    float amplitude = 0.5;
    float frequency = 1.0;
    
    for (int i = 0; i < octaves; i++) {
        value += amplitude * abs(simplex2D(p * frequency));
        frequency *= 2.0;
        amplitude *= 0.5;
    }
    
    return value;
}

// Fire color ramp
vec3 fireColor(float t) {
    // Clamp input
    t = clamp(t, 0.0, 1.0);
    
    // Define fire color stops
    vec3 color1 = vec3(0.0, 0.0, 0.0);        // Black (no fire)
    vec3 color2 = vec3(0.8, 0.1, 0.0);        // Dark red
    vec3 color3 = vec3(1.0, 0.4, 0.0);        // Orange-red
    vec3 color4 = vec3(1.0, 0.8, 0.0);        // Orange
    vec3 color5 = vec3(1.0, 1.0, 0.6);        // Yellow-white
    vec3 color6 = vec3(1.0, 1.0, 1.0);        // White hot
    
    if (t < 0.2) {
        return mix(color1, color2, t * 5.0);
    } else if (t < 0.4) {
        return mix(color2, color3, (t - 0.2) * 5.0);
    } else if (t < 0.6) {
        return mix(color3, color4, (t - 0.4) * 5.0);
    } else if (t < 0.8) {
        return mix(color4, color5, (t - 0.6) * 5.0);
    } else {
        return mix(color5, color6, (t - 0.8) * 5.0);
    }
}

void main()
{
    vec2 uv = texcoord;

    // flip vertical uv so flames rise upward
    uv.y = 1.0 - uv.y;

    // no horizontal distortion — flames go straight up
    // (commented out the previous flame shaping)
    // uv.x = (uv.x - 0.5) * (2.0 - uv.y * 1.5) + 0.5;

    float time = u_time * u_speed;
    vec2 p = uv * u_scale;

    float noise1 = fbm(p + vec2(0.0, time * 0.5), u_octaves);
    float noise2 = fbm(p * 2.0 + vec2(time * 0.3, time * 0.8), u_octaves - 1);
    float noise3 = turbulence(p * 4.0 + vec2(time * 0.1, time * 1.2), u_octaves - 2);
    
    float firenoise = noise1 * 0.5 + noise2 * 0.3 + noise3 * 0.2;

    float flamemotion = fbm(vec2(uv.x * 3.0, uv.y * 2.0 + time * 1.5), 3);
    firenoise += flamemotion * 0.3;

    float flamemask = uv.y * uv.y; // stronger at bottom, tapering off at top
    flamemask *= smoothstep(0.0, 0.3, 1.0 - abs(uv.x - 0.5) * 2.0);

    float fireintensity = (firenoise + 0.5) * flamemask * u_intensity;

    float flicker = sin(time * 15.0) * 0.1 + sin(time * 23.0) * 0.05;
    fireintensity += flicker * flamemask;

    vec3 firecol = firecolor(fireintensity);

    float glow = smoothstep(0.0, 0.8, fireintensity) * 0.3;
    firecol += vec3(glow * 0.5, glow * 0.2, 0.0);

    float alpha = smoothstep(0.0, 0.5, fireintensity);

    fragcolor = vec4(firecol, alpha);
}
)";
