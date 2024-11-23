#include <iostream>
#include <glad/glad.h>

#include "renderer.h"
#include "maze.h"
#include "pathfinder.h"

int main()
{
    Renderer renderer;

    renderer.init();

    Maze m(21, 21);
    m.generate(time(nullptr));

    Point start{0, 0}, end{20, 20};

    PathFinder finder(m);
    auto path = finder.findPath(start, end);
    finder.printPath(path);

    while (!renderer.shouldClose())
    {
        renderer.pollEvents();

        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        renderer.drawMaze(m);
        renderer.drawPath(path, finder);

        // renderer.drawQuad(glm::vec3(0.2f, 0.2f, 0.0f), glm::vec3(0.5f, 0.8f, 0.3f)); // greeb quad
        // renderer.drawQuad(glm::vec3(0.3f, 0.3f, 0.0f), glm::vec3(0.3f, 0.5f, 0.8f)); // blue quad

        renderer.flush();

        renderer.swapBuffers();
    }
    return 0;
}
