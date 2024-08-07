#include "HexRenderer.h"

#include <array>
#include <stdexcept>
#include <cassert>

namespace hex {

	HexRenderer::HexRenderer(HexWindow &window, HexDevice &device) : hexWindow{window}, hexDevice{device} {
		recreateSwapChain();
		createCommandBuffers();
	}

	HexRenderer::~HexRenderer() {
		freeCommandBuffers();
	}

	void HexRenderer::recreateSwapChain() {
		auto extent = hexWindow.getExtent();
		// Wait to windows to have 2d size
		while (extent.width == 0 || extent.height == 0) {
			extent = hexWindow.getExtent();
			glfwWaitEvents();
		}

		vkDeviceWaitIdle(hexDevice.device());

		if (hexSwapChain == nullptr) {
			hexSwapChain = std::make_unique<HexSwapChain>(hexDevice, extent);
		} else {
			std::shared_ptr<HexSwapChain> oldSwapChain = std::move(hexSwapChain);
			hexSwapChain = std::make_unique<HexSwapChain>(hexDevice, extent, oldSwapChain);

			if (!oldSwapChain->compareSwapFormat(*hexSwapChain.get())) {
				// Maybe not throw an error here, make a callback to get error
				throw std::runtime_error("Swap chain image(or depth) format has changed");
			}

		}

	}

	void HexRenderer::createCommandBuffers() {
		commandBuffers.resize(HexSwapChain::MAX_FRAMES_IN_FLIGHT);
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY; // Can be submitted
		allocInfo.commandPool = hexDevice.getCommandPool();
		allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

		if (vkAllocateCommandBuffers(hexDevice.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
			throw std::runtime_error("Failed to allocate command buffers");
		}
	}

	void HexRenderer::freeCommandBuffers() {
		vkFreeCommandBuffers(
			hexDevice.device(), 
			hexDevice.getCommandPool(), 
			static_cast<uint32_t>(commandBuffers.size()), 
			commandBuffers.data()
		);

		commandBuffers.clear();
	}

	VkCommandBuffer HexRenderer::beginFrame() {
		assert(!isFrameStarted && "Can't call beginFrame while already in progress");
		

		auto result = hexSwapChain->acquireNextImage(&currentImageIndex);

		// Size has changed
		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			recreateSwapChain();
			return nullptr;
		}

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			throw std::runtime_error("Failed to acquire swap chain image");
		}

		isFrameStarted = true;

		auto commandBuffer = getCurrentCommandBuffer();

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		
		if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("Failed to begin recording command buffer");
		}

		return commandBuffer;
	}

	void HexRenderer::endFrame() {
		assert(isFrameStarted && "Can't call endFrame while frame is not in progress");

		auto commandBuffer = getCurrentCommandBuffer();
		if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
			throw std::runtime_error("Failed to record command buffer");
		}

		VkResult result = hexSwapChain->submitCommandBuffers(&commandBuffer, &currentImageIndex);

		// Size has changed or windows resize callback was called
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || hexWindow.wasWindowResized()) {
			recreateSwapChain();
		} else if (result != VK_SUCCESS) {
			throw std::runtime_error("Failed to present swap chain image");
		}

		isFrameStarted = false;
		currentFrameIndex = (currentFrameIndex + 1) % HexSwapChain::MAX_FRAMES_IN_FLIGHT;
	}

	void HexRenderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer) {
		assert(isFrameStarted && "Can't call beginSwapChainRenderPass while frame is not in progress");
		assert(commandBuffer == getCurrentCommandBuffer() && "Can't begin render pass on a command buffer from a different frame");

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = hexSwapChain->getRenderPass();
		renderPassInfo.framebuffer = hexSwapChain->getFrameBuffer(currentImageIndex);
		renderPassInfo.renderArea.offset = {0, 0};
		renderPassInfo.renderArea.extent = hexSwapChain->getSwapChainExtent();

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = {0.1f, 0.1f, 0.1f};
		clearValues[1].depthStencil.depth = 1.0f;
		clearValues[1].depthStencil.stencil = 0;

		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(hexSwapChain->getSwapChainExtent().width);
		viewport.height = static_cast<float>(hexSwapChain->getSwapChainExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		VkRect2D scissor{};
		scissor.offset = {0, 0};
		scissor.extent = {hexSwapChain->getSwapChainExtent().width, hexSwapChain->getSwapChainExtent().height};
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
	}
	
	void HexRenderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer) {
		assert(isFrameStarted && "Can't call endSwapChainRenderPass while already in progress");
		assert(commandBuffer == getCurrentCommandBuffer() && "Can't end render pass on a command buffer from a different frame");

		vkCmdEndRenderPass(commandBuffer);
	}

}