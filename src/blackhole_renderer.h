#ifndef BLACKHOLESIMULATOR_BLACKHOLE_RENDERER_H
#define BLACKHOLESIMULATOR_BLACKHOLE_RENDERER_H
#include "compute_shader.h"
#include "render_shader.h"


class blackhole_renderer {
public:
    float display_scale;
    float window_width;
    float window_height;

    blackhole_renderer(float display_scale, float window_width, float window_height);
    void render();

private:
    float render_width;
    float render_height;
    render_shader quad_shader;
    compute_shader black_hole_shader;
    unsigned int VAO{}, VBO{}, EBO{};
    unsigned int texture0{}, texture1{};
};


#endif //BLACKHOLESIMULATOR_BLACKHOLE_RENDERER_H