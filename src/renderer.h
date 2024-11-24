#ifndef RENDERER_H
#define RENDERER_H

#include "shader.h"
#include "maze.h"
#include "pathfinder.h"

#include <glm/glm.hpp>

#include <unordered_set>

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

    void drawQuad(const glm::vec3& position,
                  const glm::vec3& scale,
                  const glm::vec3& color = glm::vec3(1.0f));

    void drawMaze(const Maze& maze);
    void drawPath(const std::vector<Point>& path,
                  const PathFinder& finder);

    void visPath(const Maze& maze,
                 const std::vector<Point>& point,
                 const std::unordered_set<Point, PointHash>& openSet);

    void flush();

    const
    Shader& mainShader() const;

private:
    void onResize(uint32_t width,
                  uint32_t height);

private:
    Shader m_mainShader;
    uint32_t m_vao, m_vbo, m_ibo;
    uint32_t m_width, m_height;
};

#endif // RENDERER_H
