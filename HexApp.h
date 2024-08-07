#pragma once

#include "HexWindow.h"
#include "HexPipeline.h"
#include "hex_device.h"
#include "HexRenderer.h"
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
		void renderGameObjectObjects(VkCommandBuffer commandBuffer);

		HexWindow hexWindow{WIDTH, HEIGHT, "Hello !"};
		HexDevice hexDevice{hexWindow};

		HexRenderer hexRenderer{hexWindow, hexDevice};

		std::unique_ptr<HexPipeline> hexPipeline;
		VkPipelineLayout pipelineLayout;
		std::vector<HexGameObject> gameObjects;

	};
}