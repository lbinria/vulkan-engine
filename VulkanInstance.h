#include <vulkan/vulkan.h>
#include "VulkanPhysicalDevice.h"

class VulkanInstance {
    public:
        
		void create();
		VulkanPhysicalDevice pickPhysicalDevice();
		void cleanup();

    private:
		bool checkValidationLayerSupport();

		bool isDeviceSuitable(VkPhysicalDevice device);

		VkInstance instance;

};