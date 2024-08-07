#include "HexApp.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <array>
#include <stdexcept>
#include <cassert>

namespace hex {

	struct SimplePushConstantData {
		glm::mat2 transform{1.f}; // identity matrix (initialize diagonal by 1)
		glm::vec2 offset;
		alignas(16) glm::vec3 color;
	};

	HexApp::HexApp() {
		loadGameObjects();
		createPipelineLayout();
		recreateSwapChain();
		createCommandBuffers();
	}

	HexApp::~HexApp() {
		vkDestroyPipelineLayout(hexDevice.device(), pipelineLayout, nullptr);
	}

	void HexApp::run() {
		while (!hexWindow.shouldClose()) {
			glfwPollEvents();
			drawFrame();
		}

		vkDeviceWaitIdle(hexDevice.device());
	}

	void HexApp::loadGameObjects() {
		std::vector<HexModel::Vertex> vertices {
			{{0.0f, -0.8f}, {1.0f, 0.0f, 0.0f}},
			{{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
			{{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
		};

		auto hexModel = std::make_shared<HexModel>(hexDevice, vertices);
		auto triangle = HexGameObject::createGameObject();
		triangle.model = hexModel;
		triangle.color = {0.1f, 0.8f, 0.1f};
		triangle.transform2d.translation.x = .2f;

		gameObjects.push_back(std::move(triangle));
	}

	void HexApp::createPipelineLayout() {

		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0; // Used if we use separate range between vertex and fragment shader
		pushConstantRange.size = sizeof(SimplePushConstantData);


		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0;
		pipelineLayoutInfo.pSetLayouts = nullptr;
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

		if (vkCreatePipelineLayout(hexDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create pipeline layout");
		}
	}

	void HexApp::createPipeline() {
		assert(hexSwapChain != nullptr && "Cannot create pipeline before swap chain");
		assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

		PipelineConfigInfo pipelineConfig{};
		HexPipeline::defaultPipelineConfigInfo(pipelineConfig);
		pipelineConfig.renderPass = hexSwapChain->getRenderPass();
		pipelineConfig.pipelineLayout = pipelineLayout;

		hexPipeline = std::make_unique<HexPipeline>(
			hexDevice,
			"shaders/simple_shader.vert.spv",
			"shaders/simple_shader.frag.spv",
			pipelineConfig
		);
	}

	void HexApp::recreateSwapChain() {
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
			hexSwapChain = std::make_unique<HexSwapChain>(hexDevice, extent, std::move(hexSwapChain));
			if (hexSwapChain->imageCount() != commandBuffers.size()) {
				freeCommandBuffers();
				createCommandBuffers();
			}
		}

		// if render pass compatible do nothing else
		createPipeline();
	}

	void HexApp::createCommandBuffers() {
		commandBuffers.resize(hexSwapChain->imageCount());
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY; // Can be submitted
		allocInfo.commandPool = hexDevice.getCommandPool();
		allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

		if (vkAllocateCommandBuffers(hexDevice.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
			throw std::runtime_error("Failed to allocate command buffers");
		}
	}

	void HexApp::freeCommandBuffers() {
		vkFreeCommandBuffers(
			hexDevice.device(), 
			hexDevice.getCommandPool(), 
			static_cast<uint32_t>(commandBuffers.size()), 
			commandBuffers.data()
		);

		commandBuffers.clear();
	}

	void HexApp::recordCommandBuffer(int imageIndex) {

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		
		if (vkBeginCommandBuffer(commandBuffers[imageIndex], &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("Failed to begin recording command buffer");
		}

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = hexSwapChain->getRenderPass();
		renderPassInfo.framebuffer = hexSwapChain->getFrameBuffer(imageIndex);
		renderPassInfo.renderArea.offset = {0, 0};
		renderPassInfo.renderArea.extent = hexSwapChain->getSwapChainExtent();

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = {0.1f, 0.1f, 0.1f};
		clearValues[1].depthStencil.depth = 1.0f;
		clearValues[1].depthStencil.stencil = 0;

		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

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
		vkCmdSetViewport(commandBuffers[imageIndex], 0, 1, &viewport);
		vkCmdSetScissor(commandBuffers[imageIndex], 0, 1, &scissor);

		renderGameObjectObjects(commandBuffers[imageIndex]);

		vkCmdEndRenderPass(commandBuffers[imageIndex]);

		if (vkEndCommandBuffer(commandBuffers[imageIndex]) != VK_SUCCESS) {
			throw std::runtime_error("Failed to record command buffer");
		}
	}

	void HexApp::renderGameObjectObjects(VkCommandBuffer commandBuffer) {
		hexPipeline->bind(commandBuffer);

		for (auto &gameObject : gameObjects) {
			

			SimplePushConstantData push{};
			push.offset = gameObject.transform2d.translation;
			push.color = gameObject.color;
			push.transform = gameObject.transform2d.mat2();

			vkCmdPushConstants(
				commandBuffer, 
				pipelineLayout, 
				VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 
				0, 
				sizeof(SimplePushConstantData), &push
			);

			gameObject.model->bind(commandBuffer);
			gameObject.model->draw(commandBuffer);
		}
	}

	void HexApp::drawFrame() {
		uint32_t imageIndex;

		auto result = hexSwapChain->acquireNextImage(&imageIndex);

		// Size has changed
		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			recreateSwapChain();
			return;
		}

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			throw std::runtime_error("Failed to acquire swap chain image");
		}

		recordCommandBuffer(imageIndex);
		result = hexSwapChain->submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);

		// Size has changed or windows resize callback was called
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || hexWindow.wasWindowResized()) {
			recreateSwapChain();
			return;
		}

		if (result != VK_SUCCESS) {
			throw std::runtime_error("Failed to present swap chain image");
		}
	}

}