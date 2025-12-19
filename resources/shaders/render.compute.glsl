#version 460
#define MAX_DEPTH 10000
#define DELTA 0.01

layout (local_size_x = 8, local_size_y = 8) in;
layout (rgba32f, binding = 0) uniform image2D imgOutput;
layout (rgba32f, binding = 1) uniform image2D imgInput;

layout (location = 0) uniform mat3 transform;
layout (location = 1) uniform vec3 camera_position;

struct Ray {
    vec3 position;
    vec3 direction;
};

void main() {
    vec2 group_coords = vec2(gl_GlobalInvocationID.xy);
    vec2 num_invocations = vec2((gl_NumWorkGroups * gl_WorkGroupSize).xy);

    Ray ray;
    ray.position = vec3((group_coords - 0.5 * num_invocations) / num_invocations.x, 0);
    ray.position *= transform;
    ray.position += camera_position;
    ray.direction = transform * vec3(0, 0, 1);// + ray.position / 5;

    int depth = 0;
//    imageStore(imgOutput, ivec2(gl_GlobalInvocationID.xy), vec4(vec3(length(ray.position) / 10), 1));
//    return;

    while (depth < MAX_DEPTH) {
        ray.direction = normalize(ray.direction);
        ray.position += ray.direction * DELTA;

        if (length(ray.position) < 0.5) {
            imageStore(imgOutput, ivec2(gl_GlobalInvocationID.xy), vec4(ray.direction, 1));
            return;
        }

        if (length(ray.position) > 10) {
            imageStore(imgOutput, ivec2(gl_GlobalInvocationID.xy), vec4(0.3, 0.2, 0.7, 1));
            return;
        }

        depth++;
    }
}