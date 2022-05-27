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

        void createWindowSurface(VkInstance instance, VkSurfaceKHR *surface);

    private:
        void initWindow();

        const u_int32_t m_width;
        const u_int32_t m_height;

        std::string m_title;
        GLFWwindow *m_window;
    };
} // namespace tarask