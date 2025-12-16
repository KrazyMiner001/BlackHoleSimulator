#include "shader_program.h"

#include "glbinding/gl/functions.h"

void shader_program::use() const {
    gl::glUseProgram(ID);
}
