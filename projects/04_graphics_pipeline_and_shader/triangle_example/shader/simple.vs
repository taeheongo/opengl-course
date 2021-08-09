#version 330 core
layout (location = 0) in vec3 aPos; // location = 0이 곧 VAO attribute 0번을 의미

void main() {
    gl_Position = vec4(aPos, 1.0);
}