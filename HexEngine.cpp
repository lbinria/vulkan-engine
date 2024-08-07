#include "HexEngine.h"

void HexEngine::run() {
	initWindow();
	initVulkan();
	mainLoop();
	cleanup();
}

void HexEngine::initWindow() {
	glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
}

void HexEngine::initVulkan() {
	// vulkanInstance = VulkanInstance();
	vulkanInstance.create();
	auto pd = vulkanInstance.pickPhysicalDevice();

}

void HexEngine::mainLoop() {
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }
}

void HexEngine::cleanup() {
	vulkanInstance.cleanup();

    glfwDestroyWindow(window);
    glfwTerminate();
}
