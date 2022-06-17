#include "first_app.hpp"

#include <array>
#include <cassert>
#include <iostream>
#include <stdexcept>

namespace tarask
{
    FirstApp::FirstApp()
    {
        loadModels();
        createPipelineLayout();
        recreateSwapChain();
        createCommandBuffers();
    }
    FirstApp::~FirstApp()
    {
        vkDestroyPipelineLayout(m_taraskDevice.device(), m_pipelineLayout, nullptr);
    }
    void FirstApp::run()
    {
        while (!m_taraskWindow.shouldClose())
        {
            glfwPollEvents();
            drawFrame();
        }

        vkDeviceWaitIdle(m_taraskDevice.device());
    }

    void FirstApp::sierpinski(std::vector<TaraskModel::Vertex> &vertices, int depth, glm::vec2 left,
                              glm::vec2 right, glm::vec2 top)
    {
        if (depth <= 0)
        {
            vertices.push_back({top, {1.0f, 0.0f, 0.0f}});
            vertices.push_back({right, {0.0f, 1.0f, 0.0f}});
            vertices.push_back({left, {0.0f, 0.0f, 1.0f}});
        }
        else
        {
            auto leftTop = 0.5f * (left + top);
            auto rightTop = 0.5f * (right + top);
            auto leftRight = 0.5f * (left + right);
            sierpinski(vertices, depth - 1, left, leftRight, leftTop);
            sierpinski(vertices, depth - 1, leftRight, right, rightTop);
            sierpinski(vertices, depth - 1, leftTop, rightTop, top);
        }
    }

    void FirstApp::loadModels()
    {
        // std::vector<TaraskModel::Vertex> vertices = {
        //     {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
        //     {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
        //     {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
        // };
        // std::cout << "Starting calculating sierpinski triangle..." << std::endl;
        std::vector<TaraskModel::Vertex> vertices = {};
        sierpinski(vertices, 8, {-0.9f, 0.9f}, {0.9f, 0.9f}, {0.0f, -0.9f});
        // std::cout << "Finished calculating sierpinski triangle..." << std::endl;

        m_taraskModel = std::make_unique<TaraskModel>(m_taraskDevice, vertices);
    }

    void FirstApp::createPipelineLayout()
    {
        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0;
        pipelineLayoutInfo.pSetLayouts = nullptr;
        pipelineLayoutInfo.pushConstantRangeCount = 0;
        pipelineLayoutInfo.pPushConstantRanges = nullptr;
        if (vkCreatePipelineLayout(m_taraskDevice.device(), &pipelineLayoutInfo, nullptr,
                                   &m_pipelineLayout) != VK_SUCCESS)
        {
            throw std::runtime_error("FirstApp: failed to create pipeline layout!");
        }
    }

    void FirstApp::recreateSwapChain()
    {
        auto extent = m_taraskWindow.getExtent();
        while (extent.width == 0 || extent.height == 0)
        {
            extent = m_taraskWindow.getExtent();
            glfwWaitEvents();
        }
        vkDeviceWaitIdle(m_taraskDevice.device());
        if (m_taraskSwapChain == nullptr)
        {
            m_taraskSwapChain = std::make_unique<TaraskSwapChain>(m_taraskDevice, extent);
        }
        else
        {
            m_taraskSwapChain = std::make_unique<TaraskSwapChain>(m_taraskDevice, extent, std::move(m_taraskSwapChain));
            if (m_taraskSwapChain->imageCount() != m_commandBuffers.size())
            {
                freeCommandBuffers();
                createCommandBuffers();
            }
        }

        createPipeline();
    }

