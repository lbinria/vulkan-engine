#pragma once

#include "hex_device.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace hex {
	class HexModel {
		public:

		struct Vertex {
			glm::vec2 position;
			glm::vec3 color;

			static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
			static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
		};

		HexModel(HexDevice &device, const std::vector<Vertex> &vertices);
		~HexModel();

		HexModel(const HexModel &) = delete;
		HexModel &operator=(const HexModel &) = delete;

		void bind(VkCommandBuffer commandBuffer);
		void draw(VkCommandBuffer commandBuffer);

		private:
		void createVertexBuffers(const std::vector<Vertex> &vertices);

		HexDevice &hexDevice;
		VkBuffer vertexBuffer;
		VkDeviceMemory vertexBufferMemory;
		uint32_t vertexCount;
	};
}