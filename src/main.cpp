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
#include "blackhole_renderer.h"
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

    blackhole_renderer renderer(1, WIDTH, HEIGHT);

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
                    renderer.window_height = static_cast<float>(height);
                    renderer.window_width = static_cast<float>(width);
                }
                default: ;
            }
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Controls");
        ImGui::SliderFloat("Resolution Scale", &renderer.display_scale,0.125, 2);
        ImGui::End();

        ImGui::Render();

        glViewport(0, 0, static_cast<int>(io.DisplaySize.x), static_cast<int>(io.DisplaySize.y));
        glClearColor(0.2, 0.3, 0.3, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);

        renderer.render();

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
