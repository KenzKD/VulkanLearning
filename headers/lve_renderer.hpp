#pragma once

#include "lve_window.hpp"
#include "lve_device.hpp"
#include "lve_swap_chain.hpp"

// std
#include <memory>
#include <vector>
#include <cassert>

namespace lve
{
    class LveRenderer
    {
    public:
        LveRenderer(LveWindow& window, LveDevice& device);
        ~LveRenderer();

        LveRenderer(const LveRenderer&) = delete;
        LveRenderer& operator=(const LveRenderer&) = delete;

        bool isFrameInProgress() const
        {
            return isFrameStarted;
        };

        VkCommandBuffer getCurrentCommandBuffer() const
        {
            assert(isFrameStarted && "Cannot get command buffer when frame not in progress");
            return commandBuffers[currentFrameIndex];
        };

        int getCurrentFrameIndex() const
        {
            assert(isFrameStarted && "Cannot get frame index when frame not in progress");
            return currentFrameIndex;
        };

        VkRenderPass getSwapChainRenderPass() const
        {
            return lveSwapChain->getRenderPass();
        };

        float getAspectRatio() const
        {
            return lveSwapChain->extentAspectRatio();
        }

        VkCommandBuffer beginFrame();
        void endFrame();
        void beginSwapChainRenderPass(VkCommandBuffer commandBuffer) const;
        void endSwapChainRenderPass(VkCommandBuffer commandBuffer) const;

    private:
        void createCommandBuffers();
        void freeCommandBuffers();
        void recreateSwapChain();

        LveWindow& lveWindow;
        LveDevice& lveDevice;
        std::unique_ptr<LveSwapChain> lveSwapChain;
        std::vector<VkCommandBuffer> commandBuffers;

        uint32_t currentImageIndex;
        int currentFrameIndex{0};
        bool isFrameStarted = false;
    };
}
