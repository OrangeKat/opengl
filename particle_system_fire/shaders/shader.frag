#version 330 core

in vec4 fragColor;
in float particleSize;
out vec4 color;

void main() {
    vec2 coord = gl_PointCoord - vec2(0.5);
    float dist = length(coord);

    if (dist > 0.5) discard;

    // Create softer, more natural particle edges
    float alpha = 1.0 - smoothstep(0.2, 0.5, dist);

    // Add some internal variation for more organic look
    float variation = sin(coord.x * 20.0) * sin(coord.y * 20.0) * 0.1 + 0.9;

    // Brighten center for glow effect
    float centerGlow = 1.0 - dist * 2.0;
    centerGlow = max(0.0, centerGlow);

    vec4 finalColor = fragColor;
    finalColor.rgb *= variation;
    finalColor.rgb += centerGlow * 0.3; // Add glow
    finalColor.a *= alpha;

    color = finalColor;
}
