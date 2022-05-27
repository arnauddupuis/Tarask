#include "tarask_window.hpp"

#include <stdexcept>

namespace tarask
{
    TaraskWindow::TaraskWindow(uint32_t width, uint32_t height, const std::string &title)
        : m_width(width), m_height(height), m_title(title)
    {
        initWindow();
    }

    TaraskWindow::~TaraskWindow()
    {
        glfwDestroyWindow(m_window);
        glfwTerminate();
    }

    void TaraskWindow::initWindow()
    {
        glfwInit();

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        m_window = glfwCreateWindow(m_width, m_height, m_title.c_str(), nullptr, nullptr);
    }

    void TaraskWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR *surface)
    {
        if (glfwCreateWindowSurface(instance, m_window, nullptr, surface) != VK_SUCCESS)
        {
            throw std::runtime_error("TaraskWindow: failed to create window surface.");
        }
    }
} // namespace tarask