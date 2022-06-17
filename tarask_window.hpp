#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <string>

namespace tarask
{
    class TaraskWindow
    {
    public:
        TaraskWindow(uint32_t width, uint32_t height, const std::string &title);
        ~TaraskWindow();

        TaraskWindow(const TaraskWindow &) = delete;
        TaraskWindow &operator=(const TaraskWindow &) = delete;

        bool shouldClose() { return glfwWindowShouldClose(m_window); }
        VkExtent2D getExtent()
        {
            return {static_cast<uint32_t>(m_width), static_cast<uint32_t>(m_height)};
        }
        bool wasWindowResized() { return m_framebufferResized; }
        void resetWindowResizedFlag() { m_framebufferResized = false; }

        void createWindowSurface(VkInstance instance, VkSurfaceKHR *surface);

    private:
        static void framebufferResizeCallback(GLFWwindow *window, int width, int height);
        void initWindow();

        int m_width;
        int m_height;
        bool m_framebufferResized = false;

        std::string m_title;
        GLFWwindow *m_window;
    };
} // namespace tarask