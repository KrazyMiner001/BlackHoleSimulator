#version 460

layout (local_size_x = 8, local_size_y = 8) in;
layout (rgba32f, binding = 0) uniform image2D imgOutput;
layout (rgba32f, binding = 1) uniform image2D imgInput;

void main() {
    ivec2 position = ivec2(gl_GlobalInvocationID.xy);
    vec4 color;
    if (position.x / 8 % 2 == 0) {
        color = vec4(1, 1, 1, 1);
    } else {
        color = vec4(0, 0, 0, 1);
    }

    imageStore(imgOutput, position, color);
}