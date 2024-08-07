#pragma once

#include "HexWindow.h"
#include "hex_device.h"
#include "HexSwapChain.h"

#include <memory>
#include <vector>
#include <cassert>

namespace hex {
	class HexRenderer {
		public:
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;

		HexRenderer(HexWindow &window, HexDevice &device);
		~HexRenderer();

		HexRenderer(const HexRenderer&) = delete;
		HexRenderer &operator=(const HexRenderer &) = delete;

		VkRenderPass getSwapChainRenderPass() const { return hexSwapChain->getRenderPass(); }

		bool isFrameInProgress() const { return isFrameStarted; }

		VkCommandBuffer getCurrentCommandBuffer() const {
			assert(isFrameStarted && "Cannot get command buffer when frame not in progress");
			return commandBuffers[currentFrameIndex];
		}

		int getFrameIndex() const { 
			assert(isFrameStarted && "Cannot get frame index when frame is not in progress");
			return currentFrameIndex; 
		}

		VkCommandBuffer beginFrame();
		void endFrame();
		void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
		void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

		private:

		void createCommandBuffers();
		void freeCommandBuffers();
		void recreateSwapChain();

		HexWindow& hexWindow;
		HexDevice& hexDevice;
		std::unique_ptr<HexSwapChain> hexSwapChain;
		std::vector<VkCommandBuffer> commandBuffers;

		uint32_t currentImageIndex;
		int currentFrameIndex{0};
		bool isFrameStarted{false};

	};
}