#ifndef APPLICATION_H
#define APPLICATION_H

#include <stdint.h>

#include <imgui.h>

struct GLFWwindow;
class Application
{
public:
    Application();
    virtual ~Application();

    virtual void onCreate() {}
    virtual void onUpdate() {}
    virtual void onImGuiUpdate() {}
    virtual void onDestroy() {}

    void init();
    void run();

    bool shouldClose();
    void pollEvents();
    void swapBuffers();

    GLFWwindow* getWindow() const;
    uint32_t getWindowWidth() const;
    uint32_t getWindowHeight() const;

    void showDockspace();
    void rendererDockspace();
    void bindFramebuffer();
    void unbindFramebuffer();

    static Application* getInstance();

protected:
    virtual void onResize(uint32_t width,
                          uint32_t height);
    virtual void onKeyInput(int key,
                            int scancode,
                            int action,
                            int mods);

protected:
    bool m_isViewportFocused = false;
    ImVec2 m_viewportPos;

private:
    GLFWwindow* m_window = nullptr;
    uint32_t m_width = 720;
    uint32_t m_height = 480;
    uint32_t m_fbo, m_fboTexture, m_rbo;

    friend void framebufferSizeCallback(GLFWwindow* window,
                                        int width,
                                        int height);
    friend void keyCallback(GLFWwindow* window,
                            int key,
                            int scancode,
                            int action,
                            int mods);
};

#endif // APPLICATION_H
