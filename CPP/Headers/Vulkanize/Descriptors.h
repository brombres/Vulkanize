// Thanks to Vasif Abdullayev for this helpful article on configuring descriptors:
// https://mr-vasifabdullayev.medium.com/multiple-object-rendering-in-vulkan-3d07aa583cec
#pragma once

#include <vector>
#include "Vulkanize/Vulkanize.h"

namespace VKZ
{
  struct Descriptor
  {
    uint32_t               binding;
    uint32_t               count;
    size_t                 size;
    VkDescriptorType       type;
    VkShaderStageFlags     stage;
    VkSampler*             samplers = nullptr;
    VkDescriptorBufferInfo buffer_info;
    VkDescriptorImageInfo  image_info;

    virtual ~Descriptor(){}
  };

  template <typename DataType>
  struct UniformBufferDescriptor : Descriptor
  {
    DataType value;

    virtual ~UniformBufferDescriptor(){}
  };

  struct Descriptors
  {
    // PROPERTIES
    std::vector<Descriptor*> descriptor_info;

    VkDescriptorSetLayout layout;
    VkDescriptorPool      pool;

    // METHODS
    ~Descriptors();

    virtual Descriptor* add_combined_image_sampler( uint32_t binding, VkShaderStageFlags stage,
                                                    VkSampler* samplers, uint32_t count=1 );
    virtual Descriptor* add_descriptor( uint32_t binding, VkDescriptorType type, VkShaderStageFlags stage,
                                        VkSampler* samplers=nullptr, uint32_t count=1 );

    template <typename DataType>
    UniformBufferDescriptor<DataType>* add_uniform_buffer( uint32_t binding, VkShaderStageFlags stage, uint32_t count=1 )
    {
      UniformBufferDescriptor<DataType>* info = new UniformBufferDescriptor<DataType>();
      info->binding  = binding;
      info->count    = count;
      info->type     = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
      info->stage    = stage;
      descriptor_info.push_back( info );
      return info;
    }
  };
};
