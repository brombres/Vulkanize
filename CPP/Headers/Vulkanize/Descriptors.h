// Thanks to Vasif Abdullayev for this helpful article on configuring descriptors:
// https://mr-vasifabdullayev.medium.com/multiple-object-rendering-in-vulkan-3d07aa583cec
#pragma once

#include <vector>
#include "Vulkanize/Vulkanize.h"

namespace VKZ
{
  struct Descriptor
  {
    Context*               context;
    bool                   activated = false;
    uint32_t               binding;
    VkDescriptorType       type;
    uint32_t               count = 1;
    VkSampler*             immutable_samplers = nullptr;
    VkShaderStageFlags     stage;

    Descriptor( uint32_t binding, VkShaderStageFlags stage, VkDescriptorType type );
    virtual ~Descriptor(){}

    virtual bool activate( Context* context );
    virtual void deactivate();

    virtual bool configure_descriptor_set( uint32_t swap_index, VkDescriptorSet& set ) = 0;

    virtual bool on_activate() { return true; }
    virtual void on_deactivate() {}
  };

  template <typename DataType>
  struct UniformBufferDescriptor : Descriptor
  {
    // PROPERTIES
    DataType value;
    std::vector<Buffer> buffers;

    // METHODS
    UniformBufferDescriptor( uint32_t binding, VkShaderStageFlags stage )
      : Descriptor( binding, stage, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER ) {}

    bool on_activate() override
    {
      uint32_t count = context->swapchain_count;
      buffers.resize( count );
      for (uint32_t i=0; i<count; ++i)
      {
        if ( !buffers[i].create_uniform_buffer(context, sizeof(DataType)) ) return false;
        buffers[i].copy_from( &value, sizeof(DataType) );
      }

      return true;
    }

    void on_deactivate() override
    {
      uint32_t count = context->swapchain_count;
      for (uint32_t i=0; i<count; ++i)
      {
        buffers[i].destroy();
      }
    }

    bool configure_descriptor_set( uint32_t swap_index, VkDescriptorSet& set ) override
    {
      VkDescriptorBufferInfo buffer_info = {};
      buffer_info.buffer = buffers[swap_index].vk_buffer;
      buffer_info.offset = 0;
      buffer_info.range = sizeof(DataType);

      VkWriteDescriptorSet write;
      write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      write.dstSet = set;
      write.dstBinding = binding;
      write.dstArrayElement = 0;
      write.descriptorCount = 1;
      write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
      write.pBufferInfo = &buffer_info;

      context->device_dispatch.updateDescriptorSets( 1, &write, 0, nullptr );

      return true;
    }

    void set( DataType value )
    {
      this->value = value;

      for (auto& buffer : buffers)
      {
        buffer.copy_from( &value, sizeof(DataType) );
      }
    }
  };

  struct CombinedImageSamplerDescriptor : Descriptor
  {
    // PROPERTIES
    Image*    image;
    VkSampler vk_sampler;
    VkDescriptorImageInfo image_info = {};

    // METHODS
    CombinedImageSamplerDescriptor( uint32_t binding, VkShaderStageFlags stage,
                                    Image* image, VkSampler vk_sampler );

    bool configure_descriptor_set( uint32_t swap_index, VkDescriptorSet& set ) override;
  };

  struct Descriptors
  {
    // PROPERTIES
    Context*                 context = nullptr;
    std::vector<Descriptor*> descriptors;

    VkDescriptorSetLayout        layout;
    VkDescriptorPool             pool;
    std::vector<VkDescriptorSet> sets;

    bool activated = false;

    // METHODS
    ~Descriptors();

    virtual bool activate( Context* context );
    virtual void deactivate();

    virtual CombinedImageSamplerDescriptor* add_combined_image_sampler(
        uint32_t binding, VkShaderStageFlags stage, Image* image, VkSampler sampler
    );

    template <typename DataType>
    UniformBufferDescriptor<DataType>* add_uniform_buffer( uint32_t binding, VkShaderStageFlags stage )
    {
      UniformBufferDescriptor<DataType>* descriptor = new UniformBufferDescriptor<DataType>( binding, stage );
      descriptors.push_back( descriptor );
      return descriptor;
    }

    virtual bool configure_descriptor_sets( uint32_t swap_index );

    virtual Descriptor*& operator[]( size_t index )
    {
      return descriptors[index];
    }
  };
};
