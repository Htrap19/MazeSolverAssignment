#include "renderer.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>

#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>

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
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

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

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    const char* glsl_version = "#version 400";
    ImGui_ImplGlfw_InitForOpenGL(m_window, true);
#ifdef __EMSCRIPTEN__
    ImGui_ImplGlfw_InstallEmscriptenCallbacks(window, "#canvas");
#endif
    ImGui_ImplOpenGL3_Init(glsl_version);

    m_mainShader.load(VERTEX_SHADER_PATH, FRAGMENT_SHADER_PATH);

    // Generate and bind the framebuffer
    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    // Create a texture to render to
    glGenTextures(1, &m_fboTexture);
    glBindTexture(GL_TEXTURE_2D, m_fboTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_fboTexture, 0);

    // Create a renderbuffer object for depth and stencil
    glGenRenderbuffers(1, &m_rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, m_rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_rbo);

    // Check framebuffer completeness
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cerr << "Framebuffer not complete!" << std::endl;

    // Unbind framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

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
    const auto& grid = finder.getMaze().getGrid();
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
                 glm::vec3(0.5f, 0.7f, 0.3f));
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

void Renderer::bindFramebuffer()
{
    // Bind framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    // Set the viewport to match the framebuffer
    glViewport(0, 0, m_width, m_height);
}

void Renderer::unbindFramebuffer()
{
    // Unbind framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

const
Shader& Renderer::mainShader() const
{
    return m_mainShader;
}

void Renderer::showDockspace()
{
    // Get the main viewport
    ImGuiViewport* viewport = ImGui::GetMainViewport();

    // Set up the Dockspace window
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);

    // Create the Dockspace window flags
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

    // Begin the Dockspace window
    ImGui::Begin("Dockspace", nullptr, window_flags);
    ImGui::PopStyleVar(3);

    // Create the Dockspace
    ImGuiID dockspace_id = ImGui::GetID("MyDockspace");
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);

    ImGui::End();
}

void Renderer::rendererDockspace()
{
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    showDockspace();

    ImGui::Begin("Viewport");
    auto windowSize = ImGui::GetContentRegionAvail();
    ImGui::Image((ImTextureID)(uintptr_t)m_fboTexture,
                 ImVec2(windowSize.x, windowSize.y));
    ImGui::End();

    ImGui::Begin("Maze");
    static const char* mazeAlgos[] =
        { "Recursive Backtracking", "BFS", "DFS" };
    static int currentAlgo = 0;
    if (ImGui::Combo("Algo", &currentAlgo, mazeAlgos, IM_ARRAYSIZE(mazeAlgos)))
    {
        std::cout << "Selected: " << mazeAlgos[currentAlgo] << std::endl;
    }
    ImGui::Button("Generate");
    ImGui::End();

    ImGui::Render();

    int display_w, display_h;
    glfwGetFramebufferSize(m_window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    ImGuiIO& io = ImGui::GetIO(); (void)io;

    // Update and Render additional Platform Windows
    // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
    //  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        GLFWwindow* backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }
}

void Renderer::onResize(uint32_t width,
                        uint32_t height)
{
    m_width = width;
    m_height = height;
    glViewport(0, 0, width, height);

    glBindTexture(GL_TEXTURE_2D, m_fboTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    glBindRenderbuffer(GL_RENDERBUFFER, m_rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
}
