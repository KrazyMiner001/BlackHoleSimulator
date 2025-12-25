#ifndef BLACKHOLESIMULATOR_BLACKHOLE_RENDERER_H
#define BLACKHOLESIMULATOR_BLACKHOLE_RENDERER_H
#include "compute_shader.h"
#include "render_shader.h"
#include "gamepad.h"
#include "glm/glm.hpp"


class blackhole_renderer {
public:
    float display_scale;
    float window_width;
    float window_height;
    float eq1;
    float eq2;

    blackhole_renderer(float display_scale, float window_width, float window_height);

    void gamepad_input(SDL_GamepadAxisEvent const &event);

    void render();

public:
    float render_width;
    float render_height;
    render_shader quad_shader;
    compute_shader black_hole_shader;
    unsigned int VAO = 0, VBO = 0, EBO = 0;
    unsigned int texture0 = 0, texture1 = 0;
    glm::mat3 transform_matrix;
    glm::vec3 position;
    float yaw = 0, pitch = 0;
    gamepad gamepad{};
};


#endif //BLACKHOLESIMULATOR_BLACKHOLE_RENDERER_H
