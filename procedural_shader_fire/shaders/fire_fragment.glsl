#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform float u_time;
uniform float u_intensity;
uniform float u_speed;
uniform int u_octaves;
uniform float u_scale;
uniform int u_noise_type; // 0 = simplex, 1 = perlin

// Hash function for noise generation
vec3 hash3(vec2 p) {
    vec3 q = vec3(dot(p, vec2(127.1, 311.7)), 
                  dot(p, vec2(269.5, 183.3)), 
                  dot(p, vec2(419.2, 371.9)));
    return fract(sin(q) * 43758.5453);
}

// Classic Perlin noise by Ken Perlin
vec4 permute(vec4 x) {
    return mod(((x*34.0)+1.0)*x, 289.0);
}

vec2 fade(vec2 t) {
    return t*t*t*(t*(t*6.0-15.0)+10.0);
}

float cnoise(vec2 P) {
    vec4 Pi = floor(P.xyxy) + vec4(0.0, 0.0, 1.0, 1.0);
    vec4 Pf = fract(P.xyxy) - vec4(0.0, 0.0, 1.0, 1.0);
    Pi = mod(Pi, 289.0);
    vec4 ix = Pi.xzxz;
    vec4 iy = Pi.yyww;
    vec4 fx = Pf.xzxz;
    vec4 fy = Pf.yyww;

    vec4 i = permute(permute(ix) + iy);
    vec4 gx = 2.0 * fract(i * 0.0243902439) - 1.0;
    vec4 gy = abs(gx) - 0.5;
    vec4 tx = floor(gx + 0.5);
    gx = gx - tx;

    vec2 g00 = vec2(gx.x, gy.x);
    vec2 g10 = vec2(gx.y, gy.y);
    vec2 g01 = vec2(gx.z, gy.z);
    vec2 g11 = vec2(gx.w, gy.w);

    vec4 norm = 1.79284291400159 - 0.85373472095314 * 
        vec4(dot(g00, g00), dot(g01, g01), dot(g10, g10), dot(g11, g11));
    g00 *= norm.x;
    g01 *= norm.y;
    g10 *= norm.z;
    g11 *= norm.w;

    float n00 = dot(g00, vec2(fx.x, fy.x));
    float n10 = dot(g10, vec2(fx.y, fy.y));
    float n01 = dot(g01, vec2(fx.z, fy.z));
    float n11 = dot(g11, vec2(fx.w, fy.w));

    vec2 fade_xy = fade(Pf.xy);
    vec2 n_x = mix(vec2(n00, n01), vec2(n10, n11), fade_xy.x);
    float n_xy = mix(n_x.x, n_x.y, fade_xy.y);
    return 2.3 * n_xy;
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

// Selectable noise function
float noise2D(vec2 p) {
    if (u_noise_type == 0) { // Simplex
        return simplex2D(p);
    } else { // Perlin
        return cnoise(p);
    }
}

// Fractal Brownian Motion
float fbm(vec2 p, int octaves) {
    float value = 0.0;
    float amplitude = 0.5;
    float frequency = 1.0;
    
    for (int i = 0; i < octaves; i++) {
        value += amplitude * noise2D(p * frequency);
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
        value += amplitude * abs(noise2D(p * frequency));
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
    vec2 uv = TexCoord;

    // Flip vertical uv so flames rise upward
    uv.y = 1.0 - uv.y;

    float time = u_time * u_speed;
    vec2 p = uv * u_scale;

    float noise1 = fbm(p + vec2(0.0, time * 0.5), u_octaves);
    float noise2 = fbm(p * 2.0 + vec2(time * 0.3, time * 0.8), u_octaves - 1);
    float noise3 = turbulence(p * 4.0 + vec2(time * 0.1, time * 1.2), u_octaves - 2);
    
    float firenoise = noise1 * 0.5 + noise2 * 0.3 + noise3 * 0.2;

    float flamemotion = fbm(vec2(uv.x * 3.0, uv.y * 2.0 + time * 1.5), 3);
    firenoise += flamemotion * 0.3;

    float flamemask = uv.y * uv.y; // Stronger at bottom
    flamemask *= smoothstep(0.0, 0.3, 1.0 - abs(uv.x - 0.5) * 2.0);

    float fireintensity = (firenoise + 0.5) * flamemask * u_intensity;

    float flicker = sin(time * 15.0) * 0.1 + sin(time * 23.0) * 0.05;
    fireintensity += flicker * flamemask;

    vec3 firecol = fireColor(fireintensity);

    float glow = smoothstep(0.0, 0.8, fireintensity) * 0.3;
    firecol += vec3(glow * 0.5, glow * 0.2, 0.0);

    float alpha = smoothstep(0.0, 0.5, fireintensity);

    FragColor = vec4(firecol, alpha);
}