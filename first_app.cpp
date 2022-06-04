#include "first_app.hpp"

#include <array>
#include <iostream>
#include <stdexcept>

namespace tarask
{
    FirstApp::FirstApp()
    {
        loadModels();
        createPipelineLayout();
        createPipeline();
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
            vertices.push_back({top});
            vertices.push_back({right});
            vertices.push_back({left});
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
        std::vector<TaraskModel::Vertex> vertices = {
            {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
            {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
            {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
        };
        // std::cout << "Starting calculating sierpinski triangle..." << std::endl;
        // sierpinski(vertices, 8, {-0.9f, 0.9f}, {0.9f, 0.9f}, {0.0f, -0.9f});
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

    void FirstApp::createPipeline()
    {
        auto pipelineConfig = TaraskPipeline::defaultPipelineConfigInfo(m_taraskSwapChain.width(),
                                                                        m_taraskSwapChain.height());
        pipelineConfig.renderPass = m_taraskSwapChain.getRenderPass();
        pipelineConfig.pipelineLayout = m_pipelineLayout;
        m_taraskPipeline =
            std::make_unique<TaraskPipeline>(m_taraskDevice, "shaders/simple_shader.vert.spv",
                                             "shaders/simple_shader.frag.spv", pipelineConfig);
    }

    void FirstApp::createCommandBuffers()
    {
        m_commandBuffers.resize(m_taraskSwapChain.imageCount());
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

        for (int i = 0; i < m_commandBuffers.size(); i++)
        {
            VkCommandBufferBeginInfo beginInfo{};
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

            if (vkBeginCommandBuffer(m_commandBuffers[i], &beginInfo) != VK_SUCCESS)
            {
                throw std::runtime_error("FirstApp: failed to begin recording command buffer!");
            }

            VkRenderPassBeginInfo renderPassInfo{};
            renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            renderPassInfo.renderPass = m_taraskSwapChain.getRenderPass();
            renderPassInfo.framebuffer = m_taraskSwapChain.getFrameBuffer(i);

            renderPassInfo.renderArea.offset = {0, 0};
            renderPassInfo.renderArea.extent = m_taraskSwapChain.getSwapChainExtent();

            std::array<VkClearValue, 2> clearValues{};
            clearValues[0].color = {0.1f, 0.1f, 0.1f, 0.1f};
            clearValues[1].depthStencil = {1.0f, 0};
            renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
            renderPassInfo.pClearValues = clearValues.data();

            vkCmdBeginRenderPass(m_commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
            m_taraskPipeline->bind(m_commandBuffers[i]);
            m_taraskModel->bind(m_commandBuffers[i]);
            m_taraskModel->draw(m_commandBuffers[i]);

            vkCmdEndRenderPass(m_commandBuffers[i]);
            if (vkEndCommandBuffer(m_commandBuffers[i]) != VK_SUCCESS)
            {
                throw std::runtime_error("FirstApp: failed to record command buffer.");
            }
        }
    }
    void FirstApp::drawFrame()
    {
        uint32_t imageIndex;
        auto result = m_taraskSwapChain.acquireNextImage(&imageIndex);
        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
        {
            throw std::runtime_error("FirstApp: failed to acquire swap chain image.");
        }
        result = m_taraskSwapChain.submitCommandBuffers(&m_commandBuffers[imageIndex], &imageIndex);
        if (result != VK_SUCCESS)
        {
            throw std::runtime_error("FirstApp: failed to present swap chain image.");
        }
    }
} // namespace tarask