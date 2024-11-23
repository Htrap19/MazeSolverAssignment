#include "renderer.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <string>

#define VERTEX_SHADER_PATH   "resources/shaders/vertex.glsl"
#define FRAGMENT_SHADER_PATH "resources/shaders/fragment.glsl"

static void erroCallback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

static void keyCallback(GLFWwindow* window,
                        int key,
                        int scancode,
                        int action,
                        int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void framebufferSizeCallback(GLFWwindow* window,
                             int width,
                             int height)
{
    auto renderer = (Renderer*)glfwGetWindowUserPointer(window);
    renderer->onResize((uint32_t)width,
                       (uint32_t)height);
}

struct RenderData
{
    static constexpr size_t totalVerticesSize = sizeof(Renderer::Vertex) * 3 * 2048;
    size_t vertexOffset = 0;
    size_t indexOffset = 0;
    float modelIndex = 0.0f;

    std::array<glm::mat4, 1000> m_Models;
};

static RenderData s_data;

Renderer::Renderer()
{
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW!" << std::endl;
        return;
    }

    glfwSetErrorCallback(erroCallback);
}

Renderer::~Renderer()
{
    glfwDestroyWindow(m_window);
    glfwTerminate();
}

void Renderer::init()
{
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    m_window = glfwCreateWindow(m_width, m_height, "Maze Solver Vis", nullptr, nullptr);
    if (!m_window)
    {
        std::cerr << "Unable to create GLFW window!" << std::endl;
        return;
    }

    glfwMakeContextCurrent(m_window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD!" << std::endl;
        return;
    }

    glfwSetWindowUserPointer(m_window, this);
    glfwSetKeyCallback(m_window, keyCallback);
    glfwSetFramebufferSizeCallback(m_window, framebufferSizeCallback);
    glfwSwapInterval(1);

    int width, height;
    glfwGetFramebufferSize(m_window, &width, &height);
    glViewport(0, 0, width, height);

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
}

bool Renderer::shouldClose()
{
    return glfwWindowShouldClose(m_window);
}

void Renderer::pollEvents()
{
    glfwPollEvents();
}

void Renderer::swapBuffers()
{
    glfwSwapBuffers(m_window);
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

    s_data.m_Models[s_data.modelIndex] = model;

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
    auto rows = grid.size();    // Number of rows in the maze grid
    auto cells = grid[0].size(); // Number of columns in the maze grid

    // Calculate quad size based on maze grid dimensions
    float quadWidth = 2.0f / (cells * 2 + 1);  // Width per cell (including walls)
    float quadHeight = 2.0f / (rows * 2 + 1);  // Height per cell (including walls)

    for (int y = 0; y < rows * 2 + 1; ++y)
    {
        for (int x = 0; x < cells * 2 + 1; ++x)
        {
            // Calculate position in NDC
            float xPos = -1.0f + x * quadWidth;
            float yPos = 1.0f - y * quadHeight; // Flipping y-axis for OpenGL's NDC

            glm::vec3 color = glm::vec3(1.0f); // Default white color (for debugging)

            if (y % 2 == 0 || x % 2 == 0) // Check if it's a wall location
            {
                if (y % 2 == 0 && x % 2 == 0) // Corner walls
                {
                    color = glm::vec3(0.3f, 0.5f, 0.7f); // Wall color
                }
                else if (y % 2 == 0) // Horizontal walls
                {
                    int cx = x / 2;
                    int cy = (y - 1) / 2;
                    if (cy >= 0 && cy < rows && cx >= 0 && cx < cells && (grid[cy][cx] & S))
                        color = glm::vec3(0.0f); // Path
                    else
                        color = glm::vec3(0.3f, 0.5f, 0.7f); // Wall
                }
                else if (x % 2 == 0) // Vertical walls
                {
                    int cx = (x - 1) / 2;
                    int cy = y / 2;
                    if (cy >= 0 && cy < rows && cx >= 0 && cx < cells && (grid[cy][cx] & E))
                        color = glm::vec3(0.0f); // Path
                    else
                        color = glm::vec3(0.3f, 0.5f, 0.7f); // Wall
                }
                else
                {
                    color = glm::vec3(0.3f, 0.5f, 0.7f); // Default edge wall color
                }
            }
            else // Path cells
            {
                color = glm::vec3(0.0f); // Path color
            }

            // Draw the quad at the calculated position
            drawQuad(glm::vec3(xPos, yPos, 0.0f),  // Quad center
                     glm::vec3(quadWidth, quadHeight, 1.0f), // Quad size
                     color); // Quad color
        }
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
                                    s_data.m_Models[i]);
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
    glViewport(0, 0, width, height);
}
