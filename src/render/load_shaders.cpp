#include "renderer/load_shaders.h"

#include <string>
#include <fstream>
#include <sstream>
#include <vector>

#include <GLFW/glfw3.h>

bool load_file_into_string(const char *file_path, std::string& string)
{
    std::ifstream file_stream(file_path, std::ios::in);
    if (file_stream.is_open()) {
        std::stringstream sstream;
        sstream << file_stream.rdbuf();
        string = sstream.str();
        file_stream.close();
        return true;
    } else {
        printf("Failed to open file %s.\n", file_path);
        return false;
    }
}

void compile_and_check_shader(
    const char *file_path,
    const std::string& code,
    const GLuint& shader_id)
{
    GLint result = GL_FALSE;
    int info_log_length;

    // Compile shader
    printf("Compiling shader : %s\n", file_path);
    char const *vertex_source_pointer = code.c_str();
    glShaderSource(shader_id, 1, &vertex_source_pointer, NULL);
    glCompileShader(shader_id);

    // Check shader
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &result);
    glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &info_log_length);
    if (info_log_length > 0) {
        std::vector<char> vertex_shader_error_message(info_log_length + 1);
        glGetShaderInfoLog(
            shader_id, info_log_length, NULL, &vertex_shader_error_message[0]
        );
        printf("%s\n", &vertex_shader_error_message[0]);
    }
}

GLuint load_shaders(
    const char *vertex_file_path,
    const char *fragment_file_path)
{
    // Create shaders
    GLuint vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);

    std::string vertex_shader_code;
    if (!load_file_into_string(vertex_file_path, vertex_shader_code)) {
        return 0;
    }

    std::string fragment_shader_code;
    if (!load_file_into_string(fragment_file_path, fragment_shader_code)) {
        return 0;
    }

    compile_and_check_shader(
        vertex_file_path, vertex_shader_code, vertex_shader_id
    );

    compile_and_check_shader(
        fragment_file_path, fragment_shader_code, fragment_shader_id
    );

    // Link the program
    printf("Linking program\n");
    GLuint program_id = glCreateProgram();
    glAttachShader(program_id, vertex_shader_id);
    glAttachShader(program_id, fragment_shader_id);
    glLinkProgram(program_id);

    // Check the program

    GLint result = GL_FALSE;
    int info_log_length;

    glGetProgramiv(program_id, GL_LINK_STATUS, &result);
    glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &info_log_length);
    if (info_log_length > 0) {
        std::vector<char> program_error_message(info_log_length + 1);
        glGetProgramInfoLog(
            program_id, info_log_length, NULL, &program_error_message[0]
        );
        printf("%s\n", &program_error_message[0]);
    }

    glDetachShader(program_id, vertex_shader_id);
    glDetachShader(program_id, fragment_shader_id);

    glDeleteShader(vertex_shader_id);
    glDeleteShader(fragment_shader_id);

    return program_id;
}
