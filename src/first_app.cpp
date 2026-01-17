#include "first_app.hpp"

#include "keyboard_movement_controller.hpp"
#include "lve_buffer.hpp"
#include "lve_camera.hpp"
#include "systems/point_light_system.hpp"
#include "systems/simple_render_system.hpp"

#define SHADER_PATH(x) "shaders/" x
#define MODEL_PATH(x) "C:/Users/Lenovo/Desktop/Game_Making/Vulkan_Stuff/VulkanLearning/models/" x

// Libraries
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// std
#include<cassert>
#include <chrono>

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
        globalPool = LveDescriptorPool::Builder(lveDevice)
                     .setMaxSets(LveSwapChain::MAX_FRAMES_IN_FLIGHT)
                     .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, LveSwapChain::MAX_FRAMES_IN_FLIGHT)
                     .build();

        loadGameObjects();
    }

    FirstApp::~FirstApp() = default;

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

        std::unique_ptr<LveDescriptorSetLayout> globalSetLayout
            = LveDescriptorSetLayout::Builder(lveDevice)
              .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
              .build();

        std::vector<VkDescriptorSet> globalDescriptorSets(LveSwapChain::MAX_FRAMES_IN_FLIGHT);
        for (size_t i = 0; i < globalDescriptorSets.size(); i++)
        {
            VkDescriptorBufferInfo bufferInfo = uboBuffers[i]->descriptorInfo();
            LveDescriptorWriter(*globalSetLayout, *globalPool)
                .writeBuffer(0, &bufferInfo)
                .build(globalDescriptorSets[i]);
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

        SimpleRenderSystem simpleRenderSystem
            (lveDevice, lveRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout());
        PointLightSystem pointLightSystem
            (lveDevice, lveRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout());

        LveCamera camera{};
        camera.setViewTarget(glm::vec3(-1.0f, -2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 2.5f));

        LveGameObject viewerObject = LveGameObject::createGameObject();
        viewerObject.transform.translation.z = -2.5f;


        std::chrono::time_point<std::chrono::system_clock> currentTime = std::chrono::high_resolution_clock::now();
        while (!lveWindow.shouldClose())
        {
            KeyboardMovementController cameraController{};
            glfwPollEvents();

            std::chrono::time_point<std::chrono::system_clock> newTime = std::chrono::high_resolution_clock::now();
            float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
            currentTime = newTime;

            // cameraController.lookInPlaneXZ(lveWindow.getGLFWWindow(), frameTime, viewerObject);
            cameraController.moveInPlaneXZ(lveWindow.getGLFWWindow(), frameTime, viewerObject);
            camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

            float aspect = lveRenderer.getAspectRatio();
            // camera.setOrthographicProjection(-aspect, aspect, -1, 1, -1, 1);
            camera.setPerspectiveProjection(glm::radians(50.0f), aspect, 0.1f, 100.0f);

            if (VkCommandBuffer commandBuffer = lveRenderer.beginFrame())
            {
                int frameIndex = lveRenderer.getCurrentFrameIndex();

                FrameInfo frameInfo
                {
                    frameIndex,
                    frameTime,
                    commandBuffer,
                    camera,
                    globalDescriptorSets[frameIndex],
                    gameObjects
                };

                // Update
                GlobalUbo ubo = {};
                ubo.projectionMatrix = camera.getProjection();
                ubo.viewMatrix = camera.getView();
                ubo.inverseViewMatrix = camera.getInverseView();
                pointLightSystem.update(frameInfo, ubo);
                uboBuffers[frameIndex]->writeToBuffer(&ubo);
                uboBuffers[frameIndex]->flush();

                // Render
                lveRenderer.beginSwapChainRenderPass(commandBuffer);

                // Render Order
                simpleRenderSystem.renderGameObjects(frameInfo);
                pointLightSystem.render(frameInfo);

                lveRenderer.endSwapChainRenderPass(commandBuffer);
                lveRenderer.endFrame();
            }
        }
        vkDeviceWaitIdle(lveDevice.device());
    }

    void FirstApp::loadGameObjects()
    {
        // Flat Vase
        std::shared_ptr<LveModel> lveModel = LveModel::createModelFromFile(
            lveDevice, MODEL_PATH("flat_vase.obj"));
        LveGameObject flatVase = LveGameObject::createGameObject();
        flatVase.model = lveModel;
        flatVase.transform.translation = {-0.5f, 0.5f, 0.0f};;
        flatVase.transform.scale = {3.0f, 1.5f, 3.0f};
        gameObjects.emplace(flatVase.getId(), std::move(flatVase));

        // Smooth Vase
        lveModel = LveModel::createModelFromFile(
            lveDevice, MODEL_PATH("smooth_vase.obj"));
        LveGameObject smoothVase = LveGameObject::createGameObject();
        smoothVase.model = lveModel;
        smoothVase.transform.translation = {0.5f, 0.5f, 0.0f};
        smoothVase.transform.scale = {3.0f, 1.5f, 3.0f};
        gameObjects.emplace(smoothVase.getId(), std::move(smoothVase));

        // Floor
        lveModel = LveModel::createModelFromFile(
            lveDevice, MODEL_PATH("quad.obj"));
        LveGameObject floor = LveGameObject::createGameObject();
        floor.model = lveModel;
        floor.transform.translation = {0.0f, 0.5f, 0.0f};
        floor.transform.scale = {3.0f, 1.0f, 3.0f};
        gameObjects.emplace(floor.getId(), std::move(floor));

        // Point Lights
        std::vector<glm::vec3> lightColors
        {
            {1.f, .1f, .1f},
            {.1f, .1f, 1.f},
            {.1f, 1.f, .1f},
            {1.f, 1.f, .1f},
            {.1f, 1.f, 1.f},
            {1.f, 1.f, 1.f}
        };

        for (int i = 0; i < lightColors.size(); i++)
        {
            LveGameObject pointLight = LveGameObject::makePointLight(0.2f);
            pointLight.color = lightColors[i];
            glm::mat<4, 4, float> rotateLight =
                glm::rotate
                (
                    glm::mat4(1.0f),
                    i * glm::two_pi<float>() / lightColors.size(),
                    {0.0f, -1.f, 0.0f}
                );
            pointLight.transform.translation = glm::vec3(rotateLight * glm::vec4(-1.0f, -1.0f, -1.0f, 1.0f));

            gameObjects.emplace(pointLight.getId(), std::move(pointLight));
        }
    }
}
