#include "renderer.h"
#include "maze.h"
#include "pathfinder.h"

#include <glad/glad.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

int main()
{
    Renderer renderer;

    renderer.init();

    Maze m(51, 51);
    m.generate(time(nullptr));

    Point start{0, 0}, end{50, 50};

    PathFinder finder(m);
    auto path = finder.findPath(start, end);
    finder.printPath(path);

    while (!renderer.shouldClose())
    {
        renderer.pollEvents();

        renderer.bindFramebuffer();

        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        renderer.drawMaze(m);
        renderer.drawPath(path, finder);

        // renderer.drawQuad(glm::vec3(0.2f, 0.2f, 0.0f), glm::vec3(0.5f, 0.8f, 0.3f)); // greeb quad
        // renderer.drawQuad(glm::vec3(0.3f, 0.3f, 0.0f), glm::vec3(0.3f, 0.5f, 0.8f)); // blue quad

        renderer.flush();

        renderer.unbindFramebuffer();

        renderer.rendererDockspace(); // Start Imgui frame

        renderer.swapBuffers();
    }
    return 0;
}
