#pragma once

#include "lve_game_object.hpp"
#include "lve_window.hpp"

namespace lve
{
    class KeyboardMovementController
    {
    public:
        struct KeyMappings
        {
            int moveForward = GLFW_KEY_W;
            int moveBackward = GLFW_KEY_S;
            int moveRight = GLFW_KEY_D;
            int moveLeft = GLFW_KEY_A;
            int moveUp = GLFW_KEY_E;
            int moveDown = GLFW_KEY_Q;
            int lookUp = GLFW_KEY_UP;
            int lookDown = GLFW_KEY_DOWN;
            int lookRight = GLFW_KEY_RIGHT;
            int lookLeft = GLFW_KEY_LEFT;
        };

        KeyMappings keys{};
        float moveSpeed{3.0f};
        float lookSpeed{1.5f};

        void lookInPlaneXZ(GLFWwindow* window, float dt, LveGameObject& gameObject);
        void moveInPlaneXZ(GLFWwindow* window, float dt, LveGameObject& gameObject);
    };
}
