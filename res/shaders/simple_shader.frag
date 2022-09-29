#version 450

layout (location = 0) in float fragLight;
layout (location = 1) in vec2 fragUv;
layout (location = 2) in flat int fragTex;

layout (location = 0) out vec4 outColor;

layout (binding = 1) uniform sampler2D texSampler[9];

void main() {
  outColor = texture(texSampler[fragTex], fragUv) + fragLight;
}