#pragma once

#include "tarask_device.hpp"
#include "tarask_model.hpp"
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
        void loadModels();
        void sierpinski(std::vector<TaraskModel::Vertex> &vertices, int depth, glm::vec2 left,
                        glm::vec2 right, glm::vec2 top);
        void createPipelineLayout();
        void createPipeline();
        void createCommandBuffers();
        void freeCommandBuffers();
        void drawFrame();
        void recreateSwapChain();
        void recordCommandBuffer(int imageIndex);

        TaraskWindow m_taraskWindow{WIDTH, HEIGHT, "Tarask Vulkan Engine"};
        TaraskDevice m_taraskDevice{m_taraskWindow};
        std::unique_ptr<TaraskSwapChain> m_taraskSwapChain;
        std::unique_ptr<TaraskPipeline> m_taraskPipeline;
        VkPipelineLayout m_pipelineLayout;
        std::vector<VkCommandBuffer> m_commandBuffers;
        std::unique_ptr<TaraskModel> m_taraskModel;
    };
} // namespace tarask