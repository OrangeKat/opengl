#version 330 core

layout(location = 0) in vec3 inPos;
layout(location = 1) in float inSize;
layout(location = 2) in vec4 inColor;

uniform mat4 projection;
uniform mat4 view;

out vec4 fragColor;
out float particleSize;

void main() {
    gl_Position = projection * view * vec4(inPos, 1.0);

    // Dynamic point size based on distance and particle properties
    float distance = length((view * vec4(inPos, 1.0)).xyz);
    gl_PointSize = inSize * 150.0 / (1.0 + distance * 0.1);

    fragColor = inColor;
    particleSize = inSize;
}
