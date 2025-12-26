#version 460
#define MAX_DEPTH 100000
#define DELTA -0.001
#define RADIUS 1
#define PI 3.141592

layout (local_size_x = 8, local_size_y = 8) in;
layout (rgba32f, binding = 0) uniform image2D imgOutput;
layout (rgba32f, binding = 1) uniform image2D imgInput;

layout (location = 0) uniform mat3 transform;
layout (location = 1) uniform vec3 camera_position;
layout (location = 2) uniform float eq1;
layout (location = 3) uniform float eq2;

struct Position {
    float r;
    float phi;
    float t;
};

void main() {
    vec2 group_coords = vec2(gl_GlobalInvocationID.xy);
    vec2 num_invocations = vec2((gl_NumWorkGroups * gl_WorkGroupSize).xy);

    vec3 world_space = vec3((group_coords - 0.5 * num_invocations) / num_invocations.x, 0);
    world_space = world_space * transform;
    world_space += camera_position;

    Position position;
    position.t = 0;

    position.r = length(world_space);
    vec3 direction = normalize(vec3(0, 0, 1) * transform);
    float delta_u = DELTA * (-1 / position.r) * dot(direction, normalize(world_space)) / dot(direction, normalize(cross(cross(normalize(world_space), direction), normalize(world_space))));

    float theta = asin(world_space.y / position.r);
    vec3 rotation_axis = normalize(cross(vec3(world_space.x, 0, world_space.z), vec3(0, world_space.y, 0)));
    world_space = cos(theta)*(cross(cross(rotation_axis, world_space), rotation_axis)) + sin(theta) * cross(world_space, rotation_axis);

    position.phi = atan(world_space.z / world_space.x);

    int depth = 0;

    if (position.r < RADIUS) {
        imageStore(imgOutput, ivec2(gl_GlobalInvocationID.xy), vec4(1, 0, 0, 1));
        return;
    }

    while (depth < MAX_DEPTH) {
        position.phi += DELTA;
        float u = 1 / position.r;
        delta_u += DELTA * DELTA * (1.5 * RADIUS * u * u - u);

        position.r += 1 / (u + delta_u);

        if (position.r <= RADIUS + DELTA) {
            imageStore(imgOutput, ivec2(gl_GlobalInvocationID.xy), vec4(0, 0, 0, 1));
            return;
        }

        if (position.r >= 20) {
            vec4 color = imageLoad(imgInput, ivec2(512 * (theta / (4 * PI) + 0.5), 512 * (position.phi / (4 * PI) + 0.5)));
            imageStore(imgOutput, ivec2(gl_GlobalInvocationID.xy), color);
            return;
        }

        depth++;
    }

    imageStore(imgOutput, ivec2(gl_GlobalInvocationID.xy), vec4(0, 1, 0, 1));
}