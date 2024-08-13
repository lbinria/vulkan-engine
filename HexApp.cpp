#include "HexApp.h"

#include "HexCamera.h"
#include "KeyboardMovementController.h"
#include "SimpleRendererSystem.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <array>
#include <stdexcept>
#include <cassert>

#include <chrono>

namespace hex {

	HexApp::HexApp() {
		loadGameObjects();
	}

	HexApp::~HexApp() {
	}

	void HexApp::run() {

		SimpleRendererSystem simpleRendererSystem{hexDevice, hexRenderer.getSwapChainRenderPass()};
		HexCamera camera{};
		
		// camera.setViewDirection(glm::vec3{0.f}, glm::vec3{0.5f, 0.f, 1.f});
		camera.setViewTarget(glm::vec3{-1.f, -2.f, 2.f}, glm::vec3{0.f, 0.f, 2.5f});

		auto viewerObject = HexGameObject::createGameObject();
		KeyboardMovementController cameraController{};

		auto currentTime = std::chrono::high_resolution_clock::now();

		while (!hexWindow.shouldClose()) {
			glfwPollEvents();

			auto newTime = std::chrono::high_resolution_clock::now();
			float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
			currentTime = newTime;

			cameraController.moveInPlaneXZ(hexWindow.getGLFWWindow(), frameTime, viewerObject);
			camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

			float aspect = hexRenderer.getAspectRatio();
			// camera.setOrthographicProjection(-aspect, aspect, -1, 1, -1, 1);
			camera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 100.f);

			if (auto commandBuffer = hexRenderer.beginFrame()) {
				hexRenderer.beginSwapChainRenderPass(commandBuffer);
				simpleRendererSystem.renderGameObjectObjects(commandBuffer, gameObjects, camera);
				hexRenderer.endSwapChainRenderPass(commandBuffer);
				hexRenderer.endFrame();
			}
						
		}

		vkDeviceWaitIdle(hexDevice.device());
	}

	std::unique_ptr<HexModel> createCubeModel(HexDevice& device, glm::vec3 offset) {
		std::vector<HexModel::Vertex> vertices{
		
			// left face (white)
			{{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
			{{-.5f, .5f, .5f}, {.9f, .9f, .9f}},
			{{-.5f, -.5f, .5f}, {.9f, .9f, .9f}},
			{{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
			{{-.5f, .5f, -.5f}, {.9f, .9f, .9f}},
			{{-.5f, .5f, .5f}, {.9f, .9f, .9f}},
		
			// right face (yellow)
			{{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
			{{.5f, .5f, .5f}, {.8f, .8f, .1f}},
			{{.5f, -.5f, .5f}, {.8f, .8f, .1f}},
			{{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
			{{.5f, .5f, -.5f}, {.8f, .8f, .1f}},
			{{.5f, .5f, .5f}, {.8f, .8f, .1f}},
		
			// top face (orange, remember y axis points down)
			{{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
			{{.5f, -.5f, .5f}, {.9f, .6f, .1f}},
			{{-.5f, -.5f, .5f}, {.9f, .6f, .1f}},
			{{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
			{{.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
			{{.5f, -.5f, .5f}, {.9f, .6f, .1f}},
		
			// bottom face (red)
			{{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
			{{.5f, .5f, .5f}, {.8f, .1f, .1f}},
			{{-.5f, .5f, .5f}, {.8f, .1f, .1f}},
			{{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
			{{.5f, .5f, -.5f}, {.8f, .1f, .1f}},
			{{.5f, .5f, .5f}, {.8f, .1f, .1f}},
		
			// nose face (blue)
			{{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
			{{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
			{{-.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
			{{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
			{{.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
			{{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
		
			// tail face (green)
			{{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
			{{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
			{{-.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
			{{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
			{{.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
			{{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
		
		};
		for (auto& v : vertices) {
			v.position += offset;
		}
		return std::make_unique<HexModel>(device, vertices);
	}

	void HexApp::loadGameObjects() {

		std::shared_ptr<HexModel> hexModel = createCubeModel(hexDevice, {.0f,.0f,.0f});

		auto cube = HexGameObject::createGameObject();
		cube.model = hexModel;
		cube.transform.translation = {.0f, .0f, 2.5f};
		cube.transform.scale = {.5f, .5f, .5f};
		gameObjects.push_back(std::move(cube));
	}

}