#ifndef APPLICATION_H
#define APPLICATION_H

#include <stdint.h>

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

private:
    void onResize(uint32_t width,
                  uint32_t height);

private:
    GLFWwindow* m_window = nullptr;
    uint32_t m_width = 720;
    uint32_t m_height = 480;
    uint32_t m_fbo, m_fboTexture, m_rbo;

    friend void framebufferSizeCallback(GLFWwindow* window,
                                        int width,
                                        int height);
};

#endif // APPLICATION_H
