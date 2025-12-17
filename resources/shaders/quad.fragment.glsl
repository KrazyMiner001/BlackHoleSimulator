#version 460

layout (binding = 0) uniform sampler2D tex;
in vec2 FragCoord;

out vec4 FragColor;

void main() {
    FragColor = texture(tex, FragCoord);
}