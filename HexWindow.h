#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>
#include <vulkan/vulkan.h>

namespace hex {

	class HexWindow {
		public:
			HexWindow(int w, int h, std::string name);
			~HexWindow();
			
			HexWindow(const HexWindow &) = delete;
			HexWindow &operator=(const HexWindow &) = delete;

			bool shouldClose() { return glfwWindowShouldClose(window); }
			VkExtent2D getExtent() { return {static_cast<uint32_t>(width), static_cast<uint32_t>(height)}; }
			bool wasWindowResized() { return framebufferResize; }
			void resetWindowResizedFlag() { framebufferResize = false; }

			void createWindowSurface(VkInstance instance, VkSurfaceKHR *surface);

		private:
			static void framebufferResizeCallback(GLFWwindow *window, int width, int height);
			void initWindow();

			int width;
			int height;
			bool framebufferResize = false;

			std::string windowName;
			GLFWwindow* window;

	};

}