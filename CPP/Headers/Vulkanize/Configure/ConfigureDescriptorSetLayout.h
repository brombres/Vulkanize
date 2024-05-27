#pragma once

#include <vector>
#include "Vulkanize/Vulkanize.h"

namespace VKZ
{
  struct ConfigureDescriptorSetLayout : ContextOperation<Context>
  {
    VkDescriptorSetLayout* layout;
    std::vector<VkDescriptorSetLayoutBinding> bindings;

    ConfigureDescriptorSetLayout( VkDescriptorSetLayout* layout ) : layout(layout) {}

    virtual void add_binding( uint32_t index, VkDescriptorType type=VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                              VkShaderStageFlags stage=VK_SHADER_STAGE_VERTEX_BIT,
                              uint32_t count=1, VkSampler* samplers=nullptr );

    // Override and call add_binding().
    // void on_configure() override;

    bool on_activate() override;
    void on_deactivate() override;
  };
};
