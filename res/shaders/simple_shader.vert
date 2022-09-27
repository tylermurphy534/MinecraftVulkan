#version 450

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;
layout (location = 3) in int tex;

layout (location = 0) out float fragLight;
layout (location = 1) out vec2 fragUv;
layout (location = 2) out int fragTex;

layout (binding = 0) uniform GlobalUbo {
  mat4 projectionViewMatrix;
  vec3 directionToLight;
} ubo;

layout (push_constant) uniform Push {
  mat4 modelMatrix;
  mat4 normalMatrix;
} push;

const float AMBIENT = 0.02;

void main() {
  gl_Position = ubo.projectionViewMatrix * push.modelMatrix * vec4(position, 1.0);

  vec3 normalWorldSpace = normalize(mat3(push.normalMatrix) * normal);

  float lightIntensity = AMBIENT + max(dot(normalWorldSpace, ubo.directionToLight), 0);

  fragLight = lightIntensity / 5;
  fragUv = uv;
  fragTex = tex;
}