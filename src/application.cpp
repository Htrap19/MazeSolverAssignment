#include "application.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <iostream>

static void erroCallback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

void keyCallback(GLFWwindow* window,
                 int key,
                 int scancode,
                 int action,
                 int mods)
{
    auto app = (Application*)glfwGetWindowUserPointer(window);
    app->onKeyInput(key, scancode, action, mods);
}

void framebufferSizeCallback(GLFWwindow* window,
                             int width,
                             int height)
{
    auto app = (Application*)glfwGetWindowUserPointer(window);
    app->onResize((uint32_t)width,
                  (uint32_t)height);
}

static Application* s_Ins = nullptr;

Application::Application()
{
    if (s_Ins != nullptr)
    {
        std::cerr << "Application is already created!" << std::endl;
        return;
    }
    s_Ins = this;

    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW!" << std::endl;
        return;
    }

    glfwSetErrorCallback(erroCallback);
}

Application::~Application()
{
    onDestroy();

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(m_window);
    glfwTerminate();
}

void Application::init()
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

    // Generate and bind the framebuffer
    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    // Create a texture to render to
    glGenTextures(1, &m_fboTexture);
    glBindTexture(GL_TEXTURE_2D, m_fboTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_fboTexture, 0);

    // Create a renderbuffer object for depth and stencil
    glGenRenderbuffers(1, &m_rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, m_rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_width, m_height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_rbo);

    // Check framebuffer completeness
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cerr << "Framebuffer not complete!" << std::endl;

    // Unbind framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    onCreate();
}

void Application::run()
{
    while (!shouldClose())
    {
        pollEvents();

        bindFramebuffer();

        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        onUpdate();

        unbindFramebuffer();

        rendererDockspace();

        swapBuffers();
    }
}

bool Application::Application::shouldClose()
{
    return glfwWindowShouldClose(m_window);
}

void Application::Application::pollEvents()
{
    glfwPollEvents();
}

void Application::Application::swapBuffers()
{
    glfwSwapBuffers(m_window);
}

GLFWwindow *Application::getWindow() const
{
    return m_window;
}

uint32_t Application::getWindowWidth() const
{
    return m_width;
}

uint32_t Application::getWindowHeight() const
{
    return m_height;
}

void Application::showDockspace()
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

void Application::rendererDockspace()
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
    m_isViewportFocused = ImGui::IsWindowFocused();
    m_viewportPos = ImGui::GetWindowPos();
    ImGui::End();

    onImGuiUpdate();

    ImGui::Render();

    glViewport(0, 0, m_width, m_height);

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

void Application::bindFramebuffer()
{
    // Bind framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    // Set the viewport to match the framebuffer
    glViewport(0, 0, m_width, m_height);
}

void Application::unbindFramebuffer()
{
    // Unbind framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Application *Application::getInstance()
{
    return s_Ins;
}

void Application::onResize(uint32_t width, uint32_t height)
{
    m_width = width;
    m_height = height;
    glViewport(0, 0, width, height);
    glBindTexture(GL_TEXTURE_2D, m_fboTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    glBindRenderbuffer(GL_RENDERBUFFER, m_rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
}

void Application::onKeyInput(int key,
                             int scancode,
                             int action,
                             int mods)
{
}
