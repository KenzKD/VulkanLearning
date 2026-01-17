#include "systems/point_light_system.hpp"
#define SHADER_PATH(x) "shaders/" x

// Libraries
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

// std
#include<cassert>
#include<map>
#include <ranges>
#include <stdexcept>

namespace lve
{
    struct PointLightPushConstants
    {
        glm::vec4 position{};
        glm::vec4 color{};
        float radius;
    };

    PointLightSystem::PointLightSystem
    (LveDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout) : lveDevice(device)
    {
        createPipelineLayout(globalSetLayout);
        createPipeline(renderPass);
    }

    PointLightSystem::~PointLightSystem()
    {
        vkDestroyPipelineLayout(lveDevice.device(), pipelineLayout, nullptr);
    }

    void PointLightSystem::update(const FrameInfo& frameInfo, GlobalUbo& ubo)
    {
        const glm::mat<4, 4, float> rotateLight =
            glm::rotate
            (
                glm::mat4(1.0f),
                frameInfo.frameTime,
                {0.0f, -1.f, 0.0f}
            );

        int lightIndex = 0;
        for (std::pair<const unsigned, LveGameObject>& kv : frameInfo.gameObjects)
        {
            LveGameObject& obj = kv.second;
            if (obj.pointLight == nullptr)
            {
                continue;
            }

            assert(lightIndex <MAX_LIGHTS && "Point lights Exceed Maximum Specified");

            obj.transform.translation = glm::vec3(rotateLight * glm::vec4(obj.transform.translation, 1.0f));

            ubo.pointLights[lightIndex].position = glm::vec4(obj.transform.translation, 1.0f);
            ubo.pointLights[lightIndex].color = glm::vec4(obj.color, obj.pointLight->lightIntensity);
            lightIndex += 1;
        }
        ubo.numLights = lightIndex;
    }

    void PointLightSystem::render(const FrameInfo& frameInfo) const
    {
        // Sort Lights
        std::map<float, LveGameObject::id_t> sorted;

        for (std::pair<const unsigned, LveGameObject>& kv : frameInfo.gameObjects)
        {
            LveGameObject& obj = kv.second;
            if (obj.pointLight == nullptr)
            {
                continue;
            }

            // calculate distance
            glm::vec<3, float> offset = frameInfo.camera.getPosition() - obj.transform.translation;
            float disSquared = glm::dot(offset, offset);
            sorted[disSquared] = obj.getId();
        }

        lvePipeline->bind(frameInfo.commandBuffer);
        vkCmdBindDescriptorSets
        (
            frameInfo.commandBuffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            pipelineLayout,
            0,
            1,
            &frameInfo.globalDescriptorSet,
            0,
            nullptr
        );

        for (std::pair<const float, unsigned>& it : std::ranges::reverse_view(sorted))
        {
            LveGameObject& obj = frameInfo.gameObjects.at(it.second);

            PointLightPushConstants push{};
            push.position = glm::vec4(obj.transform.translation, 1.0f);
            push.color = glm::vec4(obj.color, obj.pointLight->lightIntensity);
            push.radius = obj.transform.scale.x;

            vkCmdPushConstants
            (
                frameInfo.commandBuffer,
                pipelineLayout,
                VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                0,
                sizeof(PointLightPushConstants),
                &push
            );

            vkCmdDraw(frameInfo.commandBuffer, 6, 1, 0, 0);
        }
    }

    void PointLightSystem::createPipelineLayout(VkDescriptorSetLayout globalSetLayout)
    {
        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(PointLightPushConstants);

        const std::vector<VkDescriptorSetLayout> descriptorSetLayouts{globalSetLayout};

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
        pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

        if (vkCreatePipelineLayout(lveDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create pipeline layout!");
        }
    }

    void PointLightSystem::createPipeline(VkRenderPass renderPass)
    {
        assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

        PipelineConfigInfo pipelineConfig{};
        LvePipeline::defaultPipelineConfigInfo(pipelineConfig);
        LvePipeline::enableAlphaBlending(pipelineConfig);
        pipelineConfig.attributeDescriptions.clear();
        pipelineConfig.bindingDescriptions.clear();

        pipelineConfig.renderPass = renderPass;
        pipelineConfig.pipelineLayout = pipelineLayout;

        lvePipeline = std::make_unique<LvePipeline>
        (
            lveDevice,
            SHADER_PATH("point_light.vert.spv"),
            SHADER_PATH("point_light.frag.spv"),
            pipelineConfig
        );
    }
}
