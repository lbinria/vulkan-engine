#include "HexWindow.h"
#include <stdexcept>

namespace hex {

	HexWindow::HexWindow(int w, int h, std::string name) : width{w}, height{h}, windowName{name} {
		initWindow();
	}

	HexWindow::~HexWindow() {
		glfwDestroyWindow(window);
		glfwTerminate();
	}

	void HexWindow::initWindow() {
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

		window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
		glfwSetWindowUserPointer(window, this);
		glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
	}

	void HexWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR *surface) {
		if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create window surface");
		}
	}

	void HexWindow::framebufferResizeCallback(GLFWwindow *window, int width, int height) {
		auto hexWindow = reinterpret_cast<HexWindow*>(glfwGetWindowUserPointer(window));
		hexWindow->framebufferResize = true;
		hexWindow->width = width;
		hexWindow->height = height;
	}

}