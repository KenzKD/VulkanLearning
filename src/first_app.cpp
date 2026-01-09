#include "first_app.hpp"
#include "simple_render_system.hpp"
#include "lve_camera.hpp"
#include "keyboard_movement_controller.hpp"
#define SHADER_PATH(x) "shaders/" x
#define MODEL_PATH(x) "C:/Users/Lenovo/Desktop/Game_Making/Vulkan_Stuff/VulkanLearning/models/" x

// Libraries
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// std
#include <array>
#include <chrono>
#include <stdexcept>
#include<cassert>

namespace lve
{
    struct SimplePushConstantData
    {
        glm::mat2 transform{1.f};
        glm::vec2 offset;
        alignas(16) glm::vec3 color;
    };

    FirstApp::FirstApp()
    {
        loadGameObjects();
    }

    FirstApp::~FirstApp()
    {
    }

    void FirstApp::run()
    {
        SimpleRenderSystem simpleRenderSystem(lveDevice, lveRenderer.getSwapChainRenderPass());
        LveCamera camera{};
        // camera.setViewDirection(glm::vec3(0.0f), glm::vec3(0.5f, 0.0f, 1.0f));
        camera.setViewTarget(glm::vec3(-1.0f, -2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 2.5f));

        LveGameObject viewerObject = LveGameObject::createGameObject();
        KeyboardMovementController cameraController{};


        std::chrono::time_point<std::chrono::system_clock> currentTime = std::chrono::high_resolution_clock::now();
        while (!lveWindow.shouldClose())
        {
            glfwPollEvents();

            std::chrono::time_point<std::chrono::system_clock> newTime = std::chrono::high_resolution_clock::now();
            float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
            currentTime = newTime;

            // cameraController.lookInPlaneXZ(lveWindow.getGLFWWindow(), frameTime, viewerObject);
            cameraController.moveInPlaneXZ(lveWindow.getGLFWWindow(), frameTime, viewerObject);
            camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

            float aspect = lveRenderer.getAspectRatio();
            // camera.setOrthographicProjection(-aspect, aspect, -1, 1, -1, 1);
            camera.setPerspectiveProjection(glm::radians(50.0f), aspect, 0.1f, 10.0f);

            if (VkCommandBuffer commandBuffer = lveRenderer.beginFrame())
            {
                lveRenderer.beginSwapChainRenderPass(commandBuffer);
                simpleRenderSystem.renderGameObjects(commandBuffer, gameObjects, camera);
                lveRenderer.endSwapChainRenderPass(commandBuffer);
                lveRenderer.endFrame();
            }
        }
        vkDeviceWaitIdle(lveDevice.device());
    }

    void FirstApp::loadGameObjects()
    {
        std::shared_ptr<LveModel> gameObjectModel = LveModel::createModelFromFile(
            lveDevice, MODEL_PATH("smooth_vase.obj"));

        LveGameObject gameObj = LveGameObject::createGameObject();
        gameObj.model = std::move(gameObjectModel);
        gameObj.transform.translation = {0.0f, 0.0f, 2.5f};
        gameObj.transform.scale = glm::vec3{1.0f};

        gameObjects.push_back(std::move(gameObj));
    }
}
