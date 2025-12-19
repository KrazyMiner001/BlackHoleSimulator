#include "blackhole_renderer.h"

#include <cmath>

#include "glbinding/gl/enum.h"
#include "glbinding/gl/functions.h"

#include "stb_image.h"
#include "glm/gtc/matrix_transform.hpp"

using namespace gl;
using namespace glm;

blackhole_renderer::blackhole_renderer(const float display_scale, const float window_width, const float window_height)
    : display_scale(display_scale)
      , window_width(window_width)
      , window_height(window_height)
      , render_width(window_width * display_scale)
      , render_height(window_height * display_scale)
      , quad_shader("resources/shaders/quad.vertex.glsl", "resources/shaders/quad.fragment.glsl")
      , black_hole_shader("resources/shaders/render.compute.glsl")
      , transform_matrix(mat3(
          5, 0, 0,
          0, 5, 0,
          0, 0, 0
      ))
      , position(vec3(0, 0, -5)) {
    constexpr float vertices[] = {
        -1, -1, 0,
        -1, 1, 0,
        1, -1, 0,
        1, 1, 0,
    };

    constexpr unsigned int indices[] = {
        0, 1, 2,
        1, 2, 3,
    };

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glEnableVertexArrayAttrib(VAO, 0);
    glVertexAttribPointer(0, 3, GL_FLOAT, false, 3 * sizeof(float), nullptr);

    glGenTextures(1, &texture0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, render_width, render_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    int image_width, image_height, channels;
    unsigned char *image_data = stbi_load("resources/checkerboard.png", &image_width, &image_height, &channels, 4);
    glGenTextures(1, &texture1);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
    stbi_image_free(image_data);
}

void blackhole_renderer::render() {
    yaw -= gamepad.get_right_stick().x;
    pitch -= gamepad.get_right_stick().y;
    pitch = clamp(pitch, -90.0f, 90.0f);

    transform_matrix = mat3(5);
    transform_matrix *= mat3(
        cos(radians(yaw)), 0, sin(radians(yaw)),
        0, 1, 0,
        -sin(radians(yaw)), 0, cos(radians(yaw))
    );
    transform_matrix *= mat3(
        1, 0, 0,
        0, cos(radians(pitch)), -sin(radians(pitch)),
        0, sin(radians(pitch)), cos(radians(pitch))
    );

    position -= gamepad.get_left_stick().y * 0.01f * transform_matrix * vec3(0, 0, 1);
    position += gamepad.get_left_stick().x * 0.01f * transform_matrix * vec3(1, 0, 0);

    render_width = std::ceil(window_width * display_scale / 8) * 8;
    render_height = std::ceil(window_height * display_scale / 8) * 8;

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, static_cast<int>(render_width), static_cast<int>(render_height), 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    glBindImageTexture(0, texture0, 0, false, 0, GL_WRITE_ONLY, GL_RGBA32F);
    glBindImageTexture(1, texture1, 0, false, 0, GL_READ_ONLY, GL_RGBA32F);

    black_hole_shader.use();
    shader_program::setMat3(0, transform_matrix);
    shader_program::setVec3(1, position);
    glDispatchCompute(static_cast<int>(render_width / 8), static_cast<int>(render_height / 8), 1);

    quad_shader.use();
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

void blackhole_renderer::gamepad_input(SDL_GamepadAxisEvent const &event) {
    gamepad.process_event(event);
}
