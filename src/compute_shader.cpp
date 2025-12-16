#include "compute_shader.h"

#include <fstream>
#include <iostream>
#include <vector>

#include "glbinding/gl/enum.h"
#include "glbinding/gl/functions.h"

using namespace gl;

compute_shader::compute_shader(const std::string &path) : shader_program(glCreateProgram()) {
    std::ifstream file_stream(path);

    if (!file_stream.is_open()) {
        std::cerr << "Failed to open " << path << " (compute shader)" << std::endl;
    }

    std::string shader_code{
        std::istreambuf_iterator(file_stream),
        std::istreambuf_iterator<char>()
    };
    const char *shader_code_c_str = shader_code.c_str();
    file_stream.close();

    int success;
    std::vector<char> info_vector(512);

    unsigned int shader_id = glCreateShader(GL_COMPUTE_SHADER);
    glShaderSource(shader_id, 1, &shader_code_c_str, nullptr);
    glCompileShader(shader_id);
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader_id, info_vector.size(), nullptr, info_vector.data());
        std::cerr << "Compute Shader Compilation Failed" << std::endl << info_vector.data() << std::endl;
    }

    glAttachShader(ID, shader_id);
    glLinkProgram(ID);
    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(ID, info_vector.size(), nullptr, info_vector.data());
        std::cerr << "Shader Program Linking Failed" << std::endl << info_vector.data() << std::endl;
    }

    glDeleteShader(shader_id);
}
