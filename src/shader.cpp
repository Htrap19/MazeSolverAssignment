#include "shader.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <fstream>
#include <sstream>

namespace Util
{
std::string loadShaderSource(const std::string& filePath)
{
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open shader file: " << filePath << std::endl;
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

GLuint compileShader(const std::string& source, GLenum shaderType)
{
    GLuint shader = glCreateShader(shaderType);
    const char* sourceCStr = source.c_str();
    glShaderSource(shader, 1, &sourceCStr, nullptr);
    glCompileShader(shader);

    GLint isCompiled = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE) {
        GLint maxLength = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

        std::string errorLog(maxLength, '\0');
        glGetShaderInfoLog(shader, maxLength, &maxLength, &errorLog[0]);

        std::cerr << "Shader Compilation Error: " << errorLog << std::endl;

        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

GLuint createShaderProgram(const std::string& vertexPath,
                           const std::string& fragmentPath)
{
    std::string vertexSource = loadShaderSource(vertexPath);
    std::string fragmentSource = loadShaderSource(fragmentPath);

    GLuint vertexShader = compileShader(vertexSource, GL_VERTEX_SHADER);
    GLuint fragmentShader = compileShader(fragmentSource, GL_FRAGMENT_SHADER);

    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    GLint isLinked = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &isLinked);
    if (isLinked == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

        std::string errorLog(maxLength, '\0');
        glGetProgramInfoLog(program, maxLength, &maxLength, &errorLog[0]);

        std::cerr << "Shader Linking Error: " << errorLog << std::endl;

        glDeleteProgram(program);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return 0;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}
}

Shader::Shader()
{
}

void Shader::load(const std::string &vertexPath,
                  const std::string &fragmentPath)
{
    m_programId = Util::createShaderProgram(vertexPath, fragmentPath);
}

void Shader::use()
{
    glUseProgram(m_programId);
}

void Shader::setUniformVec4(const std::string& name,
                            const glm::vec4& value)
{
    glUniform4fv(getUniformLocation(name), 1, glm::value_ptr(value));
}

void Shader::setUniformMat4(const std::string &name, const glm::mat4 &value)
{
    glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
}

int Shader::getUniformLocation(const std::string &name)
{
    if (m_uniformLocations.find(name) != m_uniformLocations.end())
    {
        int location = glGetUniformLocation(m_programId, name.c_str());
        m_uniformLocations[name] = location;
    }

    return m_uniformLocations[name];
}
