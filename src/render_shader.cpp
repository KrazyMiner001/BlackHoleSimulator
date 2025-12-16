#include "render_shader.h"

#include <fstream>
#include <iostream>

#include "glbinding/gl/enum.h"
#include "glbinding/gl/functions.h"

using namespace gl;

render_shader::render_shader(const std::string &vertex_shader_path, const std::string &fragment_shader_path) : shader_program(glCreateProgram()) {
    std::ifstream vertex_shader_stream(vertex_shader_path);
    std::ifstream fragment_shader_stream(fragment_shader_path);

    if (!vertex_shader_stream.is_open()) {
        std::cerr << "Failed to open " << vertex_shader_path << " (vertex shader)" << std::endl;
    }
    if (!fragment_shader_stream.is_open()) {
        std::cerr << "Failed to open " << fragment_shader_path <<" (fragment shader)" <<  std::endl;
    }

    std::string vertex_shader_code{
        std::istreambuf_iterator(vertex_shader_stream),
        std::istreambuf_iterator<char>()
    };
    vertex_shader_stream.close();
    const char *vertex_shader_c_str = vertex_shader_code.c_str();

    std::string fragment_shader_code{
        std::istreambuf_iterator(fragment_shader_stream),
        std::istreambuf_iterator<char>()
    };
    fragment_shader_stream.close();
    const char *fragment_shader_c_str = fragment_shader_code.c_str();

    unsigned int vertex_id, fragment_id;
    int success;
    std::vector<char> info_vector(512);

    vertex_id = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_id, 1, &vertex_shader_c_str, nullptr);
    glCompileShader(vertex_id);
    glGetShaderiv(vertex_id, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex_id, info_vector.size(), nullptr, info_vector.data());
        std::cerr << "Vertex Shader Compilation Failed" << std::endl << info_vector.data() << std::endl;
    }

    fragment_id = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_id, 1, &fragment_shader_c_str, nullptr);
    glCompileShader(fragment_id);
    glGetShaderiv(fragment_id, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment_id, info_vector.size(), nullptr, info_vector.data());
        std::cerr << "Fragment Shader Compilation Failed" << std::endl << info_vector.data() << std::endl;
    }

    glAttachShader(ID, vertex_id);
    glAttachShader(ID, fragment_id);
    glLinkProgram(ID);
    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(ID, info_vector.size(), nullptr, info_vector.data());
        std::cerr << "Shader Program Linking Failed" << std::endl << info_vector.data() << std::endl;
    }

    glDeleteShader(vertex_id);
    glDeleteShader(fragment_id);
}
