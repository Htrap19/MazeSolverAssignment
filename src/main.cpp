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



    while (!renderer.shouldClose())
    {
        renderer.pollEvents();

        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        renderer.drawMaze(m);

        // renderer.drawQuad(glm::vec3(0.2f, 0.2f, 0.0f), glm::vec3(0.5f, 0.8f, 0.3f)); // greeb quad
        // renderer.drawQuad(glm::vec3(0.3f, 0.3f, 0.0f), glm::vec3(0.3f, 0.5f, 0.8f)); // blue quad

        renderer.flush();

        renderer.swapBuffers();
    }
    return 0;
}
