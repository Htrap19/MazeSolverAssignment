#include <iostream>
#include <glad/glad.h>

#include "renderer.h"
#include "maze.h"

int main()
{
    Renderer renderer;

    renderer.init();

    Maze m(31, 31);
    m.generate();

    const auto& grid = m.getGrid();
    auto rows = grid.size();
    auto cells = grid[0].size();

    while (!renderer.shouldClose())
    {
        renderer.pollEvents();

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

                glm::vec3 color = !m.isWall(x, y) ?
                                      glm::vec3(0.0f) :
                                      glm::vec3(0.3f, 0.5f, 0.7f);

                renderer.drawQuad(glm::vec3(xPos, yPos, 0.0f),
                                  glm::vec3(quadWidth, quadHeight, 1.0f),
                                  color);
            }
        }

        // renderer.drawQuad(glm::vec3(0.2f, 0.2f, 0.0f), glm::vec3(0.5f, 0.8f, 0.3f)); // greeb quad
        // renderer.drawQuad(glm::vec3(0.3f, 0.3f, 0.0f), glm::vec3(0.3f, 0.5f, 0.8f)); // blue quad

        renderer.flush();

        renderer.swapBuffers();
    }
    return 0;
}