    void FirstApp::createPipeline()
    {

        assert(m_taraskSwapChain != nullptr && "Cannot create pipeline before swap chain");
        assert(m_pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

        PipelineConfigInfo pipelineConfig{};
        TaraskPipeline::defaultPipelineConfigInfo(pipelineConfig);
        pipelineConfig.renderPass = m_taraskSwapChain->getRenderPass();
        pipelineConfig.pipelineLayout = m_pipelineLayout;
        m_taraskPipeline = std::make_unique<TaraskPipeline>(
            m_taraskDevice,
            "shaders/simple_shader.vert.spv",
            "shaders/simple_shader.frag.spv",
            pipelineConfig);

        // auto pipelineConfig = TaraskPipeline::defaultPipelineConfigInfo(
        //     m_taraskSwapChain->width(), m_taraskSwapChain->height());
        // pipelineConfig.renderPass = m_taraskSwapChain->getRenderPass();
        // pipelineConfig.pipelineLayout = m_pipelineLayout;
        // m_taraskPipeline =
        //     std::make_unique<TaraskPipeline>(m_taraskDevice, "shaders/simple_shader.vert.spv",
        //                                      "shaders/simple_shader.frag.spv", pipelineConfig);
    }

    void FirstApp::createCommandBuffers()
    {
        m_commandBuffers.resize(m_taraskSwapChain->imageCount());
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = m_taraskDevice.getCommandPool();
        allocInfo.commandBufferCount = static_cast<uint32_t>(m_commandBuffers.size());
        if (vkAllocateCommandBuffers(m_taraskDevice.device(), &allocInfo,
                                     m_commandBuffers.data()) != VK_SUCCESS)
        {
            throw std::runtime_error("FirstApp: failed to allocate command buffers.");
        }
    }

    void FirstApp::freeCommandBuffers()
    {
        vkFreeCommandBuffers(m_taraskDevice.device(), m_taraskDevice.getCommandPool(),
                             static_cast<uint32_t>(m_commandBuffers.size()), m_commandBuffers.data());
        m_commandBuffers.clear();
    }
    void FirstApp::recordCommandBuffer(int imageIndex)
    {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(m_commandBuffers[imageIndex], &beginInfo) != VK_SUCCESS)
        {
            throw std::runtime_error("FirstApp: failed to begin recording command buffer!");
        }

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = m_taraskSwapChain->getRenderPass();
        renderPassInfo.framebuffer = m_taraskSwapChain->getFrameBuffer(imageIndex);

        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = m_taraskSwapChain->getSwapChainExtent();

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = {0.1f, 0.1f, 0.1f, 0.1f};
        clearValues[1].depthStencil = {1.0f, 0};
        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(m_commandBuffers[imageIndex], &renderPassInfo,
                             VK_SUBPASS_CONTENTS_INLINE);
        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(m_taraskSwapChain->getSwapChainExtent().width);
        viewport.height = static_cast<float>(m_taraskSwapChain->getSwapChainExtent().height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        VkRect2D scissor{{0, 0}, m_taraskSwapChain->getSwapChainExtent()};
        vkCmdSetViewport(m_commandBuffers[imageIndex], 0, 1, &viewport);
        vkCmdSetScissor(m_commandBuffers[imageIndex], 0, 1, &scissor);
        m_taraskPipeline->bind(m_commandBuffers[imageIndex]);
        m_taraskModel->bind(m_commandBuffers[imageIndex]);
        m_taraskModel->draw(m_commandBuffers[imageIndex]);

        vkCmdEndRenderPass(m_commandBuffers[imageIndex]);
        if (vkEndCommandBuffer(m_commandBuffers[imageIndex]) != VK_SUCCESS)
        {
            throw std::runtime_error("FirstApp: failed to record command buffer.");
        }
    }
    void FirstApp::drawFrame()
    {
        uint32_t imageIndex;
        auto result = m_taraskSwapChain->acquireNextImage(&imageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            recreateSwapChain();
            return;
        }

        // std::cout << "SWAP CHAIN RECREATED" << std::endl;

        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
        {
            throw std::runtime_error("FirstApp: failed to acquire swap chain image.");
        }
        recordCommandBuffer(imageIndex);
        result =
            m_taraskSwapChain->submitCommandBuffers(&m_commandBuffers[imageIndex], &imageIndex);
        if (result == VK_SUBOPTIMAL_KHR || result == VK_ERROR_OUT_OF_DATE_KHR ||
            m_taraskWindow.wasWindowResized())
        {
            m_taraskWindow.resetWindowResizedFlag();
            recreateSwapChain();
            return;
        }
        if (result != VK_SUCCESS)
        {
            throw std::runtime_error("FirstApp: failed to present swap chain image.");
        }
    }
} // namespace tarask