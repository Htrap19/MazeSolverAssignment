#ifndef SHADER_H
#define SHADER_H

#include <glm/glm.hpp>

#include <stdint.h>
#include <string>
#include <unordered_map>

class Shader
{
public:
    Shader();

    void load(const std::string& vertexPath,
              const std::string& fragmentPath);

    void use();

    void setUniformVec4(const std::string& name,
                        const glm::vec4& value);
    void setUniformMat4(const std::string& name,
                        const glm::mat4& value);

    inline
    uint32_t getID() const {return m_programId;}


private:
    int getUniformLocation(const std::string& name);

private:
    uint32_t m_programId;
    std::unordered_map<std::string, uint32_t> m_uniformLocations;
};

#endif // SHADER_H
