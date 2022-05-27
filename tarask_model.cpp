#include "tarask_model.hpp"

#include <cassert>
#include <cstring>

namespace tarask
{
    TaraskModel::TaraskModel(TaraskDevice &device, const std::vector<Vertex> &vertices)
        : taraskDevice{device}
    {
        createVertexBuffer(vertices);
    }
    TaraskModel::~TaraskModel()
    {
        vkDestroyBuffer(taraskDevice.device(), vertexBuffer, nullptr);
        vkFreeMemory(taraskDevice.device(), vertexBufferMemory, nullptr);
    }

    void TaraskModel::createVertexBuffer(const std::vector<Vertex> &vertices)
    {
        vertexCount = static_cast<uint32_t>(vertices.size());
        assert(vertexCount >= 3 && "Tarask::Model::vertexCount must be at least 3.");
        VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount;
        taraskDevice.createBuffer(bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                                  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                      VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                  vertexBuffer, vertexBufferMemory);
        void *data;
        vkMapMemory(taraskDevice.device(), vertexBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, vertices.data(), static_cast<size_t>(bufferSize));
        vkUnmapMemory(taraskDevice.device(), vertexBufferMemory);
    }

    void TaraskModel::bind(VkCommandBuffer commandBuffer)
    {
        VkBuffer vertexBuffers[] = {vertexBuffer};
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
    }

    void TaraskModel::draw(VkCommandBuffer commandBuffer)
    {
        vkCmdDraw(commandBuffer, vertexCount, 1, 0, 0);
    }

    std::vector<VkVertexInputBindingDescription> TaraskModel::Vertex::getBindingDescriptions()
    {
        std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
        bindingDescriptions[0].binding = 0;
        bindingDescriptions[0].stride = sizeof(Vertex);
        bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return bindingDescriptions;
    }

    std::vector<VkVertexInputAttributeDescription> TaraskModel::Vertex::getAttributeDescriptions()
    {
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions(1);
        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[0].offset = 0;
        return attributeDescriptions;
    }
}