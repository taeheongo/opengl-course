#version 330 core
uniform vec4 color; // uniform: 병렬로 수행되는 모든 shader thread들이 동일한 값을 전달받는다
out vec4 fragColor;

void main() {
    fragColor = color;
}
