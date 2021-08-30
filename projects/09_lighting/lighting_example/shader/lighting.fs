#version 330 core
in vec3 normal;
in vec2 texCoord;
out vec4 fragColor;

uniform vec3 lightColor;    // 빛의 색상
uniform vec3 objectColor;   // 물체의 색상
uniform float ambientStrength; // 밝기. ambient light는 주변의 깔려있는 은은헌 빛이기 때문에 ambientStrength가 보통 0.1 정도이다.

void main() {
  vec3 ambient = ambientStrength * lightColor;
  vec3 result = ambient * objectColor;
  fragColor = vec4(result, 1.0);
}