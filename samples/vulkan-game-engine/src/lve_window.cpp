#include "lve_window.hpp"

#include <stdexcept>

namespace lve {
    LveWindow::LveWindow(int w, int h, std::string name) :
        width(w),
        height(h),
        windowName{ name } {
        initWindow();
    }

    LveWindow::~LveWindow() {
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    void LveWindow::initWindow() {
        glfwInit();
        // not to create glfw context when glfw window created, we don't need that since we use vulkan
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        // disable window resize
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        // initialize window pointer
        window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
    }

    void LveWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface) {
        if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS) {
            throw std::runtime_error("failed to create window surface");
        }
    }
}