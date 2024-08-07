#pragma once 

#include "HexModel.h"

#include <memory>

namespace hex {

	struct Transform2dComponent {
		glm::vec2 translation{};
		glm::vec2 scale{1.f, 1.f};
		float rotation;
		
		glm::mat2 mat2() { 
			const float s = glm::sin(rotation);
			const float c = glm::cos(rotation);
			glm::mat2 scaleMat{{scale.x, .0f}, {.0f, scale.y}};
			glm::mat2 rotMat{{c, s}, {-s, c}};
			return rotMat * scaleMat;
		}
	};

	class HexGameObject {
		public:
		using id_t = unsigned int;

		static HexGameObject createGameObject() {
			static id_t currentId = 0;
			return HexGameObject{currentId++};
		}

		HexGameObject(const HexGameObject&) = delete;
		HexGameObject &operator=(const HexGameObject&) = delete;
		HexGameObject(HexGameObject&&) = default;
		HexGameObject &operator=(HexGameObject&&) = default;

		id_t getId() { return id; }

		std::shared_ptr<HexModel> model{};
		glm::vec3 color{};
		Transform2dComponent transform2d{};

		private:
		
		HexGameObject(id_t objId) : id{objId} {}
		id_t id;

	};
}