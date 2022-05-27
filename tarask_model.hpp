#pragma once

#include "tarask_device.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <vector>

namespace tarask
{
    class TaraskModel
    {

    public:
        struct Vertex
        {
            glm::vec2 position;

            static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
            static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
        };

        TaraskModel(TaraskDevice &device, const std::vector<Vertex> &vertices);
        ~TaraskModel();
        TaraskModel(const TaraskModel &) = delete;
        TaraskModel &operator=(const TaraskModel &) = delete;

        void bind(VkCommandBuffer commandBuffer);
        void draw(VkCommandBuffer commandBuffer);

    private:
        void createVertexBuffer(const std::vector<Vertex> &vertices);

        TaraskDevice &taraskDevice;
        VkBuffer vertexBuffer;
        VkDeviceMemory vertexBufferMemory;
        uint32_t vertexCount;
    };

}
