#version 450

layout (location = 0) in vec3 fragColor;
layout (location = 1) in vec2 fragUv;
layout (location = 2) in float fragTex;

layout (location = 0) out vec4 outColor;

layout (binding = 0) uniform GlobalUbo {
  mat4 projectionViewMatrix;
  vec3 directionToLight;
} ubo;

layout (binding = 1) uniform sampler2D texSampler[2];

layout(push_constant) uniform Push {
  mat4 transform;
  mat4 normalMatrix;
} push;

void main() {
  outColor = mix(texture(texSampler[int(fragTex)], fragUv), vec4(fragColor, 1.0), .1);
}