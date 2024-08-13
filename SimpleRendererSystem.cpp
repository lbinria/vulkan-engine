#include "SimpleRendererSystem.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <array>
#include <stdexcept>
#include <cassert>

namespace hex {

	struct SimplePushConstantData {
		glm::mat4 transform{1.f}; // identity matrix (initialize diagonal by 1)
		alignas(16) glm::vec3 color;
	};

	SimpleRendererSystem::SimpleRendererSystem(HexDevice &device, VkRenderPass renderPass) : hexDevice{device} {
		createPipelineLayout();
		createPipeline(renderPass);
	}

	SimpleRendererSystem::~SimpleRendererSystem() {
		vkDestroyPipelineLayout(hexDevice.device(), pipelineLayout, nullptr);
	}

	void SimpleRendererSystem::createPipelineLayout() {

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

	void SimpleRendererSystem::createPipeline(VkRenderPass renderPass) {
		assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

		PipelineConfigInfo pipelineConfig{};
		HexPipeline::defaultPipelineConfigInfo(pipelineConfig);
		pipelineConfig.renderPass = renderPass;
		pipelineConfig.pipelineLayout = pipelineLayout;

		hexPipeline = std::make_unique<HexPipeline>(
			hexDevice,
			"shaders/simple_shader.vert.spv",
			"shaders/simple_shader.frag.spv",
			pipelineConfig
		);
	}

	void SimpleRendererSystem::renderGameObjectObjects(VkCommandBuffer commandBuffer, std::vector<HexGameObject> &gameObjects, const HexCamera &camera) {
		hexPipeline->bind(commandBuffer);

		auto projectionView = camera.getProjection() * camera.getViewMatrix();

		for (auto &gameObject : gameObjects) {
			// gameObject.transform.rotation.y = glm::mod(gameObject.transform.rotation.y + 0.001f, glm::two_pi<float>());
			// gameObject.transform.rotation.z = glm::mod(gameObject.transform.rotation.z + 0.002f, glm::two_pi<float>());

			SimplePushConstantData push{};
			push.color = gameObject.color;
			push.transform = projectionView * gameObject.transform.mat4();

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