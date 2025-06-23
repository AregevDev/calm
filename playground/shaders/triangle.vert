#version 460 core

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inTexCoords;

layout (location = 0) out vec3 outPosition;
layout (location = 1) out flat ivec2 outWindowSize;

layout (std140) uniform Mvp
{
    mat4 model;
    mat4 view;
    mat4 projection;
    mat4 mvp;
    ivec2 windowSize;
};

void main() {
    gl_Position = mvp * vec4(inPosition, 1.0);
    outPosition = vec3(inPosition.xy, 1.0);
    outWindowSize = windowSize;
}
