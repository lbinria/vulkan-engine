#pragma once

#include "HexWindow.h"
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

		HexWindow hexWindow{WIDTH, HEIGHT, "Hello !"};
		HexDevice hexDevice{hexWindow};

		HexRenderer hexRenderer{hexWindow, hexDevice};

		std::vector<HexGameObject> gameObjects;

	};
}