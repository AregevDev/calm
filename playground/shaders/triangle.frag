#version 460 core

layout (location = 0) in vec3 inPosition;
layout (location = 1) in flat ivec2 inWindowSize;

layout (location = 0) uniform float time;

layout (location = 0) out vec4 fragColor;

const vec3 colA = vec3(0.1843137254901961, 0.0, 0.3764705882352941);
const vec3 colB = vec3(1.0, 0.47058823529411764, 0.0);

const float PI = 3.14159265358979323846;
const float TWO_PI = 6.28318530717958647692;

void main() {
    vec2 uv = (2.0 * gl_FragCoord.xy - inWindowSize.xy) / inWindowSize.y;
    vec3 col = 0.5 + 0.5 * cos(time+uv.xyx + vec3(0.0, 2.0, 4.0));

    float angle = atan(uv.y, uv.x);
    float dist = length(uv);

    float spiral_val = log(dist) * 3.0 + angle * (5.0 / TWO_PI) - time * 0.2;
    float spiral = fract(spiral_val);

    vec3 bkg = mix(colA, colB, spiral);
    
    // vec3 bkg = vec3(0.0);
    fragColor = vec4(bkg, 1.0);
}
