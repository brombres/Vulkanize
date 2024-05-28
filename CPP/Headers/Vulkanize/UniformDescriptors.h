// Thanks to Vasif Abdullayev for this helpful article on configuring descriptors:
// https://mr-vasifabdullayev.medium.com/multiple-object-rendering-in-vulkan-3d07aa583cec
#pragma once

#include <vector>
#include "Vulkanize/Vulkanize.h"

namespace VKZ
{
  struct Uniform
  {
    uint32_t               binding;
    uint32_t               count;
    VkDescriptorType       type;
    VkShaderStageFlags     stage;
    VkSampler*             samplers;
    VkDescriptorBufferInfo buffer_info;
    VkDescriptorImageInfo  image_info;
  };

  struct UniformDescriptors
  {
    std::vector<VkDescriptorSetLayoutBinding> bindings;
  };
};
