#version 460

layout (location = 0) in vec3 VertexCoord;
out vec2 FragCoord;

void main() {
    gl_Position = vec4(VertexCoord, 1);
    FragCoord = (VertexCoord.xy + 1) / 2;
}