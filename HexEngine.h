#include <GLFW/glfw3.h>
#include "VulkanInstance.h"

class HexEngine {
    public:
        void run();

    private:

		const int WIDTH = 800;
		const int HEIGHT = 600;
		GLFWwindow* window;

		void initWindow();
        void initVulkan();
        void mainLoop();
        void cleanup();

		VulkanInstance vulkanInstance;
        

};