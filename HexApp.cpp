#include "HexApp.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

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
		createPipeline();
	}

	HexApp::~HexApp() {
		vkDestroyPipelineLayout(hexDevice.device(), pipelineLayout, nullptr);
	}

	void HexApp::run() {
		while (!hexWindow.shouldClose()) {
			glfwPollEvents();
			
			if (auto commandBuffer = hexRenderer.beginFrame()) {
				hexRenderer.beginSwapChainRenderPass(commandBuffer);
				renderGameObjectObjects(commandBuffer);
				hexRenderer.endSwapChainRenderPass(commandBuffer);
				hexRenderer.endFrame();
			}

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
		triangle.transform2d.scale = {2.f, .5f};
		triangle.transform2d.rotation = glm::two_pi<float>() * .25f;

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
		assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

		PipelineConfigInfo pipelineConfig{};
		HexPipeline::defaultPipelineConfigInfo(pipelineConfig);
		pipelineConfig.renderPass = hexRenderer.getSwapChainRenderPass();
		pipelineConfig.pipelineLayout = pipelineLayout;

		hexPipeline = std::make_unique<HexPipeline>(
			hexDevice,
			"shaders/simple_shader.vert.spv",
			"shaders/simple_shader.frag.spv",
			pipelineConfig
		);
	}

	void HexApp::renderGameObjectObjects(VkCommandBuffer commandBuffer) {
		hexPipeline->bind(commandBuffer);

		for (auto &gameObject : gameObjects) {
			gameObject.transform2d.rotation = glm::mod(gameObject.transform2d.rotation + 0.001f, glm::two_pi<float>());

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

}