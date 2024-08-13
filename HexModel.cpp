#include "HexModel.h"

#include <cassert>
#include <cstring>

namespace hex {
	
	HexModel::HexModel(HexDevice &device, const std::vector<Vertex> &vertices) : hexDevice{device} {
		createVertexBuffers(vertices);
	}

	HexModel::~HexModel() {
		vkDestroyBuffer(hexDevice.device(), vertexBuffer, nullptr);
		vkFreeMemory(hexDevice.device(), vertexBufferMemory, nullptr);
	}

	void HexModel::createVertexBuffers(const std::vector<Vertex> &vertices) {
		// Set the number of vertex
		vertexCount = static_cast<uint32_t>(vertices.size());
		
		// Need at least 3 point to display a triangle
		assert(vertexCount >= 3 && "Vertex count must be at least 3");
		
		// Compute buffer size of the vertex buffer
		VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount;

		// Create a buffer for vertex buffer
		hexDevice.createBuffer(
			bufferSize, 
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, 
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			vertexBuffer,
			vertexBufferMemory
		);

		void *data;
		// Create a region of host memory mapped to device memory
		// and sets data to point to the beginning of the mapped memory range
		vkMapMemory(hexDevice.device(), vertexBufferMemory, 0, bufferSize, 0, &data);
		// Copy from vertices to data mapped memory
		memcpy(data, vertices.data(), static_cast<size_t>(bufferSize));
		vkUnmapMemory(hexDevice.device(), vertexBufferMemory);

	}

	void HexModel::draw(VkCommandBuffer commandBuffer) {
		vkCmdDraw(commandBuffer, vertexCount, 1, 0, 0);
	}

	void HexModel::bind(VkCommandBuffer commandBuffer) {
		VkBuffer buffers[] = {vertexBuffer};
		VkDeviceSize offsets[] = {0};
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
	}

	std::vector<VkVertexInputBindingDescription> HexModel::Vertex::getBindingDescriptions() {
		// Binding description, describe binding between vertex buffer and graphic pipeline
		std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
		bindingDescriptions[0].binding = 0; // Here we have only one binding and one vertex buffer (so binding = 0)
		bindingDescriptions[0].stride = sizeof(Vertex); // Each element of vertex buffer contains one vertex
		bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX; // Input rate to graphic pipeline I suppose (set to input rate for vertex seems nice)
		return bindingDescriptions;
	}

	std::vector<VkVertexInputAttributeDescription> HexModel::Vertex::getAttributeDescriptions() {
		// Attribute description, describe attribute in vertex buffer (what this vertex buffer contains)
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions(2);
		attributeDescriptions[0].binding = 0; // Associate to binding 0
		attributeDescriptions[0].location = 0; // Location 0 (see vertex shader, it means position)
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT; // This is the format data expected to represent a position
		attributeDescriptions[0].offset = offsetof(Vertex, position);

		attributeDescriptions[1].binding = 0; // Associate to binding 0
		attributeDescriptions[1].location = 1; // Location 1 (see vertex shader, it means fragColor)
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT; // This is the format data expected to represent a color
		attributeDescriptions[1].offset = offsetof(Vertex, color);

		return attributeDescriptions;
	}
}