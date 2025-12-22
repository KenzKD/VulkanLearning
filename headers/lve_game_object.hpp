#pragma once

#include "lve_model.hpp"

// std
#include <memory>

namespace lve
{
    struct Transform2DComponent
    {
        glm::vec2 translation{};
        glm::vec2 scale{1.0f, 1.0f};
        float rotation;

        glm::mat2 mat2()
        {
            const float sinRot = glm::sin(rotation);
            const float cosRot = glm::cos(rotation);

            glm::mat2 rotMat{{cosRot, sinRot}, {-sinRot, cosRot}};
            glm::mat2 scaleMat{{scale.x, 0.0f}, {0.0f, scale.y}};

            return rotMat * scaleMat;
        }
    };

    class LveGameObject
    {
    public:
        using id_t = unsigned int;

        static LveGameObject createGameObject()
        {
            static id_t currentId = 0;
            return LveGameObject{currentId++};
        }

        LveGameObject(const LveGameObject&) = delete;
        LveGameObject& operator=(const LveGameObject&) = delete;
        LveGameObject(LveGameObject&&) = default;
        LveGameObject& operator=(LveGameObject&&) = default;

        id_t getId()
        {
            return id;
        }

        std::shared_ptr<LveModel> model{};
        glm::vec3 color{};
        Transform2DComponent transform2d;

    private:
        LveGameObject(id_t objId) : id{objId}
        {
        }

        id_t id;
    };
}
