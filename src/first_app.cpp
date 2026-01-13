#include "first_app.hpp"
#include "simple_render_system.hpp"
#include "lve_camera.hpp"
#include "lve_buffer.hpp"
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
    struct GlobalUbo
    {
        glm::mat4 projectionView{1.0f};
        glm::vec3 lightDirection = glm::normalize(glm::vec3(1.0f, -3.0f, -1.0f));
    };

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
        std::vector<std::unique_ptr<LveBuffer>> uboBuffers(LveSwapChain::MAX_FRAMES_IN_FLIGHT);
        for (std::unique_ptr<LveBuffer>& uboBuffer : uboBuffers)
        {
            uboBuffer = std::make_unique<LveBuffer>
            (
                lveDevice,
                sizeof(GlobalUbo),
                1,
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
            );
            uboBuffer->map();
        }

        LveBuffer globalUboBuffer
        {
            lveDevice,
            sizeof(GlobalUbo),
            LveSwapChain::MAX_FRAMES_IN_FLIGHT,
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
            lveDevice.properties.limits.minUniformBufferOffsetAlignment,
        };
        globalUboBuffer.map();


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
                int frameIndex = lveRenderer.getCurrentFrameIndex();

                FrameInfo frameInfo
                {
                    frameIndex,
                    frameTime,
                    commandBuffer,
                    camera
                };

                // Update
                GlobalUbo ubo = {};
                ubo.projectionView = camera.getProjection() * camera.getView();
                uboBuffers[frameIndex]->writeToBuffer(&ubo);
                uboBuffers[frameIndex]->flush();

                // Render
                lveRenderer.beginSwapChainRenderPass(commandBuffer);
                simpleRenderSystem.renderGameObjects(frameInfo, gameObjects);
                lveRenderer.endSwapChainRenderPass(commandBuffer);
                lveRenderer.endFrame();
            }
        }
        vkDeviceWaitIdle(lveDevice.device());
    }

    void FirstApp::loadGameObjects()
    {
        std::shared_ptr<LveModel> lveModel = LveModel::createModelFromFile(
            lveDevice, MODEL_PATH("flat_vase.obj"));

        LveGameObject flatVase = LveGameObject::createGameObject();
        flatVase.model = lveModel;
        flatVase.transform.translation = {-0.5f, 0.5f, 2.5f};
        flatVase.transform.scale = glm::vec3{1.0f};

        gameObjects.push_back(std::move(flatVase));

        lveModel = LveModel::createModelFromFile(
            lveDevice, MODEL_PATH("smooth_vase.obj"));

        LveGameObject smoothVase = LveGameObject::createGameObject();
        smoothVase.model = lveModel;
        smoothVase.transform.translation = {0.0f, 0.5f, 2.5f};
        smoothVase.transform.scale = glm::vec3{1.0f};

        gameObjects.push_back(std::move(smoothVase));
    }
}
