#version 330 core
in vec4 vertexColor;
in vec2 texCoord;
out vec4 fragColor;

uniform sampler2D tex; // 바인딩 되었던 2d texture를 가리킴

void main() {
    fragColor = texture(tex, texCoord);
}