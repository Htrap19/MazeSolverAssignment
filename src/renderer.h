#ifndef RENDERER_H
#define RENDERER_H

#include "shader.h"
#include "maze.h"
#include "pathfinder.h"

#include <glm/glm.hpp>

struct GLFWwindow;
class Renderer
{
public:
    struct Vertex
    {
        glm::vec3 position{0.0f};
        glm::vec3 color{1.0f};
        float modelIndex = 0.0f;
    };

public:
    Renderer();
    ~Renderer();

    void init();
    bool shouldClose();
    void pollEvents();
    void swapBuffers();

    void drawQuad(const glm::vec3& position,
                  const glm::vec3& scale,
                  const glm::vec3& color = glm::vec3(1.0f));

    void drawMaze(const Maze& maze);
    void drawPath(const std::vector<Point>& path,
                  const PathFinder& finder);

    void flush();

    const
    Shader& mainShader() const;

    void rendererDockspace();

    void showDockspace();

    void bindFramebuffer();
    void unbindFramebuffer();

private:
    void onResize(uint32_t width,
                  uint32_t height);

private:
    GLFWwindow* m_window = nullptr;
    uint32_t m_width = 720;
    uint32_t m_height = 480;

    Shader m_mainShader;
    uint32_t m_vao, m_vbo, m_ibo;
    uint32_t m_fbo, m_fboTexture, m_rbo;

    friend void framebufferSizeCallback(GLFWwindow* window, int width, int height);
};

#endif // RENDERER_H
