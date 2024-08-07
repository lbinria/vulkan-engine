#pragma once

#include "HexWindow.h"
#include "HexPipeline.h"
#include "hex_device.h"
#include "HexSwapChain.h"
#include "HexGameObject.h"

#include <memory>
#include <vector>

namespace hex {
	class HexApp {
		public:
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;

		HexApp();
		~HexApp();

		HexApp(const HexApp&) = delete;
		HexApp &operator=(const HexApp &) = delete;

		void run();

		private:

		void loadGameObjects();
		void createPipelineLayout();
		void createPipeline();
		void createCommandBuffers();
		void freeCommandBuffers();
		void drawFrame();
		void recreateSwapChain();
		void recordCommandBuffer(int imageIndex);
		void renderGameObjectObjects(VkCommandBuffer commandBuffer);

		HexWindow hexWindow{WIDTH, HEIGHT, "Hello !"};
		HexDevice hexDevice{hexWindow};
		// HexSwapChain hexSwapChain{hexDevice, hexWindow.getExtent()};
		std::unique_ptr<HexSwapChain> hexSwapChain;

		std::unique_ptr<HexPipeline> hexPipeline;
		VkPipelineLayout pipelineLayout;
		std::vector<VkCommandBuffer> commandBuffers;
		std::vector<HexGameObject> gameObjects;
		// HexPipeline hexPipeline{
		// 	hexDevice,
		// 	"shaders/simple_shader.vert.spv", 
		// 	"shaders/simple_shader.frag.spv",
		// 	HexPipeline::defaultPipelineConfigInfo(WIDTH, HEIGHT)
		// };
	};
}