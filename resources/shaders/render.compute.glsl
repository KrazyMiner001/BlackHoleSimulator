#version 460
#define MAX_DEPTH 10000
#define DELTA 0.001
#define SCHWARZSCHILD_RADIUS 0.5
#define PI 3.141592

layout (local_size_x = 8, local_size_y = 8) in;
layout (rgba32f, binding = 0) uniform image2D imgOutput;
layout (rgba32f, binding = 1) uniform image2D imgInput;

layout (location = 0) uniform mat3 transform;
layout (location = 1) uniform vec3 camera_position;
layout (location = 2) uniform float eq1;
layout (location = 3) uniform float eq2;

void main() {
    vec2 group_coords = vec2(gl_GlobalInvocationID.xy);
    vec2 num_invocations = vec2((gl_NumWorkGroups * gl_WorkGroupSize).xy);

    vec3 world_space = vec3((group_coords - 0.5 * num_invocations) / num_invocations.x, 0);
    world_space = world_space * transform;
    world_space += camera_position;

    vec3 direction = vec3(0, 0, 1) * transform;

    float r = length(world_space);
    float u = 1 / r;
    //float du_dphi = sqrt(SCHWARZSCHILD_RADIUS * u * u * u - u * u + 1 / pow(2.5980762113533 * SCHWARZSCHILD_RADIUS + eq1, 2));
    float du_dphi = -u * dot(direction, normalize(world_space)) / dot(direction, normalize(cross(cross(normalize(world_space), direction), normalize(world_space))));
    float phi = atan(world_space.x, world_space.z);
    float theta = PI / 2 - atan(world_space.y, length(world_space.xz));

    int depth = 0;
    while (depth < MAX_DEPTH) {
        u = 1 / r;
        phi += DELTA;
        u += DELTA * du_dphi;
        du_dphi += DELTA * (1.5 * u * u * SCHWARZSCHILD_RADIUS - u);
        r = 1 / u;
        r = abs(r);
        if (phi > 2 * PI) phi -= 2 * PI;
        if (phi < 0) phi += 2 * PI;

        if (r < SCHWARZSCHILD_RADIUS) {
            imageStore(imgOutput, ivec2(gl_GlobalInvocationID.xy), vec4(0, 0, 0, 1));
            return;
        }

        if (r > 20) {
            imageStore(
                imgOutput,
                ivec2(gl_GlobalInvocationID.xy),
                imageLoad(imgInput, ivec2(vec2(phi / (2 * PI), theta / PI) * imageSize(imgInput)))
            );
            return;
        }

        depth++;
    }

    imageStore(imgOutput, ivec2(gl_GlobalInvocationID.xy), vec4(0, 1, 0, 1));
}