#pragma once

#include <string>
#include <vector>

#include "tarask_device.hpp"

namespace tarask
{
    struct PipelineConfigInfo
    {
        VkViewport viewport;
        VkRect2D scissor;
        VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
        VkPipelineRasterizationStateCreateInfo rasterizationInfo;
        VkPipelineMultisampleStateCreateInfo multisampleInfo;
        VkPipelineColorBlendAttachmentState colorBlendAttachment;
        VkPipelineColorBlendStateCreateInfo colorBlendInfo;
        VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
        VkPipelineLayout pipelineLayout = nullptr;
        VkRenderPass renderPass = nullptr;
        uint32_t subpass = 0;
    };
    class TaraskPipeline
    {
    public:
        TaraskPipeline(TaraskDevice &device, const std::string &vertexShaderPath,
                       const std::string &fragmentShaderPath, const PipelineConfigInfo &configInfo);
        ~TaraskPipeline();

        TaraskPipeline() = default;

        TaraskPipeline(const TaraskPipeline &) = delete;
        TaraskPipeline &operator=(const TaraskPipeline &) = delete;

        void bind(VkCommandBuffer commandBuffer);
        static PipelineConfigInfo defaultPipelineConfigInfo(u_int32_t width, uint32_t height);

    private:
        static std::vector<char> readFile(const std::string &filePath);
        void createGraphicPipeline(const std::string &vertexShaderPath,
                                   const std::string &fragmentShaderPath,
                                   const PipelineConfigInfo &configInfo);

        void createShaderModule(const std::vector<char> &code, VkShaderModule *shaderModule);

        TaraskDevice &m_taraskDevice;
        VkPipeline m_graphicsPipeline;
        VkShaderModule m_vertexShaderModule;
        VkShaderModule m_fragmentShaderModule;
    };
} // namespace tarask