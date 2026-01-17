#pragma once

#include "lve_device.hpp"
#include "lve_frame_info.hpp"
#include "lve_pipeline.hpp"

// std
#include <memory>

namespace lve
{
    class PointLightSystem
    {
    public:
        PointLightSystem(LveDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
        ~PointLightSystem();

        PointLightSystem(const PointLightSystem&) = delete;
        PointLightSystem& operator=(const PointLightSystem&) = delete;

        static void update(const FrameInfo& frameInfo, GlobalUbo& ubo);
        void render(const FrameInfo& frameInfo) const;

    private:
        void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
        void createPipeline(VkRenderPass renderPass);

        LveDevice& lveDevice;

        std::unique_ptr<LvePipeline> lvePipeline;
        VkPipelineLayout pipelineLayout{};
    };
}