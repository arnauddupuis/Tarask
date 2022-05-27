#pragma once

#include "tarask_device.hpp"
#include "tarask_pipeline.hpp"
#include "tarask_swap_chain.hpp"
#include "tarask_window.hpp"

#include <memory>
#include <vector>

namespace tarask
{
    class FirstApp
    {
    public:
        static constexpr int WIDTH = 800;
        static constexpr int HEIGHT = 600;

        FirstApp();
        ~FirstApp();
        FirstApp(const FirstApp &) = delete;
        FirstApp &operator=(const FirstApp &) = delete;
        void run();

    private:
        void createPipelineLayout();
        void createPipeline();
        void createCommandBuffers();
        void drawFrame();

        TaraskWindow m_taraskWindow{WIDTH, HEIGHT, "Tarask Vulkan Engine"};
        TaraskDevice m_taraskDevice{m_taraskWindow};
        TaraskSwapChain m_taraskSwapChain{m_taraskDevice, m_taraskWindow.getExtent()};
        std::unique_ptr<TaraskPipeline> m_taraskPipeline;
        VkPipelineLayout m_pipelineLayout;
        std::vector<VkCommandBuffer> m_commandBuffers;
    };
} // namespace tarask