#pragma once

#include "HexCamera.h"
#include "HexPipeline.h"
#include "hex_device.h"
#include "HexGameObject.h"

#include <memory>
#include <vector>

namespace hex {
	class SimpleRendererSystem {
		public:

		SimpleRendererSystem(HexDevice &device, VkRenderPass renderPass);
		~SimpleRendererSystem();

		SimpleRendererSystem(const SimpleRendererSystem&) = delete;
		SimpleRendererSystem &operator=(const SimpleRendererSystem &) = delete;

		void renderGameObjectObjects(VkCommandBuffer commandBuffer, std::vector<HexGameObject> &gameObjects, const HexCamera &camera);

		private:

		void createPipelineLayout();
		void createPipeline(VkRenderPass renderPass);

		HexDevice &hexDevice;

		std::unique_ptr<HexPipeline> hexPipeline;
		VkPipelineLayout pipelineLayout;

	};
}