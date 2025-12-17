#include <glbinding/glbinding.h>
#include <glbinding/gl/functions.h>
#include <glbinding/gl/enum.h>

#include <SDL3/SDL.h>

#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl3.h>
#include <iostream>

#include <glm/glm.hpp>

#include "render_shader.h"
#include "compute_shader.h"
#include "glbinding/gl/bitfield.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../include/stb_image.h"

#define WIDTH 600
#define HEIGHT 480

using namespace gl;
using namespace glm;

void rerender_canvas(const compute_shader render_shader, const float render_scale, const float display_x, const float display_y, float &render_width, float &render_height, unsigned int texture0) {
    render_width = ceil(display_x * render_scale / 8) * 8;
    render_height = ceil(display_y * render_scale / 8) * 8;
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, static_cast<int>(render_width), static_cast<int>(render_height), 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    glBindImageTexture(0, texture0, 0, false, 0, GL_READ_WRITE, GL_RGBA32F);

    render_shader.use();
    glDispatchCompute(ceil(render_width / 8), ceil(render_height / 8), 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}

int main() {
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD)) {
        std::cout << "SDL failed to initialise: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    SDL_Window *window = SDL_CreateWindow(
        "Black Hole Simulator",
        WIDTH,
        HEIGHT,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
    );

    if (window == nullptr) {
        std::cout << "SDL failed to create window: " << SDL_GetError() << std::endl;
        return 1;
    }

    const SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    if (gl_context == nullptr) {
        std::cout << "SDL failed to create gl context: " << SDL_GetError() << std::endl;
        return 1;
    }

    glbinding::initialize(SDL_GL_GetProcAddress, false);

    SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SetSwapInterval(1);
    SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    SDL_ShowWindow(window);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    const ImGuiIO &io = ImGui::GetIO();
    ImGui::StyleColorsDark();

    ImGui_ImplSDL3_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init("#version 460");

    const render_shader quad_shader("resources/shaders/quad.vertex.glsl", "resources/shaders/quad.fragment.glsl");
    const compute_shader render_shader("resources/shaders/render.compute.glsl");

    float render_scale = 1;
    float render_width = WIDTH;
    float render_height = HEIGHT;

    unsigned int texture0, texture1;
    glGenTextures(1, &texture0);
    glGenTextures(1, &texture1);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, static_cast<int>(render_width), static_cast<int>(render_height), 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glBindImageTexture(0, texture0, 0, false, 0, GL_READ_WRITE, GL_RGBA32F);

    int image_width, image_height, channels_in_image;
    unsigned char* image_data = stbi_load("resources/checkerboard.png", &image_width, &image_height, &channels_in_image, 4);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
    stbi_image_free(image_data);
    glBindImageTexture(1, texture1, 0, false, 0, GL_READ_WRITE, GL_RGBA32F);

    render_shader.use();
    glDispatchCompute(ceil(render_width / 8), ceil(render_height / 8), 1);

    constexpr float vertices[] = {
        -1, -1, 0,
        -1, 1, 0,
        1, -1, 0,
        1, 1, 0,
    };

    const unsigned int indices[] = {
        0, 1, 2,
        1, 2, 3,
    };

    unsigned int EBO, VBO, VAO;
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

    SDL_Event event;
    bool done = false;
    while (!done) {
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL3_ProcessEvent(&event);

            switch (event.type) {
                case SDL_EVENT_QUIT:
                    done = true;
                    break;
                case SDL_EVENT_WINDOW_RESIZED: {
                    int width, height;
                    SDL_GetWindowSize(window, &width, &height);
                    rerender_canvas(render_shader, render_scale, static_cast<float>(width), static_cast<float>(height), render_width, render_height, texture0);
                }
            }
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Controls");
        if (ImGui::SliderFloat("Resolution Scale", &render_scale,0.125, 2)) {
            rerender_canvas(render_shader, render_scale, io.DisplaySize.x, io.DisplaySize.y, render_width, render_height, texture0);
        }
        ImGui::End();

        ImGui::Render();

        glViewport(0, 0, io.DisplaySize.x, io.DisplaySize.y);
        glClearColor(0.2, 0.3, 0.3, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture0);

        quad_shader.use();
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DestroyContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
