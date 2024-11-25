#include "renderer.h"

#include "application.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>

#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>

#include <string>

#define VERTEX_SHADER_PATH   "resources/shaders/vertex.glsl"
#define FRAGMENT_SHADER_PATH "resources/shaders/fragment.glsl"

struct RenderData
{
    static constexpr size_t totalVerticesSize = sizeof(Renderer::Vertex) * 3 * 2048;
    size_t vertexOffset = 0;
    size_t indexOffset = 0;
    float modelIndex = 0.0f;

    std::array<glm::mat4, 1000> m_models;
};

static RenderData s_data;

Renderer::Renderer()
{
}

Renderer::~Renderer()
{
}

void Renderer::init()
{
    m_mainShader.load(VERTEX_SHADER_PATH, FRAGMENT_SHADER_PATH);

    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * 3 * 2048, nullptr, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, modelIndex));
    glEnableVertexAttribArray(2);

    glGenBuffers(1, &m_ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * 3 * 2048, nullptr, GL_DYNAMIC_DRAW);

    m_width = Application::getInstance()->getWindowWidth();
    m_height = Application::getInstance()->getWindowHeight();
}

void Renderer::drawQuad(const glm::vec3& position,
                        const glm::vec3& scale,
                        const glm::vec3& color)
{
    if (s_data.modelIndex >= 1000)
        flush();

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::scale(model, scale);

    s_data.m_models[s_data.modelIndex] = model;

    Vertex vertices[] =
    {
        { {-0.5f, -0.5f, 0.0f}, color, s_data.modelIndex },
        { {-0.5f,  0.5f, 0.0f}, color, s_data.modelIndex },
        { { 0.5f,  0.5f, 0.0f}, color, s_data.modelIndex },
        { { 0.5f, -0.5f, 0.0f}, color, s_data.modelIndex }
    };

    GLuint indices[] =
    {
        0, 1, 2, // first triangle (bottom left - top left - top right)
        0, 2, 3
    };

    for (auto& idx : indices)
        idx += s_data.vertexOffset;

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferSubData(GL_ARRAY_BUFFER,
                    sizeof(Vertex) * s_data.vertexOffset,
                    sizeof(vertices),
                    vertices);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER,
                    sizeof(uint32_t) * s_data.indexOffset,
                    sizeof(indices),
                    indices);

    s_data.vertexOffset += 4;
    s_data.indexOffset  += 6;
    s_data.modelIndex   += 1.0f;
}

void Renderer::drawMaze(const Maze &maze)
{
    const auto& grid = maze.getGrid();
    auto rows = grid.size();
    auto cells = grid[0].size();

    // Calculate quad size based on window dimensions
    float quadWidth = 2.0f / rows;
    float quadHeight = 2.0f / cells;

    for (int y = 0; y < rows; ++y)
    {
        for (int x = 0; x < cells; ++x)
        {
            // Calculate position in normalized device coordinates
            float xPos = -1.0f + x * quadWidth + quadWidth / 2.0f;
            float yPos = -1.0f + y * quadHeight + quadHeight / 2.0f;

            glm::vec3 color = maze.isWall(x, y) ?
                                  glm::vec3(0.3f, 0.5f, 0.7f) :
                                  glm::vec3(0.0f);


            drawQuad(glm::vec3(xPos, yPos, 0.0f),
                     glm::vec3(quadWidth, quadHeight, 1.0f),
                     color);
        }
    }
}

void Renderer::drawPath(const std::vector<Point> &path,
                        const PathFinder& finder)
{
    const auto& grid = finder.getMaze()->getGrid();
    auto rows = grid.size();
    auto cells = grid[0].size();

    // Calculate quad size based on window dimensions
    float quadWidth = 2.0f / rows;
    float quadHeight = 2.0f / cells;

    // Mark path
    for (const auto& p : path)
    {
        // Calculate position in normalized device coordinates
        float xPos = -1.0f + p.x * quadWidth + quadWidth / 2.0f;
        float yPos = -1.0f + p.y * quadHeight + quadHeight / 2.0f;

        drawQuad(glm::vec3(xPos, yPos, 0.0f),
                 glm::vec3(quadWidth * 0.2f, quadHeight * 0.2f, 1.0f),
                 glm::vec3(1.0f, 1.0f, 1.0f));
    }
}

void Renderer::visPath(const Maze& maze,
                       const std::vector<Point>& path,
                       const std::unordered_set<Point,
                                                PointHash>& openSet)
{
    const auto& grid = maze.getGrid();
    auto rows = grid.size();
    auto cells = grid[0].size();

    // Calculate quad size based on window dimensions
    float quadWidth = 2.0f / rows;
    float quadHeight = 2.0f / cells;
    for (auto& p : path)
    {
        float x1 = (float)p.x / maze.getWidth() * 2.0f - 1.0f;
        float y1 = (float)p.y / maze.getHeight() * 2.0f - 1.0f;

        drawQuad(glm::vec3(x1, y1, 0.0),
                 glm::vec3(quadWidth, quadHeight, 0.0f),
                 glm::vec3(0.0f, 1.0f, 0.0f));
    }

    for (const auto& p : openSet)
    {
        float x1 = (float)p.x / maze.getWidth() * 2.0f - 1.0f;
        float y1 = (float)p.y / maze.getHeight() * 2.0f - 1.0f;

        drawQuad(glm::vec3(x1, y1, 0.0),
                 glm::vec3(quadWidth, quadHeight, 0.0f),
                 glm::vec3(0.0f, 1.0f, 0.0f));
    }
}

void Renderer::flush()
{
    if (s_data.indexOffset == 0)
        return; // Nothing to draw

    m_mainShader.use();
    for (uint32_t i = 0; i < s_data.modelIndex; i++)
    {
        m_mainShader.setUniformMat4("u_Models[" + std::to_string(i) + "]",
                                    s_data.m_models[i]);
    }

    glBindVertexArray(m_vao);
    glDrawElements(GL_TRIANGLES,
                   s_data.indexOffset,
                   GL_UNSIGNED_INT, nullptr);

    // glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    // glBufferSubData(GL_ARRAY_BUFFER,
    //                 0,
    //                 sizeof(Vertex) * 3 * 2048,
    //                 nullptr);

    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
    // glBufferSubData(GL_ELEMENT_ARRAY_BUFFER,
    //                 0,
    //                 sizeof(uint32_t) * 3 * 2048,
    //                 nullptr);

    s_data.vertexOffset = 0;
    s_data.indexOffset = 0;
    s_data.modelIndex = 0;
}

const
Shader& Renderer::mainShader() const
{
    return m_mainShader;
}

void Renderer::onResize(uint32_t width,
                        uint32_t height)
{
    m_width = width;
    m_height = height;
}
