#include "HexApp.h"
#include "SimpleRendererSystem.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <array>
#include <stdexcept>
#include <cassert>

namespace hex {

	HexApp::HexApp() {
		loadGameObjects();
	}

	HexApp::~HexApp() {
	}

	void HexApp::run() {

		SimpleRendererSystem simpleRendererSystem{hexDevice, hexRenderer.getSwapChainRenderPass()};

		while (!hexWindow.shouldClose()) {
			glfwPollEvents();
			
			if (auto commandBuffer = hexRenderer.beginFrame()) {
				hexRenderer.beginSwapChainRenderPass(commandBuffer);
				simpleRendererSystem.renderGameObjectObjects(commandBuffer, gameObjects);
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

}